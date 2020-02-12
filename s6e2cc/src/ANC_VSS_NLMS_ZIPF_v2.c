// ANC_VSS_NLMS_ZIPF.c
#include "audio.h"
#include "my_functions.h"
#include "arm_math.h"
//#include "gpio.h"
#include "uart_io.h"
#include "dt.h"
#include "my_vss_nlms_zipf.h"

#include "stdio.h"

#define N   128

#define tempo_inicial 0x1FFFFFF
#define tempo_de_medicao 0x32

char buffer[10];

char uart_command;

void confere_uart(){

		// verifica se tem algo no scanf()
		uart_scanf(&uart_command);	
					
		// se o uart_command for igual a '1' (enviado via UART pelo computador)
		// reseta as variáveis do algoritmo VSS_NLMS de Benesty
		// e torna uart_command == '0'
		if (uart_command == '1'){
				uart_printf("reseta as variáveis! \n");
				init_vss_nlms_zipf();
				uart_command = '0';
		}
}

void run_filter(uint32_t *txbuf, uint32_t *rxbuf, int M)
{
		uint32_t audio_in, audio_out;

		int ii=0;
	
		while(ii < M) {
				// audio de entrada
				audio_in = rxbuf[ii];
			
				audio_chR = (audio_in & 0x0000FFFF);

				audio_chL = (audio_in>>16 & 0x0000FFFF);
						
				// referência de ruído
				refnoise = (float32_t)audio_chL;
		
				//desejado
				d = (float32_t)audio_chR;
		
				//shift do vetor "x"		
				memcpy(&x[1], &x[0], (L-1)*sizeof(float32_t));
	
				//novo valor de referência armazenado
				x[0]=refnoise;
	
				//energia do sinal "x".
				arm_power_f32(x,L, &energy);	
								
				//estima a saída "d_hat"
				arm_dot_prod_f32(w,x,L,&d_hat); 	

				//erro.
				e_atual = d-d_hat;
		
				// CALCULO DE mu
			
				// calculo do valor de p
				p_atual = (beta*p_anterior) + ((1-beta)*e_atual*e_anterior);
				
				// calculo do valor de q
				q_atual = (beta*q_anterior) + ((1-beta)*e_atual*e_atual);
				
				// adicionado para evitar a divisão por zero
				if (q_atual == 0){ q_atual = 1;}
								
				// variável auxiliar (reduzir as divisões)
				aux = p_atual/(q_atual);
				
				// calculo do valor de alfa
				alfa = aux*aux;
				
				// Calculo do valor de mu
				mu=alfa/(energy+ep);
				
				// fator de adaptação fact[].
				arm_scale_f32(x,mu*e_atual,fact,(uint32_t)L);
								
				//atualiza os coeficientes do filtro.
				arm_add_f32(w, fact, w, L);
												
				//out
				int_out = (int16_t)e_atual;
				
				// atualização das variáveis
				p_anterior = p_atual;
				q_anterior = q_atual;
				e_anterior = e_atual;
								
				audio_out = ((int_out<<16 & 0xFFFF0000)) + (int_out & 0x0000FFFF);
						
				// audio de saída
				txbuf[ii] = audio_out;	
				ii++;
		}

}

void proces_buffer(void) 
{
    uint32_t *txbuf, *rxbuf;

     
    // endereços de para o buffer de transporte e de recepção
    if(tx_proc_buffer == PING) txbuf = dma_tx_buffer_ping; 
    else txbuf = dma_tx_buffer_pong; 
    if(rx_proc_buffer == PING) rxbuf = dma_rx_buffer_ping; 
    else rxbuf = dma_rx_buffer_pong; 
    
		// escreve no timer o tempo inicial (número inicial) de contagem (decrescente)
		Dt_WriteLoadVal(tempo_inicial, DtChannel0);
	
		// Habilita a contagem
		Dt_EnableCount(DtChannel0);
	
		// Filtro projetado
		run_filter(txbuf, rxbuf, DMA_BUFFER_SIZE);
		
		// Desabilita a contagem
		Dt_DisableCount(DtChannel0);
		
		// Converte para string a diferença entre o tempo inicial e o valor do contador
		sprintf(buffer, "\n%i", tempo_inicial - Dt_ReadCurCntVal(DtChannel0)- tempo_de_medicao);
		
		// Passa via UART o tempo (número de clocks)
		uart_printf(buffer);
    
    tx_buffer_empty = 0;
    rx_buffer_full = 0;
}

//Main function
int main (void) { 
    
		// inicializa a comunicação UART - apenas o printf (saída)
		Uart_Io_Init();/* Initializatio of the UART unit and GPIO used in the communication */
		
		// inicializa o dual timer, se não iniciar corretamente 
		// apresenta uma mensagem de erro
  	if (my_dt() != 0)
		{
				uart_printf("erro na conffiguração do dual timer!\n");
				while(1);
		};
		
		// inicializa as variáveis do filtro
		init_vss_nlms_zipf();
		
		// inicializa o CODEC
    audio_init (hz8000, line_in, dma, DMA_HANDLER);
	

		while(1){

				// enquanto o buffer de recepção não estiver cheio 
			  // e o de transmissão vaziu
			
				while (!(rx_buffer_full && tx_buffer_empty)){};

//					// para contar o tempo total disponível para processameento de 128 amostras
//				Dt_DisableCount(DtChannel0);
//				
//				sprintf(buffer, "\n%i", tempo_inicial - Dt_ReadCurCntVal(DtChannel0) - tempo_de_medicao);
//				
//				uart_printf(buffer);
//							
//				Dt_WriteLoadVal(tempo_inicial, DtChannel0);
//			
//				Dt_EnableCount(DtChannel0);
					
				// processa o buffer	
				proces_buffer();
					
				confere_uart();

		}

}
