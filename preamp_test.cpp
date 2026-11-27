#include "Preamp.h"
#include <iostream>
int main(){for(int m=0;m<9;++m){uhg::Preamp p;double sum=0,peak=0;for(int n=0;n<24000;++n){double x=.2*std::sin(6.283185307*440*n/48000.);double y=p.process(x,300,1./48000,static_cast<uhg::PreampModel>(m),.7,.5,.5,.5);if(!std::isfinite(y))return 1;if(n>12000){sum+=y*y;peak=std::max(peak,std::abs(y));}}std::cout<<m<<" rms "<<std::sqrt(sum/11999)<<" peak "<<peak<<'\n';if(sum<1e-8)return 2;}}
