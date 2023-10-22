# OpenMP模型
gcc编译openmp指令：gcc test.cpp -o test -fopenmp
## 定积分计算 函数面积
给定一个定积分，计算其面积：
$$ \int^{1}_{0}{\frac{4.0}{(1+x^2)}dx}$$

## omp 概念
### 并行区域
并行区域用于多线程并行执行指令

``````c++
#pragma omp parallel
{
    ... do lots of stuff
}//end of parallel region
``````
### 临界区
临界区用于解决：内存竞争问题，该区块中的代码只能有一个线程执行，其他线程必须等待。


``````c++
#pragma omp critical
{
    ... one or more lines of code
}
``````

### 栅栏
栅栏用于解决线程同步问题，通过栅栏设置的锚点，必须等所有线程均执行到该位置才能继续往下执行。


``````c++
#pragma omp barrier
//... continue work

``````
栅栏通常用于这种情况，即必须在所有线程完成栅栏前的工作内容，才能继续往下执行。栅栏后的工作内容应当依赖所有线程栅栏前的工作全部完成才能执行，否则得不偿失。
因为栅栏所带来开销很大。这一点相当于cuda中的同步函数。

### omp相关函数
- `int omp_get_num_threads()` : 获取正在运行的线程数
- `int omp_get_thread_num()`  : 获取线程id
- `void omp_set_num_threads(int)` : 获取要执行的线程数
- `double omp_get_wtime()` : 返回以秒为单位的时间

### 串行执行
以横坐标为单位划分为指定数量的小块，取每个小块的中值作为该块的值，累加获得最终值。代码如下：


``````c++
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
``````

### 以padding的方式解决多线程计算总和问题


``````c++
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
``````
### 以临界区方式计算面积总和问题

``````c++
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
``````



## 执行时间统计
| 线程数|串行 |OMP|OMP消除伪共享|OMP临界区同步|
|:--------|:--------|:---------|:-----------|:---------|
|1        | 0.290821|2.49737   |2.51376     |2.50391   |       
|2        | 0.290821|2.46001   |2.55435     |1.25341   |       
|3        | 0.290821|2.92166   |1.90781     |0.991777  |       
|4        | 0.290821|4.09134   |1.26799     |0.630626  |       
|5        | 0.290821|3.13144   |2.53815     |0.627318  |       
|6        | 0.290821|2.93112   |1.24541     |0.579776  |       
|7        | 0.290821|2.83655   |2.95334     |0.527468  |       
|8        | 0.290821|2.9195    |0.826844    |0.458585  |       
|9        | 0.290821|2.39431   |0.852851    |0.409409  |       
|10       | 0.290821|3.20907   |0.82156     |0.381328  |       
|11       | 0.290821|3.02943   |1.91977     |0.352992  |       
|12       | 0.290821|2.95709   |0.737233    |0.343221  |       
