// my_vss_nlms_zipf.h
//inlcude "arm_math.h"

//parametros  do filtro vss_nlms_zipf
#define L 			128		
float32_t beta	= 0.99;
float32_t ep		= 1.0;
  
float32_t w[L], fact[L], x[L];
float32_t energy=0.0;  
float32_t d_hat=0.0, d=0.0, refnoise=0.0;
int16_t erro=0, i=0;
float32_t alfa=0.0, p_atual=0.0, q_atual=0.0;
float32_t p_anterior=0.0, q_anterior=0.0;
float32_t e_atual=0.0, e_anterior=0.0;
float32_t mu=0.0;
float32_t aux=0.0;

void init_vss_nlms_zipf(){

		//inicializa variaveis
		arm_fill_f32(0.0, w, L);
    arm_fill_f32(0.0, fact, L);
    arm_fill_f32(0.0, x, L);    
	
	return;
}
