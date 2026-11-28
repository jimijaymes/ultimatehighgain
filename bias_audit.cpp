#include "Amp.h"
#include "reference032/Amp.h"
#include <iostream>
#include <cmath>
#include <chrono>
template<class E,class M> void run(M model,int mode,bool nofb,double input){
 E e;e.controls.linkPower=false;e.controls.power=model;e.controls.powerOnly=true;e.controls.master=1.;e.controls.eco=mode==1;e.controls.light=mode==3;e.controls.disableFeedback=nofb;e.prepare(48000);
 constexpr int n=4800;constexpr double w=6.283185307179586/48.;for(int i=0;i<24000;++i)e.step(0.);
 double idle1=e.measuredPlate1,idle2=e.measuredPlate2,idles=e.measuredScreen,b=e.bias,rail=e.supply,screen=e.screen;
 for(int i=0;i<n;++i)e.step(input*std::sqrt(2.)*std::sin(w*i)/5.493063);
 double sum=0,ss=0,re=0,im=0,pk=0;auto start=std::chrono::steady_clock::now();
 for(int i=0;i<n;++i){e.step(input*std::sqrt(2.)*std::sin(w*i)/5.493063);double y=e.fb;sum+=y;ss+=y*y;re+=y*std::cos(w*i);im+=y*std::sin(w*i);pk=std::max(pk,std::abs(y));}
 double ac=std::sqrt(std::max(0.,ss/n-sum*sum/(double(n)*n))),fund=std::hypot(re,im)*std::sqrt(2.)/n;
 std::cout<<int(model)<<','<<mode<<','<<nofb<<','<<input<<','<<b<<','<<rail<<','<<screen<<','<<(mode==1?-1:idle1)<<','<<(mode==1?-1:idle2)<<','<<(mode==1?-1:idles)<<','<<fund<<','<<ac<<','<<(fund>1e-12?100*std::sqrt(std::max(0.,ac*ac-fund*fund))/fund:-1)<<','<<pk<<','<<std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()<<'\n';
}
int main(){std::cout<<"model,cpu,no_feedback,input_vrms,bias_v,idle_bplus_v,idle_screen_v,idle_plate1_a,idle_plate2_a,idle_screen_a,fund_vrms,ac_vrms,thdn_pct,peak_v,seconds\n";for(int m=0;m<10;++m)for(int mode=0;mode<4;++mode)for(bool nf:{false,true})for(double v:{0.,.001,.01,.05,.1,.25,.5,1.,2.}){if(mode==2)run<uhg_max032::Engine>(static_cast<uhg_max032::PowerModel>(m),mode,nf,v);else run<uhg::Engine>(static_cast<uhg::PowerModel>(m),mode,nf,v);}}
