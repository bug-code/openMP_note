#include <iostream>
#include <omp.h>
#include <vector>

template<typename T> T OmpReduction( const std::vector<T> &input, const int size , const int begin=0   ){
  T ret=0;
#pragma omp parallel for reduction(+:ret) schedule(dynamic)
  for(int i=begin ; i!=(begin+size);++i){
    ret +=input[i];
  }
return ret;
}

int main(){
  std::vector<int> input(110,1);
  int result = OmpReduction( input,100, 1);
  std::cout <<"result: " <<result << " "<<std::endl;
}
