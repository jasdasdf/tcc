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

void Led_On(char led)
{
    if(led == 'R') // se led vermelhor 'R' -> red
    {
            gpio_set(LED_R,0);// lig. red
            gpio_set(LED_G,1);// des. green
            gpio_set(LED_B,1);// des. azul
            return;
    }
    if(led == 'G') // se led verde 'G' -> green
    {
            gpio_set(LED_R,1);// des. red
            gpio_set(LED_G,0);// lig. green
            gpio_set(LED_B,1);// des. azul
            return;
    }
    if(led == 'B') // se led azul 'B' -> blue
    {
            gpio_set(LED_R,1);// des. red
            gpio_set(LED_G,1);// des. green
            gpio_set(LED_B,0);// des. azul
            return;
    }
        
    return;
}

void Led_off(){
    gpio_set(LED_R,1);// des. red
    gpio_set(LED_G,1);// des. green
    gpio_set(LED_B,1);// des. azul
    return;
}

void init_leds(){
    
    // Init GPIO pin function (LED's)
	gpio_set_mode(LED_R, Output); // azul
    gpio_set_mode(LED_G, Output); // vermelho
	gpio_set_mode(LED_B, Output); // verde
    Led_off();
    return;

}

void botao(){
    
    debouncing[2]=debouncing[1];   
    debouncing[1]=debouncing[0];			// shift das entradas
    debouncing[0]=gpio_get(BUTTON);
      
    status = ((((debouncing[2])^255) | debouncing[1])|debouncing[0]);
			// debouncing
		
	if(status!=(char)255) // se estatus é diferente de 255 o botão 
	  					// foi pressionado
    {
		press+=1; // incrementa a variável press
		if(press==1){									// quando press igual a 1
			Led_On('B');	// liga o led azul.	
		}
		if(press==2){			// se igual a 2
			Led_off();  	// desliga todos os leds.
			press=0;
		}	
    }			
	return; 
}


void SysTick_Handler(void)  {                               
//    test++;
    botao();
}

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

void system_init(){
    
//    // inicia os leds
//    init_leds();
//    
//    // configura systick
//    SysTick_Config(SystemCoreClock / 1000);
    
    return;
}
