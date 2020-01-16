// my_rls.h
// contem as definições principais e função de inicialização.

#define N   18

//constantes definidas para o filtro 
#define sigma				900
#define sigma_inv			(1.0/sigma)
#define lambda				0.999
#define lambda_inv		    (1/lambda)

//parametros  do filtro RLS

//vetores 
float32_t w[N];
float32_t x[N];
float32_t k[N];

float32_t p[N*N];
float32_t xt[N];

float32_t pa[N*N];
float32_t pb[N];
float32_t pc[N];
float32_t pd[N*N];

float32_t ka[N];
float32_t kb[1];

//instância das matrizes
arm_matrix_instance_f32 P;
arm_matrix_instance_f32 X;
arm_matrix_instance_f32 XT;
arm_matrix_instance_f32 W;
arm_matrix_instance_f32 K;

//matrizes para calcular P
// P = PA + PD  ;  PD = PB*PC 
arm_matrix_instance_f32 PA;
arm_matrix_instance_f32 PB;
arm_matrix_instance_f32 PC;
arm_matrix_instance_f32 PD;

//matrizes para calcular K
// K = KA/(1+ KB)
arm_matrix_instance_f32 KA;
arm_matrix_instance_f32 KB;

void rls_init(){
    
    int i=0;
    
    //completa ones e vector_init com 1
	arm_fill_f32(0.0, p, N);
	arm_fill_f32(0.0, w, N);
	arm_fill_f32(0.0, x, N);
	arm_fill_f32(0.0, k, N);
	arm_fill_f32(0.0, pa, N*N);
	arm_fill_f32(0.0, pb, N);
	arm_fill_f32(0.0, pc, N);
	arm_fill_f32(0.0, pd, N*N);
	arm_fill_f32(0.0, ka, N);
	arm_fill_f32(0.0, kb, 1);
	
	
	//inicializa uma matriz NxN completa com 1;
	arm_mat_init_f32( &P, N, N, (float32_t *) p);
	
	for (i=0; i<N*N; i++){
		p[i*(N + 1)] = 1.0;
	}
	
	//inicializa o vetor xt
	arm_mat_init_f32( &XT, 1, N, (float32_t *) x);
	
	//inicializa a matriz X transposta com os valores do vetor x.
	arm_mat_init_f32( &X, N, 1, (float32_t *)x);
	
	//inicializa a matriz P = P(0) = (1/sigma)I 
	arm_mat_scale_f32( &P, sigma_inv, &P);
	
	//inicializa as matrizes auxiliares para o calculo de P
	arm_mat_init_f32( &PA, N, N,(float32_t *) pa);
	arm_mat_init_f32( &PB, N, 1, (float32_t *)pb);
	arm_mat_init_f32( &PC, 1, N,(float32_t *) pc);
	arm_mat_init_f32( &PD, N, N,(float32_t *) pd);
		
	//inicializa a matriz K
	arm_mat_init_f32( &K, N, 1, (float32_t *)k);
	
	//inicializa as matrizes auxiliares de K
	arm_mat_init_f32( &KA, N, 1, (float32_t *)ka);
	arm_mat_init_f32( &KB, 1, 1, (float32_t *)kb);

    return ;

}
