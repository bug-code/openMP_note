# 并行化循环

## 共享工作循环构造

**共享工作构造:** 告诉编译器将构造中的工作分配给一组线程。
最常用的共享工作构造是共享工作循环构造。示例如下：

``````c++
// openMP parallel region and a worksharing-loop construct
#paragma omp parallel
{
    #paragma omp for
        for(int i=0 ; i!=N;++i){
            a[i] = a[i]+b[i];
        }
}

``````
在紧跟着`#paragma omp for`的语句后的循环中，编译器会将其中的控制索引在每个线程中私有化。例如其中的i。
但只在紧跟着`#paragma omp for`后的循环控制索引会被私有化。以上述伪代码为例，如果该循环内又嵌套了循环，
或该循环后又跟着一个循环，则其中的循环控制索引不会被私有化。

与第一章只介绍了`#paragma omp parallel`语句相比，`#paragma omp parallel`只说明了其中代码为多线程执行，具体线程如何组织
还需要手动指定。而共享工作循环构造不用手动指定线程组织。

示例程序，用多线程计算两个数组相加。


``````c++
#include <functional>
#include <iostream>
#include <omp.h>
#include <stdexcept>
#include <vector>


template <typename T, typename U> T plus(T lhs, U rhs) { return lhs + rhs; }

template <typename T,typename U ,  typename FunctionType>
std::vector<T> OmpOp(const std::vector<T> &lhs, const std::vector<U> &rhs,
                     int begin, const int size, FunctionType func) {
  std::vector<T> ret(size);
  // exception handling
  if (begin < 0 || begin > lhs.size() ||(begin+ size) > lhs.size() || lhs.empty() ||
      rhs.empty()) {
    throw std::invalid_argument("invalid argument");
  }
  // int i ;
  // func implement
  // 分块计算，每个线程计算一块
#pragma omp parallel 
{
  /* omp共享循环构造，在编译器生成代码时，
   * 会让每个线程都有一个自己的循环控制索引的私有副本,以避免数据竞争。
   * 例如下面的变量i
   * 这个规则仅使用于紧接着共享工作循环构造的循环
   * */
  #pragma omp for
  for(int i = begin ; i!=(begin+size) ; ++i){
    /*如果在这还有个嵌套循环，则该索引不会被私有化
     * */
    ret[i-begin] = func(lhs[i] , rhs[i]);
  }
}

return ret;
}


int main() {
  std::vector<int> lhs(11, 1);
  std::vector<int> rhs(12, 2);
  std::function<int(int, int)> f = plus<int, int>;
  std::vector<int> result = OmpOp(lhs, rhs, 1, 10, f);
  for (auto &val : result) {
    std::cout << val << " ";
  }
  std::cout<<std::endl;
  return 0;
}
``````
## 组合式并行共享工作循环构造
所谓组合式并行共享工作循环构造就是将上述的共享工作循环构造的两条语句合并为一条语句。

``````c++
#paragma omp parallel for
    for-loop
``````

## omp归约子句
omp中的归约与cuda中的归约差不多，如果了解cuda的规约实现，这个会更简单。
归约：将数组中的元素按线程数量分块，然后每个线程单独计算自己分块的结果，最后再计算所有分块的总计算结果。

``````c++
int i;
double ave = 0.0 , A[N];
InitA(A,N);

#paragma omp parallel for reduction (+:ave)
    for(i=0;i!=N;++i){
        ave+=A[i];
    }
ave = ave/N;

``````







