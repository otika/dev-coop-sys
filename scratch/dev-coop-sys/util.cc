#include "util.h"
#include "iostream"

namespace ns3 {
  // vectorの出力を見たいときのために
  void printVector(std::vector<double> vec){
    std::cout << "{" ;
    for (auto i = vec.begin(); i != vec.end(); ++i)
      std::cout << *i << ", ";
    std::cout << "}" << std::endl;
  }
}
