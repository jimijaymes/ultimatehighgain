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
 // Additional isolation measurements, without replacing the original pass criteria.
 for(bool eco:{false,true})for(bool powerOnly:{false,true})for(bool nofb:{false,true}){
 uhg::Engine e;e.controls.linkPower=false;e.controls.power=uhg::PowerModel::Marshall;e.controls.eco=eco;e.controls.powerOnly=powerOnly;e.controls.disableFeedback=nofb;e.prepare(48000);double idle=0,tail=0,signal=0;
 for(int n=0;n<48000;++n){double x=n>=12000&&n<24000?.04*std::sin(6.283185307*220*n/48000.):0.;double y=e.process(x);if(!std::isfinite(y))ok=false;if(n>=10000&&n<12000)idle+=y*y;if(n>=14000&&n<24000)signal+=y*y;if(n>=46000)tail+=y*y;}
 std::cout<<"Marshall isolation eco="<<eco<<" powerOnly="<<powerOnly<<" nofb="<<nofb<<" idle="<<std::sqrt(idle/2000)<<" signal="<<std::sqrt(signal/10000)<<" tail="<<std::sqrt(tail/2000)<<" rail="<<e.supply<<std::endl;
 }
 // Modular preamp-only output must be independent of power model at identical supply settings.
 for(int p=0;p<11;++p)for(bool spongy:{false,true})for(bool rect:{false,true}){
 uhg::Engine a,b;a.controls.preamp=b.controls.preamp=static_cast<uhg::PreampModel>(p);
 a.controls.linkPower=b.controls.linkPower=false;a.controls.power=uhg::PowerModel::Marshall;b.controls.power=uhg::PowerModel::AC30;a.controls.spongy=b.controls.spongy=spongy;a.controls.tubeRect=b.controls.tubeRect=rect;a.prepare(48000);b.prepare(48000);
 double rail=a.preampSupply();a.controls.spongy=false;a.controls.tubeRect=false;double normal=a.preampSupply();a.controls.spongy=spongy;a.controls.tubeRect=rect;if(rail!=b.preampSupply()||((spongy||rect)?!(rail<normal):rail!=normal)){std::cerr<<"Preamp rail control mismatch model="<<p<<std::endl;ok=false;}
 for(int n=0;n<2400;++n){double x=.04*std::sin(n*.0576);double y=a.processPreampOnly(x),z=b.processPreampOnly(x);if(!std::isfinite(y)||!std::isfinite(z)||y!=z){std::cerr<<"Independent preamp rail mismatch model="<<p<<" sample="<<n<<std::endl;ok=false;break;}}
 }
 return ok?0:1;
}
