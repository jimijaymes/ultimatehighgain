#include "JoseClip.h"
#include <iostream>
int main(){using namespace uhg; double peaks[4]={}; int j=0; for(auto mode:{JoseEra::Open,JoseEra::Zener20,JoseEra::Zener12,JoseEra::Asymmetric}){JoseClip c; for(int i=0;i<48000;++i){double y=c.process(60*sin(6.283185307*1000*i/48000.),1./48000,mode);if(!std::isfinite(y))return 1;if(i>24000)peaks[j]=std::max(peaks[j],std::abs(y));}std::cout<<peaks[j++]<<'\n';} return !(peaks[0]>peaks[1] && peaks[1]>peaks[2]);}
