//
// Header: Funções 
// File Name: my_npvss_nlms.h
// Author: Rafael Rodrigo Pertum
// Date: 07/01/2019

// Declaração de variáveis
//*******************************************************************

//constantes definidas par ao filtro npvss-nlms
#define L 			128		
#define K				6 // 2<=K<=6 ? (artigo Benesty)
#define epslon		1.0

//parametros  do filtro npvss-nlms
float32_t a, e, d, d_hat;
float32_t refnoise;
float32_t power_x, var_e, var_x, var_v, desv_v, desv_e;
float32_t power_cor_error_x, delta, beta, mu_npvss;
float32_t x[L], w[L], error[L], v_aux[L], cor_error_x[L];
float32_t fact_01[L], fact_02[L];
float32_t lambda, lambda1, lambda2;
float32_t power_error;

void init_npvss_nlms(){
	
//	lambda  = 1.0 - 1.0/(K*L);
	lambda1  = 1.0 - 1.0/(K*L);
	lambda2  = 0.9999;
	arm_fill_f32( 0.0, x, L);
	arm_fill_f32( 0.0, w, L);
	arm_fill_f32( 0.0, error, L);
	arm_fill_f32( 0.0, v_aux, L);
	arm_fill_f32( 0.0, cor_error_x, L);
	arm_fill_f32( 0.0, fact_01, L);
	arm_fill_f32( 0.0, fact_02, L);
	power_error = 0.0;
	var_e=0;
	var_x=0;

	return;
}
