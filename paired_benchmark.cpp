#include "Amp.h"
#include "baseline032/Amp.h"
#include <chrono>
#include <iostream>
#include <array>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <vector>
template<class E> double measure(int model,int rate,const std::vector<double>& x,std::vector<double>& y){
 E e; e.controls.linkPower=false;e.controls.power=static_cast<decltype(e.controls.power)>(model);e.controls.eco=false;e.controls.powerOnly=true;e.prepare(rate);
 for(int i=0;i<rate/10;++i)e.process(0.);
 auto start=std::chrono::steady_clock::now();
 for(size_t i=0;i<x.size();++i)y[i]=e.process(x[i]);
 return std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
}
int main(){std::cout<<std::setprecision(10)<<"model,rate,input,baseline_seconds,current_seconds,speedup_pct,relative_rms_error,peak_error\n";
 for(int m=0;m<10;++m)for(int rate:{48000,96000})for(double level:{.001,.01,.1}){
 std::vector<double>x(rate/10),a(x.size()),b(x.size());for(size_t i=0;i<x.size();++i)x[i]=level*std::sin(6.283185307179586*1000*i/rate);
 std::array<double,3> old{},now{};for(int j=0;j<3;++j){if(j%2){now[j]=measure<uhg::Engine>(m,rate,x,b);old[j]=measure<uhg_baseline032::Engine>(m,rate,x,a);}else{old[j]=measure<uhg_baseline032::Engine>(m,rate,x,a);now[j]=measure<uhg::Engine>(m,rate,x,b);}}
 std::sort(old.begin(),old.end());std::sort(now.begin(),now.end());double err=0,ref=0,peak=0;for(size_t i=0;i<a.size();++i){if(!std::isfinite(a[i])||!std::isfinite(b[i]))return 1;double d=a[i]-b[i];err+=d*d;ref+=a[i]*a[i];peak=std::max(peak,std::abs(d));}
 std::cout<<m<<','<<rate<<','<<level<<','<<old[1]<<','<<now[1]<<','<<100*(1-now[1]/old[1])<<','<<std::sqrt(err/std::max(ref,1e-30))<<','<<peak<<std::endl;
 }return 0;}
