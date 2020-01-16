// my_fk_rls.h
// contem as definições principais e função de inicialização.

#define N   128

//constantes definidas para o filtro 
#define sigma					900
#define sigma_inv			(1.0/sigma)
#define lambda				0.99
#define lambda_inv		(1/lambda)
//variaveis do filtro FK RLS

//vetores 
float32_t w[N];
float32_t x[N];
float32_t g[N];
float32_t g_post[N+1];
float32_t a[N];
float32_t b[N];
float32_t aux_vector00[N];
float32_t aux_vector01[N];
float32_t aux_vector02[N+1];

//variaveis
float32_t refnoise, d, d_hat, e;
float32_t ef, ep_f, Ef, eb;
float32_t aux; 

void fk_rls_init(){
          
  //completa ones e vector_init com 0
	arm_fill_f32(0.0, w, N);
	arm_fill_f32(0.0, x, N);
	arm_fill_f32(0.0, g, N);
	arm_fill_f32(0.0, g_post, N+1);
	arm_fill_f32(0.0, a, N);
	arm_fill_f32(0.0, b, N);
	arm_fill_f32(0.0, aux_vector00, N);
	arm_fill_f32(0.0, aux_vector01, N);
	arm_fill_f32(0.0, aux_vector02, N);
			
	return ;

}
