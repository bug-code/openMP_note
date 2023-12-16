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
#pragma omp parallel for
  /* omp组合式共享循环构造，不要有区域花括号
   * 在编译器生成代码时，
   * 会让每个线程都有一个自己的循环控制索引的私有副本,以避免数据竞争。
   * 例如下面的变量i
   * 这个规则仅使用于紧接着共享工作循环构造的循环
   * */
  for(int i = begin ; i!=(begin+size) ; ++i){
    /*如果在这还有个嵌套循环，则该索引不会被私有化
     * */
    ret[i-begin] = func(lhs[i] , rhs[i]);
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
