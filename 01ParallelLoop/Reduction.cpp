#include <iostream>
#include <omp.h>
#include <vector>

// template<typename T , typename FunctionType> T OmpReduction(const std::vector<T> &input ,const int &begin ,const int &size ,   FunctionType func ){
//   T ret;

//   /*
//    * 不可行，在共享循环构造中无法解决循环依赖问题。
//    * 例如下列规约中的 tmpRet变量，在归约中需要计算私有化的局部和。
//    *而以下的写法无法通过编译
//    * */
// // #pragma omp parallel for
// //   T tmpRet;
// //   for(int i = begin ; i!=(begin+size) ; ++i){
// //     tmpRet = func( tmpRet, input[i]);
// //   }
// //   #pragma omp critical 
// //   {
// //     ret = func(ret , tmpRet);
// //   }
// return ret; 
// }

template<typename T> T OmpReduction( const std::vector<T> &input, const int size , const int begin=0   ){
  T ret=0;
#pragma omp parallel for reduction(+:ret)
  for(int i=begin ; i!=(begin+size);++i){
    ret +=input[i];
  }
return ret;
}

int main(){
  std::vector<int> input(11,1);
  int result = OmpReduction( input,10, 1);
  std::cout <<"result: " <<result << " "<<std::endl;
}
