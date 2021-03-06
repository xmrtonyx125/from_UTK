#ifndef __RNUMGEN_H__
#define __RNUMGEN_H__

#include <iostream>
#include <algorithm>    // std::transform
#include <vector> 
#include <numeric>
#include <functional>
using namespace std;
class rnumgen {
  public:
    rnumgen(int seedvalue, std::vector<int> &v);
    int rand() const;

  private:
    std::vector<float> F;
};


#endif
