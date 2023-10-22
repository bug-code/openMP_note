#include <stdio.h>
#include <omp.h>
#include <iostream>
static long num_steps = 100000000;
double step;
int main(){
  int i;
  double x , pi , sum=0.0;
  double start_time , run_time;
  step = 1.0 / (double)num_steps;
  start_time = omp_get_wtime();
  for (int i=0; i<num_steps; ++i) {
    x = (i+0.5)*step;
    sum += 4.0 / (1.0 +x*x);
  }
  pi = step*sum;
  run_time = omp_get_wtime() - start_time;
  std::cout<<"area result: "<<pi<<" steps: "<<num_steps<<" runtime: "<<run_time<<std::endl;
  return 0;
}
