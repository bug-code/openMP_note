#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <math.h>
#include <functional>
double func(const double &x){
  return 4.0/(1.0+x*x);
}
template<typename FunctionType> double Area(FunctionType func,const double &start, 
                                            const double &end, const long &num_steps){
  //获取每块的宽度
  //num_steps表示分为多少块
  double width = (end-start)/num_steps;
  double sum=0.0;
  int actual_RunThreads;
  double start_time = omp_get_wtime();
  //并行执行区域
  #pragma omp parallel
  {
    double partial_sum=0.0;
    //获取线程id
    long thread_ID = omp_get_thread_num();
    //获取实际运行的线程数
    if(!thread_ID) actual_RunThreads = omp_get_num_threads();
    //actual_RunThreads表示步长
    for(long i = thread_ID; i<num_steps ; i+=actual_RunThreads){
      //以块中间的函数值作为整块的面积
      double x = start +(i+0.5)*width;
      partial_sum+=func(x);
    }
    //互斥方式计算总面积，相当于原子操作
    #pragma omp critical
    {
      sum+=partial_sum;
    }
  }

  double runtime = omp_get_wtime()-start_time;
  std::cout<<"runtime: "<<runtime<<" actual_RunThreads: "<<actual_RunThreads<<std::endl;
  return sum*width;
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
