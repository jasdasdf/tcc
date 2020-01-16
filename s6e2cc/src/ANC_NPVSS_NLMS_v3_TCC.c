// ANC_NPVSS_NLMS_v1.c 

#include "audio.h"
#include "my_functions.h"
#include "my_npvss_nlms.h"
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
            
            audio_out =  audio_chR;
                       
            break;
        
        case 1:
 
            // referência de ruído
            refnoise = (float32_t)audio_chL;
            
            //desejado
            d = (float32_t)audio_chR;
                      
            //shift do vetor "x"		
            memmove(&x[1], &x[0], (L-1)*sizeof(float32_t));
      
            //shift do vetor "error"		
            memmove(&error[1], &error[0], (L-1)*sizeof(float32_t));
            
            //novo valor de referência armazenado
            x[0]=refnoise;
                
            //estimativa gerada pelo filtro
            arm_dot_prod_f32(x, w, L, &d_hat);
      
            //erro.
            e = d-d_hat;
            
            //armazena o sinal de erro
            error[0]= e;	
            
            delta = 20*var_x;
            
            //////// ESTIMATIVA DA VARIÂNCIA DO SINAL DE FALA ////////
            
            
            //variância do sinal "x".
            arm_power_f32(x,L, &power_x);
            var_x = (1.0/L)*power_x;
            
            //estimativa da correlação entre "x" e "e"
            arm_scale_f32(cor_error_x, lambda, fact_01, L);
            arm_scale_f32(x, (1-lambda)*e, fact_02, L);
            arm_add_f32(fact_01, fact_02, cor_error_x, L);
            arm_power_f32(cor_error_x, L, &power_cor_error_x);

            //estimativa da variancia do sinal de erro
            //arm_var_f32(error, L, &var_e);
            arm_power_f32(error,L, &power_error);
            var_e = (1.0/L)*power_error;  
							
            //variância do sinal de fala (ruído de medição)
            var_v = var_e - (power_cor_error_x/var_x);
            
            ///////////////////////////////////////////////////////
            
            //desvio padrão 
            arm_sqrt_f32(var_e, &desv_e);
            arm_sqrt_f32(var_v, &desv_v);
      
            //fator beta
            beta = (1/(delta + power_x))*(1 - (desv_v/(epslon + desv_e)));
            
            if (desv_e >= desv_v){
                mu_npvss = beta;
            }
            else{
                mu_npvss = 0;
            }
 
            arm_scale_f32(x, mu_npvss*e, v_aux, L);
            arm_add_f32(w, v_aux, w,  L);
            
            //saída
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
    
    init_npvss_nlms();
    
		
    audio_init (hz32000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
        gpio_set(TEST_PIN, HIGH);
		proces_buffer();
        gpio_set(TEST_PIN, LOW);
	}
}
