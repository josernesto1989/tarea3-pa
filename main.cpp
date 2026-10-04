#include <time.h>
#include <memory>
#include "iostream"
extern "C"
{
#include <immintrin.h>
}
#include <chrono>

using namespace std;

// COUNTER TYPE //
typedef unsigned long long bench_t;


static bench_t before;
static bench_t after;

// ASKS FOR THE TIME STAMP COUNTER (= WHAT TIME IS IT PLEASE ?)//
static inline bench_t cycles(void) {
	unsigned int hi, lo;
	__asm__ __volatile__ ("rdtsc\n\t":"=a" (lo), "=d"(hi));
	return ((bench_t) lo) | (((bench_t) hi) << 32);
}


float horner(float X,float *coef,long size){
	float ACC=0.0;
	int i;
	for(i=0;i<size;i++){
		ACC=(ACC+coef[i])*X;
	}
	return ACC;	
}

float horner_intrinsic(float X,float *coef,long size){
	float *R,P;
	int i;
	__m256 *ymm0,X256,Y;	
	
	ymm0 = (__m256*)coef; 
	
	
	X256 = _mm256_set1_ps(X*X*X*X*X*X*X*X);
	Y = _mm256_set1_ps(0.0);
    
	for(i=0;i<size/8-1;i++){;
		Y = _mm256_add_ps(Y,ymm0[i]);
		Y = _mm256_mul_ps(Y,X256);
	}
	
	Y = _mm256_add_ps(Y,ymm0[i]);
	
	R = (float *)(&Y);
	
	P  = R[7]*X;
	P += R[6]*X*X;
	P += R[5]*X*X*X;
	P += R[4]*X*X*X*X;
	P += R[3]*X*X*X*X*X;
	P += R[2]*X*X*X*X*X*X;
	P += R[1]*X*X*X*X*X*X*X;
	P += R[0]*X*X*X*X*X*X*X*X;

	return P;
}

int main(){
	alignas(32) float coef[4000];
	float X=1.1;
	float R;
	int i,num_trails = 100000;
	
	
	
	srand(time(NULL));	

	float *coeficientes;
	int j;
	coeficientes = (float *)_mm_malloc(96*sizeof(float), 32);

	for(j=0;j<1;j++){
		
		for(i=0;i<96;i++){
			coeficientes[i]=(float)(rand()%96)/1000;
			// if(i<10)
			// 	cout<< coeficientes[i] <<endl;
		}
		R=horner(X,coeficientes,96);
		cout<< R <<endl;
		R=horner_intrinsic(X,coeficientes,96);
		cout<< R <<endl;		
	}
	
	auto start = std::chrono::high_resolution_clock::now();
	for(j=0;j<num_trails;j++)	
		R=horner(X,coeficientes,96);
	auto end = std::chrono::high_resolution_clock::now();
  	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  	cout<<"Tiempo horner en nanosegundos: "<<duration.count()<<endl;
	

	auto start2 = std::chrono::high_resolution_clock::now();
	for(j=0;j<num_trails;j++)	
		R=horner_intrinsic(X,coeficientes,96);
	auto end2 = std::chrono::high_resolution_clock::now();
  	auto duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end2 - start2);
  	cout<<"Tiempo horner_intrinsic en nanosegundos: "<<duration2.count()<<endl;

		
	_mm_free(coeficientes);

}