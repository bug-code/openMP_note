#include <functional>
#include <iostream>
#include <iterator>
#include <omp.h>
#include <stdexcept>
#include <vector>
/*
 * 简单计算两向量相加
 */
template <typename T, typename U> T plus(T lhs, U rhs) { return lhs + rhs; }
template <typename T,typename U ,  typename FunctionType>
std::vector<T> OmpOp(const std::vector<T> &lhs, const std::vector<U> &rhs,
                     int begin, const int size, FunctionType func) {
  std::vector<T> ret(size);
  // exception handling
  if (begin < 0 || begin > lhs.size() || (begin+size) > lhs.size() || lhs.empty() ||
      rhs.empty()) {
    throw std::invalid_argument("invalid argument");
  }

  // func implement
  // 分块计算，每个线程计算一块
#pragma omp parallel 
{
  // get threadId
  long threadId = omp_get_thread_num();
  int actual_RunTheads = omp_get_num_threads();
  // 计算该线程起始位置
  int start = begin + threadId * size / actual_RunTheads;
  int end = begin + (threadId + 1) * size / actual_RunTheads;
  if (threadId == (actual_RunTheads - 1))
    end = begin + size;
  for (int i = start; i != end; ++i) {
    ret[i-begin] = func(lhs[i], rhs[i]);
  }
}

return ret;
}

int main() {
  std::vector<int> lhs(12, 1);
  std::vector<int> rhs(12, 2);
  std::function<int(int, int)> f = plus<int, int>;
  std::vector<int> result = OmpOp(lhs, rhs, 1, 10, f);
  for (auto &val : result) {
    std::cout << val << " ";
  }
  std::cout<<std::endl;
  return 0;
}
