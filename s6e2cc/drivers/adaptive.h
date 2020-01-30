// header com os algoritmos adaptativos e suas definições

#include "arm_math.h"

#define 	N 	128

//parametros  do filtro nlms
float32_t w[N] = { 0.0f };
float32_t x[N] = { 0.0f };
float32_t p[N] = { 0.0f };

float32_t fact[N] = { 0.0f };

float32_t ep = 1.0;

float32_t p_energy;

float32_t refnoise, energy, d_hat, e, d;


float32_t nlms(float32_t refnoise, float32_t desired, float32_t MU, float32_t ep){
	
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
			
			//retorna o valor do erro
			return e;
		
}