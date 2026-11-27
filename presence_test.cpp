#include "Amp.h"
#include <iostream>
int main(){for(double f:{100.,1000.,5000.}){double result[2];for(int p=0;p<2;++p){uhg::PresenceNetwork n;double ss=0;for(int i=0;i<192000;++i){double y=n.process(std::sin(6.283185307*f*i/192000.),1./192000,100000,4700,22000*(1-p),100e-9);if(i>=96000)ss+=y*y;}result[p]=std::sqrt(ss/96000)*std::sqrt(2.);}std::cout<<f<<" Hz beta min/max presence "<<result[0]<<" "<<result[1]<<'\n';if(f>=1000 && !(result[1]<result[0]))return 1;}}
