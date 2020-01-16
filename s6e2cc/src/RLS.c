// RLS.c

#include "audio.h"
#include "my_functions.h"
#include "my_rls.h"
#include "arm_math.h"

//variaveis
float32_t fact[N];
float32_t refnoise, d, d_hat, e;

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
            
            //shift do vetor "x"		
            memmove(&x[1], &x[0], (N-1)*sizeof(float32_t));
		  
            //novo valor de referência armazenado
            x[0]=refnoise;
      
            //estimativa da saída "d_hat"
            arm_dot_prod_f32(w,x,N,&d_hat);
		  
            //auxiliar PA
            arm_mat_scale_f32( &P, lambda_inv, &PA);
      
            //auxiliar PC
            arm_mat_mult_f32( &XT, &P, &PC);
      
            //atualizar os valores de K
            
            //auxiliar KA
            arm_mat_mult_f32( &PA, &X, &KA);
            
            //auxiliar KB
            arm_mat_mult_f32( &PC, &X, &KB);
            arm_mat_scale_f32( &KB, lambda_inv, &KB);
            
            //atualiza o valor de K
            arm_mat_scale_f32( &KA, 1.0f/(1.0f + kb[0]), &K);
      
            //erro
            e = d - d_hat;
                
            //atualiza a matriz P
                            
            //auxiliar PB
            arm_mat_scale_f32( &K, lambda_inv, &PB);
            
            //auxiliar PD
            arm_mat_mult_f32( &PB, &PC, &PD);

            //atualiza P = PA - PD
            arm_mat_sub_f32( &PA, &PD, &P);
            
            //fator de adaptacao dos coeficientes
            arm_scale_f32( K.pData, e,fact, N );
            
            //atualizacao dos coeficientes
            arm_add_f32(w, fact, w, N);

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
    rls_init();
    audio_init (hz8000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
        gpio_set(TEST_PIN, HIGH);
		proces_buffer();
        gpio_set(TEST_PIN, LOW);
	}
}
