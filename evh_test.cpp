#include "Amp.h"
#include <iostream>
using namespace uhg;
int main(){double diff[5]={}; for(int k=0;k<5;++k){auto voice=static_cast<EVHVoice>(k);Preamp a,b;for(int i=0;i<4800;++i){double x=.3*sin(i*6.2831853*440/48000);double y=a.process(x,300,1./48000,PreampModel::EVH5150III,.7,.5,.5,.5,JoseEra::Zener20,true,.7,JoseArchitecture::Legacy,ColdBias::Native,CathodeBypass::Native,RectoVoice::EarlyRed,voice);double z=b.process(x,300,1./48000,(k==1||k==4)?PreampModel::EVHRed:PreampModel::EVHBlue,.7,.5,.5,.5);if(!std::isfinite(y))return 1;diff[k]+=std::abs(y-z);}std::cout<<k<<": "<<diff[k]<<'\n';}return !(diff[0]==0&&diff[1]==0&&diff[2]>0&&diff[3]>0&&diff[4]>0);}
