#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <math.h>
#include <functional>
#define NTHREADS 12
double func(const double &x){
  return 4.0/(1.0+x*x);
}
template<typename FunctionType> double Area(FunctionType func,const double &start, 
                                            const double &end, const long &num_steps){
  double width = (end-start)/num_steps;
  double hight[NTHREADS]={0.0};
  int actual_RunThreads;
  // omp_set_num_threads(NTHREADS);
  double start_time = omp_get_wtime();
  #pragma omp parallel
  {
    //获取线程id
    long thread_ID = omp_get_thread_num();
    //获取实际运行的线程数
    if(!thread_ID) actual_RunThreads = omp_get_num_threads();
    for(long i = thread_ID; i<num_steps ; i+=actual_RunThreads){
      double x = start +(i+0.5)*width;
      hight[thread_ID]+=func(x);
    }
  }
  double hightSum=0.0;
  for (int i =0;i!=actual_RunThreads ; ++i) {
    hightSum+=hight[i]; 
  }
  double runtime = omp_get_wtime()-start_time;
  std::cout<<"runtime: "<<runtime<<" actual_RunThreads: "<<actual_RunThreads<<std::endl;
  return hightSum*width;
}

template<typename FunctionType, typename... Args> double AreaAgent(FunctionType func ,Args... args){
  return Area( func, args...);
}

int main(){
  std::function<double(const double&)> f = func;
  double area =  AreaAgent(f , 0.0 , 1.0 , 100000000);
  std::cout<<"area: "<<area<<std::endl;
  return 0;
}
