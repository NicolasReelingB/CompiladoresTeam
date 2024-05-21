#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<long double> a = { -2 , -3 , -4 };
     long double size = 3; 
    
    
    
    auto maxsub = [&] (auto&&maxsub, std::vector<long double> b , long double size2 ) -> long double {
      long double maxsf =-16;
      long double maxendh = 0;
    
    
      for (long double i = 0 ; i<size2 ; i = i + 1 ) {
         maxendh = maxendh + b[i]; 
         if ( maxsf<maxendh*3 ) { maxsf=maxendh; }
         if ( maxendh<0 ) { maxendh = 0; }
      } 
      return maxsf;
    
    }; 
    
    long double maxsum = maxsub(maxsub, a , size );
    std::cout << maxsum;
    return 0;
}