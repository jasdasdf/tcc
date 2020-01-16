//my_npvss_vrc_nlms.h

// ordem do filtro
#define N	  			128
#define lambda 		0.99
#define L_max			10


// parametros para o algoritmo npvss_vrc_nlms
float32_t beta 	  = 0;
float32_t rho 	  = 0.6;
float32_t ep 		  = 1e-9; 
float32_t teta		= 0;
float32_t aux			= 0;
float32_t power_x	= 0;
int 	  	L			  = L_max;
int 		 	l 		  = 0;



//variaveis
float32_t w[N];
float32_t x[N];
float32_t fact_01[N];
float32_t fact_02[N];
float32_t fact_03[N];
float32_t w_stories[N*L_max];
float32_t w_soma[N];
float32_t w_aux[N];
float32_t w_energy;
float32_t d, refnoise, out, energy, d_hat, e;
float32_t e_bar;
float32_t var_e_posteriory;

float32_t var_w, var_x, var_e, var_v;
float32_t power_cor_error_x;
float32_t cor_error_x[N];
float32_t error[N];

void init_npvss_vrc_nlms(){

	//inicializa variaveis
	arm_fill_f32( 0.0, w, N);
	arm_fill_f32( 0.0, x, N);
	arm_fill_f32( 0.0, w_aux, N);
	arm_fill_f32( 0.0, w_soma, N);
	arm_fill_f32( 0.0, fact_01, N);
	arm_fill_f32( 0.0, fact_02, N); 
	arm_fill_f32( 0.0, fact_03, N);
	arm_fill_f32(0.0, w_stories, N*L_max);
	arm_fill_f32(0.0, error, N);
	arm_fill_f32(0.0, cor_error_x, N);
	e_bar = 0;
	var_e = 0;
	return;
}
