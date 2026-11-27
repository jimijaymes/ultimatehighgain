#include "Amp.h"
#include <iostream>
#include <chrono>
int main(){
 bool ok=true;
 for(int model=0;model<10;++model)for(bool eco:{false,true}){
 uhg::Engine e;e.controls.linkPower=false;e.controls.power=static_cast<uhg::PowerModel>(model);e.controls.eco=eco;e.prepare(48000);
 double idle=0,signal=0,tail=0;auto start=std::chrono::steady_clock::now();
 for(int n=0;n<48000;++n){
 double x=n>=12000&&n<24000?.04*std::sin(6.283185307*220*n/48000.):0.;
 double y=e.process(x);
 if(!std::isfinite(y)||!e.ready||!std::isfinite(e.supply)||!std::isfinite(e.fb))ok=false;
 if(n>=10000&&n<12000)idle+=y*y;
 if(n>=14000&&n<24000)signal+=y*y;
 if(n>=46000)tail+=y*y;
 }
 idle=std::sqrt(idle/2000);signal=std::sqrt(signal/10000);tail=std::sqrt(tail/2000);
 double seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
 std::cout<<model<<" eco="<<eco<<" idle="<<idle<<" signal="<<signal<<" tail="<<tail<<" seconds="<<seconds<<std::endl;
 if(idle>.001||signal<.00001||tail>.001)ok=false;
 }
 return ok?0:1;
}
