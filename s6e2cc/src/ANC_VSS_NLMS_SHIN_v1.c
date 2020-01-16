// ANC_VSS_NLMS_SHIN_v1.c

#include "audio.h"
#include "my_functions.h"
#include "arm_math.h"

#define N   128

//parametros  do filtro vss-nlms
arm_lms_norm_instance_f32 s;
float32_t w[N] = { 0.0f };
float32_t x[N] = { 0.0f };
float32_t p[N] = { 0.0f };

float32_t alfa = 0.9961;
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
		  
            //shift do vetor "p"
            //memmove(&p[1], &p[0], (N-1)*sizeof(float32_t));		  
				
            //novo valor de refer�ncia armazenado
            x[0]=refnoise;
		  
				    //energia do sinal "x".
            arm_power_f32(x,N, &energy);	
                    
            //estima a sa�da "d_hat"
            arm_dot_prod_f32(w,x,N,&d_hat); 	

            //erro.
            e = d-d_hat;
				
            //considerando p = pA + pB
            //logo, pB = (e*(1-alfa)/energy)*vetor_x
            arm_scale_f32( x, (1-alfa)*e/energy, pB, N);
            
            //pA = alfa*p
            arm_scale_f32( p, alfa, pA, N);
            
            //novo valor de "p", p = pA + pB
            arm_add_f32(pA, pB, p, N);
            
            //energia de "p"  --> || p ||^2
            arm_power_f32(p,N, &p_energy);	
            
            //calculo do passo vari�vel
            mu = mu_max*p_energy/(p_energy + C);
            
            //calcula o fator de adapta��o.
            arm_scale_f32(x, (mu*e)/(energy+ep),fact, N); 
            
            //atualiza os coeficientes do filtro.
            arm_add_f32(w, fact, w, N);
                            
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
    audio_init (hz8000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
        gpio_set(TEST_PIN, HIGH);
		proces_buffer();
        gpio_set(TEST_PIN, LOW);
	}
}
