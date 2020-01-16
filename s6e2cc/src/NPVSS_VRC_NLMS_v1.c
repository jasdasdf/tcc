// ANC_NPVSS_VRC_NLMS.c

#include "audio.h"
#include "my_functions.h"
#include "arm_math.h"
#include "math.h"
#include "my_npvss_vrc_nlms.h"

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
            memmove(&x[1], &x[0], (N-1)*sizeof(float32_t));
				
						//shift do vetor "error"		
            memmove(&error[1], &error[0], (N-1)*sizeof(float32_t));
		  
						//shift do vetor w_stories
						memmove(&w_stories[N], &w_stories[0], (L_max-1)*(N)*sizeof(float32_t));
				
						//add os valores de w no vetor w_stories
						memmove(&w_stories[0], &w[0], N*sizeof(float32_t));
				
            //novo valor de refer�ncia armazenado
            x[0]=refnoise;
		  
						//energia do sinal "x".
            arm_power_f32(x,N, &energy);	
						
						// ||w(k)||^2
						arm_power_f32(w, N, &w_energy);
						
						d_hat = 0;	
						L= L_max;
						
						for (l=0; l< L; l++){
							
								// salva em w_aux o vetor w(k-l)
								memmove(&w_aux[0], &w_stories[(l*N)], N*sizeof(float32_t));
															
								// 
								arm_dot_prod_f32(w_aux, x, N, &aux);
							
								// 
								d_hat += pow(rho, l)*aux;				
						}
                   
//            //estima a sa�da "d_hat"
//            arm_dot_prod_f32(w,x,N,&d_hat); 	

            //erro.
            e_bar = d-d_hat;
						
						arm_dot_prod_f32(w,x,N,&d_hat);
						
						e = d - d_hat;
						
						//armazena o sinal de erro
            error[0]= e;	
				            
						//////// ESTIMATIVA DA VARI�NCIA DO SINAL DE FALA ////////
            
            //vari�ncia do sinal "x".
            arm_power_f32(x,N, &power_x);
            var_x = (1.0/N)*power_x;
            
            //estimativa da correla��o entre "x" e "e"
            arm_scale_f32(cor_error_x, lambda, fact_01, N);
            arm_scale_f32(x, (1-lambda)*e, fact_02, N);
            arm_add_f32(fact_01, fact_02, cor_error_x, N);
            arm_power_f32(cor_error_x, N, &power_cor_error_x);
            
            //estimativa da variancia do sinal de erro
            arm_var_f32(error, N, &var_e);
                    
            //vari�ncia do sinal de fala (ru�do de medi��o)
            var_v = var_e - (power_cor_error_x/var_x);
            
            ///////////////////////////////////////////////////////				
						
//						//calculo do valor coeficiente de re�so L
//						if(e_bar*e_bar < var_v){
//								L += 1;
//								if (L >= L_max){ 
//										L = L_max;}
//						}else{
//								L -= 1;
//								if (L < 1){
//										L = 1;
//								}
//						}					
	
						
						var_e_posteriory = lambda*var_e_posteriory + (1-lambda)*e_bar	;
						
						if(ep + pow(var_e_posteriory, 1/2) < pow(var_v, 1/2)){
						
								beta = 0;
						
						}else{
						
								beta = 1 - pow(var_v, 1/2)/(ep+pow(var_e_posteriory, 1/2));
							
						}
						
						// zerar vetores auxiliares
						arm_fill_f32(0.0, fact_01, N);
						arm_fill_f32(0.0, fact_02, N);
						arm_fill_f32(0.0, fact_03, N);
						arm_fill_f32(0.0, w_soma, N);
						
						// calculo do valor de teta
						teta = (rho-1)/(pow(rho, L)-1);
					
						for (l=0; l< L; l++){
							
								// salva em w_aux o vetor w(k-l)
								memmove(&w_aux[0], &w_stories[(l*N)], N*sizeof(float32_t));
															
								// multiplica w_aux por rho elevado a l e salva novamente em w_aux
								arm_scale_f32(w_aux, pow(rho, l), w_aux, N);
							
								// soma w_aux a W_soma a w_soma
								arm_add_f32(w_soma, w_aux, w_soma, N);					
						}
										
						// fator de atualiza��o 01
						arm_scale_f32(w_soma, teta, fact_01, N);
						
						//calcula o fator de adapta��o 02
            arm_scale_f32(x, e_bar,fact_02, N); 
						
						beta = 0.6;
						
						// multiplica fact02 por beta(energy+ep)
						arm_scale_f32(fact_02, beta/(energy+1), fact_02, N);
						         
            //atualiza os coeficientes do filtro.
            arm_add_f32(fact_01, fact_02, w, N);
                            
            //out
            int_out = (int16_t)e;
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
		init_npvss_vrc_nlms();
    audio_init (hz8000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
        gpio_set(TEST_PIN, HIGH);
				proces_buffer();
        gpio_set(TEST_PIN, LOW);
	}
}
