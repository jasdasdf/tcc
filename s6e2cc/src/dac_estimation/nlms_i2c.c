/*******************************************************************
 DAC_ESTIMATOR_WITH_DAC_INVERSE
 RAFAEL R. PERTUM
 Data: 30/12/2017
 
 DESCRIÇÃO: 
						( led vermelho )
						Estima a resposta do DAC, vetor channel[]  .
						Então utilizar a resposta estimada e gerar um filtro no
						MatLab com a rotina " fir_dac_inverse_generator.m ".
						
						( led azul )
						Estima a resposta do DAC após o sinal ser filtrado 
						vetor w[].

********************************************************************/
// Bibliotecas adicionadas
#include "audio.h"
#include "platform.h"
#include "arm_math.h"
#include "dac_inv.h"

// definições
#define NUM_TAPS 256 // ordem
	
// Declaração de variáveis
//*******************************************************************
volatile int16_t audio_chR=0;  
volatile int16_t audio_chL=0;    
float32_t w[NUM_TAPS], fact[NUM_TAPS], x[NUM_TAPS];
float32_t chL[NUM_TAPS], chR[NUM_TAPS];
float32_t beta, energy;  
float32_t d_hat, d, fator_correcao, refnoise, e;

float32_t chL_in[NUM_TAPS], chR_in[NUM_TAPS];
float32_t chR_filtering, chL_filtered, PRBS;
float32_t channel[NUM_TAPS];

char debouncing[3]={0};
char status;
int16_t erro, i, press=0;

//****************************************************************
/*
******************************************************************
Função Led_on()

Descrição: 	Função que recebe um char correspondente a cor do led
						que se deseja ligar, se caracter inválido desliga
						os leds.
Retorno: nenhum
Entrada: char correspondete a cor do led (R, G ou B), se inválido 
				 (qualquer outro char) a função irá desligar os leds.
******************************************************************
*/
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

void Led_off()
{
			gpio_set(LED_R,1);// des. red
			gpio_set(LED_G,1);// des. green
			gpio_set(LED_B,1);// des. azul
			return;
}
/*
******************************************************************
Função botao()

Descrição:Função que executa o debouncing do botão, incrementa o 
					a variável press e liga algum led indicando 
					uma mudança (foi pressionado um botão!).
Retorno: nenhum
Entrada: nenhuma
******************************************************************
*/
void botao()
{
			debouncing[2]=debouncing[1];   
      debouncing[1]=debouncing[0];			// shift das entradas
      debouncing[0]=gpio_get(BUTTON);
      
      status = ((((debouncing[2])^255) | debouncing[1])|debouncing[0]);
			// debouncing
		
			if(status!=255) // se estatus é diferente de 255 o botão 
											// foi pressionado
      {
					press+=1; // incrementa a variável press
					if(press==1)
					{									// quando press igual a 1
							Led_On('B');	// liga o led azul.	
					}

					if(press==2){			// se igual a 2
							Led_off();  	// desliga todos os leds.
							Led_On('R');
							press=0;
					}	
      }			
			return; 
}

void I2S_HANDLER(void) {   /****** I2S Interruption Handler *****/

	
			int16_t audio_out_chL;
			audio_IN = i2s_rx();	//recebe os dados via i2S 
														// inteiro de 32-bits.

			audio_chL = (audio_IN>>16 & 0x0000FFFF); //Separate 16 bits channel right
							
      botao();   // verifica botão   
					
			if(press==0)
			{
					// *********  DAC ESTIMATION   ****************************
					PRBS=(float32_t)prbs();	
					audio_out_chL= (int16_t)(PRBS);
					audio_OUT= (audio_out_chL<<16 & 0xFFFF0000) + (audio_out_chL & 0x0000FFFF);
				
					beta = 0.1;	
					
					refnoise 	= PRBS;
						
					d =  (float32_t)audio_chL;
														 
					energy -= x[NUM_TAPS-1]*x[NUM_TAPS-1]; 
					for(i=NUM_TAPS-1; i>0; i-=4)
					{
							x[i]=x[i-1];
							x[i-1]=x[i-2];			// shift do vetor de entrada
							x[i-2]=x[i-3];
							x[i-3]=x[i-4];
					}
					x[0]=refnoise;
					energy += x[0]*x[0];
					arm_dot_prod_f32(channel,x,NUM_TAPS,&d_hat); 					
					e = d-d_hat;
					arm_scale_f32(x, (beta*e)/(energy+1),fact, NUM_TAPS);
					arm_add_f32(channel, fact, channel, NUM_TAPS);
					
			}
			
			if(press==1){ 
					
					// *******  DAC INVERSE FILTER	***************************
				
					for(i=NUM_TAPS-1; i>0; i-=4)
					{
							chL_in[i]=chL_in[i-1];
							chL_in[i-1]=chL_in[i-2];			// shift do vetor
							chL_in[i-2]=chL_in[i-3];
							chL_in[i-3]=chL_in[i-4];
					}
					
					chL_in[0]=(float32_t)audio_chL;
					// h = coeficientes do inv_dac.h (arquino gerado)
					arm_dot_prod_f32(h, chL_in, NUM_TAPS, &chL_filtered);

					//*************		DAC ESTIMATION    **************
				
					PRBS=(float32_t)prbs();	
					audio_out_chL= (int16_t)(PRBS);
					audio_OUT= (audio_out_chL<<16 & 0xFFFF0000) + (audio_out_chL & 0x0000FFFF);
			
					beta = 0.1;	
					
					refnoise 	= PRBS;
					d					= chL_filtered;
						
					energy -= x[NUM_TAPS-1]*x[NUM_TAPS-1]; // subtrai da energia o 
			
					for(i=NUM_TAPS-1; i>0; i-=4)
					{
							x[i]=x[i-1];
							x[i-1]=x[i-2];			// shift do vetor de entrada
							x[i-2]=x[i-3];
							x[i-3]=x[i-4];
					}
					
					// armazena o novo valor de entrada.
					x[0]=refnoise;
					// produto e soma ponto a ponto entre w[] e x[].		
					arm_dot_prod_f32(w,x,NUM_TAPS,&d_hat); 					
					// acrescenta a energia o peso do último dado armazenado.
					energy += x[0]*x[0]; 
					e = d-d_hat;	
					// fator de adaptação fact[].
					arm_scale_f32(x, (beta*e)/(energy+1),fact, NUM_TAPS);
					// atualização dos coeficientes do filtro.
					arm_add_f32(w, fact, w, NUM_TAPS);
					
			}
			
		i2s_tx(audio_OUT);
		return;	

}

int main(void)
{
		gpio_set_mode(TEST_PIN,Output);
		init_LED(); // inicializa os leds;
		Led_off();
		Led_On('R');
		audio_init ( hz8000, line_in, intr, I2S_HANDLER); 
 
  while(1){}
}
