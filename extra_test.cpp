#include "Preamp.h"
#include <iostream>
int main(){double rms[3]{}; for(int m=0;m<3;++m){uhg::Preamp p;for(int i=0;i<12000;++i){double y=p.process(.25*std::sin(6.283185307*440*i/48000.),300,1./48000,uhg::PreampModel::Jose,.7,.5,.5,.5,uhg::JoseEra::Zener20,m!=0,m==1?.2:.9);if(!std::isfinite(y))return 1;if(i>=6000)rms[m]+=y*y;}rms[m]=std::sqrt(rms[m]/6000);std::cout<<rms[m]<<'\n';} return !(rms[0]>0 && rms[1]>0 && rms[2]>0 && std::abs(rms[1]-rms[2])>1e-5 && std::abs(rms[0]-rms[2])>1e-5);}
