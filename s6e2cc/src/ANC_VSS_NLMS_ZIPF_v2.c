// ANC_NLMS.c

#include "audio.h"
#include "my_functions.h"
#include "my_vss_nlms_zipf.h"
#include "arm_math.h"

void proces_buffer(void) 
{
    uint32_t *txbuf, *rxbuf;
    uint32_t audio_in, audio_out;
     
    // endere�os de para o buffer de transporte e de recep��o
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
            // refer�ncia de ru�do
            refnoise = (float32_t)audio_chL;
				
            //desejado
            d = (float32_t)audio_chR;
				
            //shift do vetor "x"		
            memmove(&x[1], &x[0], (L-1)*sizeof(float32_t));
		  
            //novo valor de refer�ncia armazenado
            x[0]=refnoise;
		  
						//energia do sinal "x".
            arm_power_f32(x,L, &energy);	
                    
            //estima a sa�da "d_hat"
            arm_dot_prod_f32(w,x,L,&d_hat); 	

            //erro.
            e_atual = d-d_hat;
        
            // CALCULO DE mu
					
            // calculo do valor de p
            p_atual = (beta*p_anterior) + ((1-beta)*e_atual*e_anterior);
            
            // calculo do valor de q
            q_atual = (beta*q_anterior) + ((1-beta)*e_atual*e_atual);
            
            // vari�vel auxiliar (reduzir as divis�es)
            aux = p_atual/q_atual;
            
            // calculo do valor de alfa
            alfa = aux*aux;
            
            // Calculo do valor de mu
            mu=alfa/(energy+ep);
            
            // fator de adapta��o fact[].
            arm_scale_f32(x,mu*e_atual,fact,(uint32_t)L);
				            
            //atualiza os coeficientes do filtro.
            arm_add_f32(w, fact, w, L);
                            
            //out
            int_out = (int16_t)e_atual;
            
            // atualiza��o das vari�veis
            p_anterior = p_atual;
            q_anterior = q_atual;
            e_anterior = e_atual;
                    
            audio_out = ((int_out<<16 & 0xFFFF0000)) + (int_out & 0x0000FFFF);
            
        break;
    
    }
         
     // audio de sa�da
    *txbuf = audio_out;
    
    tx_buffer_empty = 0;
    rx_buffer_full = 0;
}

//Main function
int main (void) { 
    
    system_init(); 
    init_vss_nlms_zipf();
    audio_init (hz8000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
        gpio_set(TEST_PIN, HIGH);
		proces_buffer();
        gpio_set(TEST_PIN, LOW);
	}
}
