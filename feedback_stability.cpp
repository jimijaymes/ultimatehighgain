#include "Amp.h"
#include "reference032/Amp.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <type_traits>
#include <algorithm>
template<class E> bool run(int m,int cpu,int path,bool nf,int rate,double presence,double depth){
 E e; e.controls.linkPower=false;e.controls.power=static_cast<decltype(e.controls.power)>(m);e.controls.eco=cpu==1;if constexpr(std::is_same_v<E,uhg::Engine>)e.controls.light=cpu==3;
 e.controls.powerOnly=path!=0;e.controls.inverterOnly=path==2;e.controls.disableFeedback=nf;e.controls.presence=presence;e.controls.resonance=depth;e.controls.master=1.;e.prepare(rate);
 double sq[3]={},sum[3]={},pk[3]={};int count[3]={};bool finite=true;auto start=std::chrono::steady_clock::now();
 for(int n=0;n<rate*2;++n){int phase=n<rate/2?0:n<rate?1:2;double x=phase==1?.04*std::sin(6.283185307179586*220*n/rate):0.;double y=e.process(x);if(!std::isfinite(y)||!std::isfinite(e.fb)||!std::isfinite(e.supply)||!std::isfinite(e.screen)){finite=false;break;}if((phase==0&&n<rate*2/5)||(phase==1&&n<rate*3/5)||(phase==2&&n<rate*19/10))continue;sq[phase]+=y*y;sum[phase]+=y;pk[phase]=std::max(pk[phase],std::abs(y));++count[phase];}
 double rms[3]={};for(int i=0;i<3;++i)rms[i]=std::sqrt(sq[i]/std::max(1,count[i]));bool ok=finite&&rms[0]<.001&&rms[1]>1e-5&&rms[2]<.001;
 std::cout<<m<<','<<cpu<<','<<path<<','<<nf<<','<<rate<<','<<presence<<','<<depth;for(int i=0;i<3;++i)std::cout<<','<<rms[i]<<','<<pk[i]<<','<<sum[i]/std::max(1,count[i]);std::cout<<','<<std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()<<','<<finite<<','<<ok<<std::endl;return ok;
}
int main(){std::cout<<"model,cpu,path,no_feedback,rate,presence,depth,idle_rms,idle_peak,idle_dc,signal_rms,signal_peak,signal_dc,tail_rms,tail_peak,tail_dc,seconds,finite,ok\n";bool ok=true;for(int m=0;m<10;++m)for(int cpu=0;cpu<4;++cpu)for(int path=0;path<3;++path)for(bool nf:{false,true})for(int rate:{44100,48000,96000})for(double p:{0.,.5,1.}){if(rate!=48000&&p!=.5)continue;double d=p;if(cpu==2)ok=run<uhg_max032::Engine>(m,cpu,path,nf,rate,p,d)&&ok;else ok=run<uhg::Engine>(m,cpu,path,nf,rate,p,d)&&ok;}return ok?0:1;}
