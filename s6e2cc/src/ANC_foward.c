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
#define tempo_de_medicao 0x32

//parametros  do filtro vss-nlms
float32_t w[N] = { 0.0f };
float32_t x[N] = { 0.0f };
float32_t p[N] = { 0.0f };

float32_t alfa = 0.9961;
float32_t C = 0.001; // verificar artigo (k/SNR)
float32_t mu_max = 1.5;

float32_t ep = 1.0/0x7FFF;

float32_t fact[N] = { 0.0f };
float32_t pA[N] = { 0.0f };
float32_t pB[N] = { 0.0f };

float32_t p_energy;
float32_t mu;

float32_t d, refnoise, out, energy, d_hat, e;

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

				
				uart_command = '0';
		}
}

void proces_buffer(void) 
{
    uint32_t *txbuf, *rxbuf;
		int i = 0;
    // endereços de para o buffer de transporte e de recepção
    if(tx_proc_buffer == PING) txbuf = dma_tx_buffer_ping; 
    else txbuf = dma_tx_buffer_pong; 
    if(rx_proc_buffer == PING) rxbuf = dma_rx_buffer_ping; 
    else rxbuf = dma_rx_buffer_pong; 

		// Filtro projetado
		while(i<N){
				txbuf[i] = rxbuf[i];
				i++;
		}
		
    tx_buffer_empty = 0;
    rx_buffer_full = 0;
}

//Main function
int main (void) { 

		// inicializa o CODEC
    audio_init (hz8000, line_in, dma, DMA_HANDLER);
	
		while(1){

				// enquanto o buffer de recepção não estiver cheio 
			  // e o de transmissão vaziu
			
				while (!(rx_buffer_full && tx_buffer_empty)){};

				proces_buffer();

		}

}
