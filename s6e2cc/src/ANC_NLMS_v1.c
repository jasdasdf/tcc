// ANC_NLMS.c

#include "audio.h"
#include "my_functions.h"
#include "arm_math.h"

#define N   128

//parametros  do filtro nlms
float32_t w[N] = { 0.0f };
float32_t x[N] = { 0.0f };
float32_t p[N] = { 0.0f };

float32_t fact[N] = { 0.0f };

float32_t ep = 1.0;

float32_t p_energy;

float32_t error, d, refnoise, error, out, energy, d_hat, e;

float32_t MU = 0.05;


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
            // referência de ruído
            refnoise = (float32_t)audio_chL;
				
            //desejado
            d = (float32_t)audio_chR;
				
            //shift do vetor "x"		
            memmove(&x[1], &x[0], (N-1)*sizeof(float32_t));
		  
            //novo valor de referência armazenado
            x[0]=refnoise;
		  
						//energia do sinal "x".
            arm_power_f32(x,N, &energy);	
                    
            //estima a saída "d_hat"
            arm_dot_prod_f32(w,x,N,&d_hat); 	

            //erro.
            e = d-d_hat;
				            
            //calcula o fator de adaptação.
            arm_scale_f32(x, MU*e/(energy+ep),fact, N); 
            
            //atualiza os coeficientes do filtro.
            arm_add_f32(w, fact, w, N);
                            
            //out
            int_out = (int16_t)e;
            audio_out = ((int_out<<16 & 0xFFFF0000)) + (int_out & 0x0000FFFF);
            
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
    audio_init (hz8000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
        gpio_set(TEST_PIN, HIGH);
				proces_buffer();
        gpio_set(TEST_PIN, LOW);
	}
}
