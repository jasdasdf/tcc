// ANC_VSS_NLMS_SHIN_v1.c

#include "audio.h"
#include "my_functions.h"
#include "arm_math.h"
//#include "gpio.h"
#include "uart_io.h"
#include "dt.h"

#include "stdio.h"

#define N   128

#define tempo_inicial 0x1FFFFFF


//parametros  do filtro vss-nlms
float32_t w[N] = { 0.0f };
float32_t x[N] = { 0.0f };
float32_t p[N] = { 0.0f };

float32_t alfa = 0.9961;
float32_t C = 0.001; // verificar artigo (k/SNR)
float32_t mu_max = 1.5;

float32_t ep = 1.0;

float32_t fact[N] = { 0.0f };
float32_t pA[N] = { 0.0f };
float32_t pB[N] = { 0.0f };

float32_t p_energy;
float32_t mu;

float32_t d, refnoise, out, energy, d_hat, e;

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
				memmove(&x[1], &x[0], (N-1)*sizeof(float32_t));
	
				//shift do vetor "p"
				//memmove(&p[1], &p[0], (N-1)*sizeof(float32_t));		  
		
				//novo valor de referência armazenado
				x[0]=refnoise;
	
				//energia do sinal "x".
				arm_power_f32(x,N, &energy);	
								
				//estima a saída "d_hat"
				arm_dot_prod_f32(w,x,N,&d_hat); 	

				//erro.
				e = d-d_hat;
		
				//considerando p = pA + pB
				//logo, pB = (e*(1-alfa)/energy)*vetor_x
				
			
				// adicionado porque quando a energia fosse 0 tornava as variáveis seguintes nan
				if (energy ==0){
						arm_scale_f32( x, (1-alfa)*e/(energy+ep), pB, N);
				}else{
						arm_scale_f32( x, (1-alfa)*e/(energy), pB, N);
				}

				//pA = alfa*p
				arm_scale_f32( p, alfa, pA, N);
				
				//novo valor de "p", p = pA + pB
				arm_add_f32(pA, pB, p, N);
				
				//energia de "p"  --> || p ||^2
				arm_power_f32(p,N, &p_energy);	
				
				//calculo do passo variável
				mu = mu_max*p_energy/(p_energy + C);
				
				//calcula o fator de adaptação.
				arm_scale_f32(x, (mu*e)/(energy+ep),fact, N); 
				
				//atualiza os coeficientes do filtro.
				arm_add_f32(w, fact, w, N);
												
				//out
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

//					// para contar o tempo total disponível para processameento de 128 amostras
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
