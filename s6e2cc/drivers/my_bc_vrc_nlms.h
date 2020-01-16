//my_bc_vrc_nlms.h

// ordem do filtro
#define N	  			12
#define lambda 		0.999
#define L_max			4
#define var_n			0.01

// parametros para o algoritmo bc_vrc_nlms
float32_t beta 	  = 0.6;
float32_t rho 	  = 0.6;
float32_t ep 		  = 1.0;
float32_t	eta_hat = 0; 
float32_t teta		= 0;
float32_t aux			= 0;
float32_t power_x	= 0;
int 	  	L			  = 0;
int 		 	l 		  = 0;
int 			L_old		= 0;


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

float32_t var_w, var_x, var_e, var_v;
float32_t power_cor_error_x;
float32_t cor_error_x[N];
float32_t error[N];

void init_bc_vrc_nlms(){

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
	
	return;
}
