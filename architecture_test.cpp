#include "Preamp.h"
#include <iostream>
int main(){using namespace uhg; for(auto a:{JoseArchitecture::Classic,JoseArchitecture::HBE,JoseArchitecture::Snorkler})for(int b=0;b<6;++b)for(int c=0;c<5;++c){Preamp p;double sum=0;for(int i=0;i<2400;++i){double y=p.process(.3*sin(i*6.2831853*440/48000),300,1./48000,PreampModel::Jose,.7,.5,.5,.5,JoseEra::Zener20,true,.7,a,ColdBias(b),CathodeBypass(c));if(!std::isfinite(y))return 1;sum+=y*y;}if(sum<1e-12)return 2;}std::cout<<"90 architecture/bias/bypass combinations passed\n";}
