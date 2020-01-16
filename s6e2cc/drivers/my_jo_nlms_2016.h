//my_jo_nlms_2016.h

//constantes definidas par ao filtro jo-nlms
#define L 			128		
//#define lambda 	0.9
#define K 			2

//parametros  do filtro jo-nlms
float32_t cor_error_x[L];
float32_t power_u, power_x;
float32_t error[L], u[L], x[L], w[L];
float32_t m = 1, p, q;
float32_t var_w, var_x, var_e, var_v;
float32_t power_cor_error_x;
float32_t  d, refnoise, out, d_hat, e;
float32_t fact_01[L], fact_02[L];
float32_t lambda ;

void init_jo_nlms(){

	//inicializa variaveis
	lambda = 1.0 - 1.0/(K*L);
	arm_fill_f32( 0.0, w, L);
	arm_fill_f32( 0.0, u, L);	
	arm_fill_f32( 0.0, cor_error_x, L);
	arm_fill_f32( 0.0, error, L);
	arm_fill_f32( 0.0, x, L);
	arm_fill_f32( 0.0, fact_01, L);
	arm_fill_f32( 0.0, fact_02, L); 
	
	return;
}
