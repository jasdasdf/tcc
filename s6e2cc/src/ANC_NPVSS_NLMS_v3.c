// ANC_NPVSS_NLMS_v1.c 

#include "audio.h"
#include "my_functions.h"
#include "arm_math.h"
#include "uart_io.h"
#include "dt.h"

#include "stdio.h"

#define tempo_inicial 0x1FFFFFF
#include "my_npvss_nlms.h"

#define N   128

char buffer[10];

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
				memmove(&x[1], &x[0], (L-1)*sizeof(float32_t));
	
				//shift do vetor "error"		
				memmove(&error[1], &error[0], (L-1)*sizeof(float32_t));
				
				//novo valor de referência armazenado
				x[0]=refnoise;
						
				//estimativa gerada pelo filtro
				arm_dot_prod_f32(x, w, L, &d_hat);
	
				//erro.
				e = d-d_hat;
				
				//armazena o sinal de erro
				error[0]= e;	
				
				delta = 20*var_x;
				
				//////// ESTIMATIVA DA VARIÂNCIA DO SINAL DE FALA ////////
				
				
				//variância do sinal "x".
				arm_power_f32(x,L, &power_x);
				
				// adicionado para evitar a divisão por zero
				if(power_x==0){power_x=1;}
				
				var_x = (1.0/L)*power_x;
				
				//estimativa da correlação entre "x" e "e"
				arm_scale_f32(cor_error_x, lambda, fact_01, L);
				arm_scale_f32(x, (1-lambda)*e, fact_02, L);
				arm_add_f32(fact_01, fact_02, cor_error_x, L);
				arm_power_f32(cor_error_x, L, &power_cor_error_x);

				//estimativa da variancia do sinal de erro
				//arm_var_f32(error, L, &var_e);
				arm_power_f32(error,L, &power_error);
				var_e = (1.0/L)*power_error;  
					
				//variância do sinal de fala (ruído de medição)
				var_v = var_e - (power_cor_error_x/var_x);
				
				///////////////////////////////////////////////////////
				
				//desvio padrão 
				arm_sqrt_f32(var_e, &desv_e);
				arm_sqrt_f32(var_v, &desv_v);
	
				//fator beta
				beta = (1/(delta + power_x))*(1 - (desv_v/(epslon + desv_e)));
				
				if (desv_e >= desv_v){
						mu_npvss = beta;
				}
				else{
						mu_npvss = 0;
				}

				arm_scale_f32(x, mu_npvss*e, v_aux, L);
				arm_add_f32(w, v_aux, w,  L);
				
				//saída
				int_out = (int16_t)e;
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
		sprintf(buffer, "\n%i", tempo_inicial - Dt_ReadCurCntVal(DtChannel0));
		
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
		
		// inicializa o CODEC
    audio_init (hz8000, line_in, dma, DMA_HANDLER);
	

		while(1){

				// enquanto o buffer de recepção não estiver cheio 
			  // e o de transmissão vaziu
			
				while (!(rx_buffer_full && tx_buffer_empty)){};

//				  // para contar o tempo total disponível para processameento de 128 amostras
//				Dt_DisableCount(DtChannel0);
//				
//				sprintf(buffer, "\n%i", tempo_inicial - Dt_ReadCurCntVal(DtChannel0));
//				
//				uart_printf(buffer);
//							
//				Dt_WriteLoadVal(tempo_inicial, DtChannel0);
//			
//				Dt_EnableCount(DtChannel0);
					
				// processa o buffer	
				proces_buffer();

		}

}
