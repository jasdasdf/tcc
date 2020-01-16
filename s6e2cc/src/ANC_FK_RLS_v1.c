// ANC_FK_RLS_v1.c

#include "audio.h"
#include "my_functions.h"
#include "my_fk_rls.h"
#include "arm_math.h"

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
        
            audio_out =  (audio_chR<<16 & 0xFFFF0000) + (audio_chR & 0x0000FFFF);  
    
        break;
        
        case 1:
            // referência de ruído
            refnoise = (float32_t)audio_chL;
            
            //desejado
            d = (float32_t)audio_chR;
            				
						arm_dot_prod_f32(a,x, N, &aux);
            ef = refnoise + aux;
				
						arm_scale_f32( g, ef, aux_vector00, N);
						arm_sub_f32( a, aux_vector00, a, N);

						arm_dot_prod_f32(a, x, N, &aux);
						ep_f = refnoise + aux;
						
						Ef = lambda*Ef + ep_f*ef;
				
						arm_fill_f32( 0.0, g_post, N+1);
						arm_fill_f32( 1.0, aux_vector02, N+1);
						memcpy( &g_post[1], &g[0], N*sizeof(float32_t));
						memcpy( &aux_vector02[1], &a[0], N*sizeof(float32_t));
						arm_scale_f32(aux_vector02, ep_f/Ef, aux_vector02, N+1);						
						arm_add_f32(g_post, aux_vector02, g_post, N+1);
						
						memmove(&x[1], &x[0], (N-1)*sizeof(float32_t));
						x[0] = refnoise;
						
						arm_dot_prod_f32(b, x, N, &aux);
						eb = x[N-1] + aux;
						
						arm_fill_f32(0.0, aux_vector00, N);
						memcpy(&aux_vector00[0], &g_post[0], N*sizeof(float32_t));
						arm_scale_f32(b, g_post[N], aux_vector01, N);
						arm_sub_f32(aux_vector00, aux_vector01, aux_vector00, N);
						arm_scale_f32(aux_vector00, 1-(g_post[N]*eb), g, N);
						
						arm_scale_f32(g, eb, aux_vector00, N);
						arm_sub_f32(b, aux_vector00, b, N);
											
						arm_dot_prod_f32(x, w, N, &d_hat);
						e = d - d_hat;
						
						arm_scale_f32(g, e, aux_vector00, N);						
						arm_add_f32( w,  aux_vector00, w, N);

            //audio_out
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
    fk_rls_init();
    audio_init (hz8000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
        gpio_set(TEST_PIN, HIGH);
				proces_buffer();
        gpio_set(TEST_PIN, LOW);
	}
}
