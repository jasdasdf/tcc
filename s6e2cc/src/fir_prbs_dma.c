// fir_prbs_dma.c

#include "audio.h"
#include "my_functions.h"
#include "arm_math.h"

#define N   256

//parametros  do filtro vss-nlms
arm_lms_norm_instance_f32 s;
float32_t w[N] = { 0.0f };
float32_t x[N] = { 0.0f };
float32_t p[N] = { 0.0f };

float32_t alfa = 0.99;
float32_t C = 0.001; // verificar artigo (k/SNR)
float32_t mu_max = 1.5;

float32_t ep = 1.0;

float32_t fact[N] = { 0.0f };
float32_t pA[N] = { 0.0f };
float32_t pB[N] = { 0.0f };

float32_t p_energy;
float32_t mu;

float32_t error, d, refnoise, error, out, energy, d_hat, e;


void proces_buffer(void) 
{
    uint32_t *txbuf, *rxbuf;
    uint32_t audio_in, audio_out;
     
    // endereços de para o buffer de transporte e de recepção
    if(tx_proc_buffer == PING) txbuf = dma_tx_buffer_ping; 
    else txbuf = dma_tx_buffer_pong; 
    if(rx_proc_buffer == PING) rxbuf = dma_rx_buffer_ping; 
    else rxbuf = dma_rx_buffer_pong; 
    
    // audio de entrada
    audio_in = *rxbuf;
       
    audio_chR = (audio_in & 0x0000FFFF);

    audio_chL = (audio_in>>16 & 0x0000FFFF);
    
    switch(press){
        case 0:
            
            audio_out =  audio_in;
                       
            break;
        
        case 1:
           
        break;
    
    }
         
     // audio de saída
    *txbuf = audio_out;
    
    tx_buffer_empty = 0;
    rx_buffer_full = 0;
}

//Main function
int main (void) { 
    
    system_init();    
    audio_init (hz48000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
        gpio_set(TEST_PIN, HIGH);
		proces_buffer();
        gpio_set(TEST_PIN, LOW);
	}
}
