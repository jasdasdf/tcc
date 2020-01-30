// ANC_NLMS.c

#include "audio.h"
#include "my_functions.h"
#include "arm_math.h"
#include "adaptive.h"

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
    
		e = nlms((float32_t)audio_chL,(float32_t)audio_chR, 0.05, 1.0)

    int_out = (int16_t)e;
		audio_out = ((int_out<<16 & 0xFFFF0000)) + (int_out & 0x0000FFFF);       
         
     // audio de saída
    *txbuf = audio_out;
    
    tx_buffer_empty = 0;
    rx_buffer_full = 0;
}

//Main function
int main (void) { 
    
    system_init();    
    audio_init (hz8000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
        gpio_set(TEST_PIN, HIGH);
				proces_buffer();
        gpio_set(TEST_PIN, LOW);
	}
}
