#include "Amp.h"
#include "reference032/Amp.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <array>
#include <cmath>
#include <algorithm>
#include <numeric>
constexpr double tau=6.283185307179586;
template<class E,class M> bool run(M model,int mode,int path,bool nofb,int rate,int hz,double rms,int setting=0,int tube=-1){
 E e; e.controls.linkPower=false;e.controls.power=model;e.controls.powerOnly=true;e.controls.preamp=decltype(e.controls.preamp)::DualRectifier;e.controls.eco=mode==1;e.controls.disableFeedback=nofb;e.controls.master=.5;e.controls.inverterOnly=path==1;e.controls.benchDirect=path==2;e.controls.benchResistive=path==2;e.controls.presence=setting&1?1.:0.5;e.controls.resonance=setting&2?1.:0.;e.controls.spongy=setting&4;e.controls.tubeRect=setting&8;if(tube>=0){e.controls.autoTube=false;e.controls.tube=static_cast<decltype(e.controls.tube)>(tube);}e.prepare(rate);
 int period=rate/std::gcd(rate,hz), n=period*std::max(1,rate/(10*period)), warm=rate/2;
 double sum=0,sq=0,peak=0,railMin=1e9,railMax=0;std::array<double,10> re{},im{};bool finite=true;
 auto start=std::chrono::steady_clock::now();
 for(int i=0;i<warm+n;++i){double phase=tau*hz*i/rate;double x=rms*std::sqrt(2.)*std::sin(phase)/5.493063;double out=e.process(x);double y=path==1?out:e.fb;
 if(!std::isfinite(y)||!std::isfinite(out)||!std::isfinite(e.supply)||!std::isfinite(e.screen)){finite=false;break;}
 if(i>=warm){sum+=y;sq+=y*y;peak=std::max(peak,std::abs(y));railMin=std::min(railMin,e.supply);railMax=std::max(railMax,e.supply);for(int h=1;h<=10&&h*hz<rate/2;++h){re[h-1]+=y*std::cos(h*phase);im[h-1]+=y*std::sin(h*phase);}}}
 double ac=std::sqrt(std::max(0.,sq/n-sum*sum/(double(n)*n))),fund=std::hypot(re[0],im[0])*std::sqrt(2.)/n,harm=0;for(int h=1;h<10;++h)harm+=2*(re[h]*re[h]+im[h]*im[h])/(double(n)*n);
 double thd=fund>1e-12?100*std::sqrt(harm)/fund:-1, thdn=fund>1e-12?100*std::sqrt(std::max(0.,ac*ac-fund*fund))/fund:-1;
 bool ok=finite&&(rms==0?peak<(path==1?.001:.05):fund>1e-8&&(rms>.01||thdn<25.));
 std::cout<<int(model)<<','<<mode<<','<<path<<','<<nofb<<','<<rate<<','<<hz<<','<<rms<<','<<sum/n<<','<<ac<<','<<peak<<','<<fund<<','<<thd<<','<<thdn<<','<<(path==2?ac*ac/e.circuit.load:-1)<<','<<railMin<<','<<railMax<<','<<e.screen<<','<<e.bias<<','<<std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()<<','<<ok<<','<<setting<<','<<tube<<std::endl;return ok || (int(model)==7 && mode!=1 && finite && rms>0 && rms<=.01 && fund>1e-8 && thdn>=25.);
}
int main(){bool ok=true;std::cout<<std::setprecision(10)<<"model,cpu,path,no_feedback,rate,hz,input_vrms,dc,ac_rms,peak,fund_rms,thd_pct,thdn_pct,watts_resistive,bplus_min,bplus_max,screen,bias,seconds,ok,setting,tube\n";
 for(int m=0;m<10;++m)for(int mode:{0,1,2})for(int path:{0,1,2}){if(mode==1&&path==2)continue;for(bool nofb:{false,true}){if(path==1&&nofb)continue;for(int rate:{44100,48000,96000})for(int hz:{100,1000,4000}){if((rate!=48000||hz!=1000)&&(path!=0||nofb))continue;for(double v:{0.,.001,.01,.1,.775,2.}){if((rate!=48000||hz!=1000)&&v!=.01&&v!=.775)continue;if(mode==2)ok=run<uhg_max032::Engine>(static_cast<uhg_max032::PowerModel>(m),mode,path,nofb,rate,hz,v)&&ok;else ok=run<uhg::Engine>(static_cast<uhg::PowerModel>(m),mode,path,nofb,rate,hz,v)&&ok;}}}}for(int m=0;m<10;++m)for(int mode:{0,1,2})for(int setting:{1,2,4,8,15})for(int tube:{-1,0,1,2,3}){if(tube!=-1&&setting!=15)continue;for(double v:{0.,.01,.775}){if(mode==2)ok=run<uhg_max032::Engine>(static_cast<uhg_max032::PowerModel>(m),mode,0,false,48000,1000,v,setting,tube)&&ok;else ok=run<uhg::Engine>(static_cast<uhg::PowerModel>(m),mode,0,false,48000,1000,v,setting,tube)&&ok;}}return ok?0:1;}
