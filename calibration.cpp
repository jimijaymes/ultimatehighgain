#include "Amp.h"
#include "reference032/Amp.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <complex>
#include <array>
#include <string>
constexpr double rate=48000, tau=6.283185307179586;
template<class E,class M> bool run(M model,int mode,bool resistive,double hz,double rms){
 E e; e.controls.linkPower=false;e.controls.power=model;e.controls.powerOnly=true;e.controls.benchDirect=true;e.controls.benchResistive=resistive;e.controls.eco=mode==1;e.prepare(rate);
 const int period=int(rate/hz), n=period*20;double sum=0,sq=0,peak=0;std::array<std::complex<double>,10> bins{};
 auto start=std::chrono::steady_clock::now();
 for(int i=0;i<24000+n;++i){double phase=tau*hz*i/rate;double x=rms*std::sqrt(2.)*std::sin(phase)/5.493063;e.step(x);double y=e.fb;if(!std::isfinite(y)||!std::isfinite(e.supply))return false;if(i>=24000){sum+=y;sq+=y*y;peak=std::max(peak,std::abs(y));for(int h=1;h<=10;++h)bins[h-1]+=y*std::polar(1.,-h*phase);}}
 double ac=std::sqrt(std::max(0.,sq/n-sum*sum/(double(n)*n))),fund=std::abs(bins[0]),harm=0;for(int h=1;h<10;++h)harm+=std::norm(bins[h]);
 std::cout<<int(model)<<','<<mode<<','<<resistive<<','<<hz<<','<<rms<<','<<sum/n<<','<<ac<<','<<(resistive?ac*ac/e.circuit.load:-1)<<','<<(fund>1e-9?100*std::sqrt(harm)/fund:-1)<<','<<peak<<','<<e.supply<<','<<e.screen<<','<<e.bias<<','<<e.cathodeV<<','<<std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()<<'\n';return true;
}
int main(int argc,char**argv){bool all=argc>1&&std::string(argv[1])=="--all";bool ok=true;std::cout<<std::setprecision(10)<<"model,cpu,resistive,hz,input_vrms,dc_v,output_vrms,watts_resistive,thd_pct,peak_v,bplus,screen,bias,cathode,seconds\n";for(int m=0;m<10;++m){if(!all&&m!=2&&m!=3)continue;for(int mode=0;mode<3;++mode)for(bool resistive:{true,false})for(double hz:{100.,1000.,4000.}){if(!all&&hz!=1000)continue;for(double v:{0.,.01,.05,.1,.25,.5,1.,2.,4.}){if(mode==2)ok=run<uhg_max032::Engine>(static_cast<uhg_max032::PowerModel>(m),mode,resistive,hz,v)&&ok;else ok=run<uhg::Engine>(static_cast<uhg::PowerModel>(m),mode,resistive,hz,v)&&ok;}}}return ok?0:1;}
