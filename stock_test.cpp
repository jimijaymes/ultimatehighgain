#include "Preamp.h"
#include <iostream>
int main(){using namespace uhg; int count=0;for(int b=0;b<7;++b)for(int c=0;c<5;++c){Preamp a,z;for(int n=0;n<2400;++n){double x=.3*sin(n*.0576);double y=a.process(x,300,1./48000,PreampModel::Marshall,.7,.4,.6,.5,JoseEra::Open,true,.7,JoseArchitecture::Classic,ColdBias(b),CathodeBypass(c));double q=z.process(x,300,1./48000,PreampModel::Jose,.7,.4,.6,.5,JoseEra::Zener20,true,.1,JoseArchitecture::Stock,ColdBias(b),CathodeBypass(c));if(!std::isfinite(q)||q!=y)return 1;}++count;}std::cout<<count<<" exact stock equivalence tests passed\n";}
