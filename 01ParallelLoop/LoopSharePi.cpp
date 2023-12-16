#include <iostream>
#include <omp.h>

#define NTHREADS 4

static long num_steps = 1000000000;

int main(){
  double  sum=0.0;
  double  step = 1.0/(double)num_steps;
  // omp_set_num_threads(NTHREADS);
  double start_time = omp_get_wtime();
  #pragma omp parallel for reduction(+:sum)
  for(int i=0 ; i!=num_steps ; ++i){
    double x = (i+0.5)*step;
    sum +=4.0/(1.0+x*x);
  }
  double pi = sum*step;
  double runtime = omp_get_wtime() - start_time;
  std::cout<<"pi area : "<<pi <<" run in "<<runtime<<std::endl;
  
}
