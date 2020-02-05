// My functions 

#include "platform.h"
#include "s6e2cc.h"
#include "audio.h"

char debouncing[3]={0};
char status;
int  press=0;
int16_t test;

volatile int16_t audio_chR=0;    
volatile int16_t audio_chL=0; 
volatile uint32_t int_out;

void DMA_HANDLER (void)
{
    if (dstc_state(0)){ //check interrupt status on channel 0

        if(tx_proc_buffer == (PONG))
            {
                dstc_src_memory (0,(uint32_t)&(dma_tx_buffer_pong));    //Soucrce address
                tx_proc_buffer = PING; 
            }
            else
            {
                dstc_src_memory (0,(uint32_t)&(dma_tx_buffer_ping));    //Soucrce address
                tx_proc_buffer = PONG; 
            }
        tx_buffer_empty = 1;                                        //Signal to main() that tx buffer empty					
       
        dstc_reset(0);			                                        //Clean the interrup flag
    }
    if (dstc_state(1)){ //check interrupt status on channel 1

        if(rx_proc_buffer == PONG)
            {
                dstc_dest_memory (1,(uint32_t)&(dma_rx_buffer_pong));   //Destination address
                rx_proc_buffer = PING;
            }
        else
            {
                dstc_dest_memory (1,(uint32_t)&(dma_rx_buffer_ping));   //Destination address
                rx_proc_buffer = PONG;
            }
        rx_buffer_full = 1;   
						
        dstc_reset(1);		
    }
}

