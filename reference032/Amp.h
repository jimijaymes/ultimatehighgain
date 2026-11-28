#pragma once
#include <cmath>
#include <algorithm>
#include "Circuits.h"
#include "Tubes.h"
#include "Preamp.h"
namespace uhg_max032 {
inline double clamp(double x,double a,double b){return std::clamp(x,a,b);}
inline double lp(double x,double &z,double f,double dt){z+=(1-std::exp(-6.283185307*f*dt))*(x-z);return z;}
// Reduced-order RC feedback divider, with series presence rheostat and capacitor.
struct PresenceNetwork {
 double capacitor=0;
 double process(double tap,double dt,double rf,double rg,double rp,double c){
  const double rth=rf*rg/(rf+rg), vth=tap*rg/(rf+rg);
  const double a=dt/c, i=(vth-capacitor)/(rth+rp+a);
  capacitor+=a*i;
  return vth-rth*i;
 }
};
struct Circuit {double B=450, screen=440, primary=1700, load=8, pairs=2, reservoir=50e-6, supplyR=65, screenR=1000, feedbackR=100000, feedbackGround=4700, presenceR=25000, presenceC=100e-9, feedbackTap=4, transformerL=20, leakage=.001, fluxLimit=.65;};
enum class PowerModel { Marshall, Peavey, RectoVintage, RectoModern, VHT, EVH, MiniRecto, AC30, AC50, SLO };
// Provisional architecture seeds; not verified schematic transcriptions.
inline Circuit powerCircuit(PowerModel m){ Circuit q; switch(m){
case PowerModel::Peavey: q.B=490;q.screen=475;q.primary=1900;q.feedbackR=47000;q.reservoir=100e-6;q.supplyR=35;break;
case PowerModel::RectoVintage: case PowerModel::RectoModern: q.B=460;q.screen=450;q.primary=1900;q.reservoir=100e-6;q.feedbackR=47000;q.supplyR=45;break;
case PowerModel::VHT: q.B=500;q.screen=480;q.primary=1900;q.feedbackR=33000;q.supplyR=25;q.reservoir=150e-6;break;
case PowerModel::EVH: q.B=480;q.screen=465;q.primary=1900;q.feedbackR=47000;q.supplyR=35;q.reservoir=100e-6;break;
case PowerModel::MiniRecto: q.B=350;q.screen=330;q.primary=8000;q.pairs=1;q.reservoir=47e-6;q.supplyR=85;break;
case PowerModel::AC30: q.B=320;q.screen=310;q.primary=4000;q.reservoir=32e-6;q.supplyR=90;break;
case PowerModel::AC50: q.B=440;q.screen=420;q.primary=3400;q.pairs=1;q.feedbackR=56000;q.reservoir=50e-6;break;
case PowerModel::SLO: q.B=500;q.screen=480;q.primary=1900;q.feedbackR=39000;q.reservoir=100e-6;q.supplyR=35;break;
default: break;}return q;}
inline Tube nativeTube(PowerModel m){switch(m){case PowerModel::AC30:case PowerModel::MiniRecto:return Tube::EL84;case PowerModel::Peavey:case PowerModel::RectoVintage:case PowerModel::RectoModern:case PowerModel::VHT:case PowerModel::EVH:case PowerModel::SLO:return Tube::L6;default:return Tube::EL34;}}
inline PhaseInverterSpec inverterSpec(PowerModel m){PhaseInverterSpec p;switch(m){case PowerModel::RectoVintage:case PowerModel::RectoModern:p.coupling=47e-9;p.gridStop=1500;break;case PowerModel::AC30:p.plate1=100000;p.plate2=100000;p.tail=48200;p.biasR=1200;p.coupling=150e-9;p.gridStop=1500;break;case PowerModel::MiniRecto:case PowerModel::Peavey:case PowerModel::EVH:p.coupling=47e-9;p.gridStop=1500;break;case PowerModel::VHT:case PowerModel::SLO:p.coupling=100e-9;p.gridStop=2200;break;default:break;}return p;}
struct Controls {bool disableFeedback=false;bool inverterOnly=false;bool powerOnly=false;bool eco=false;bool autoTube=true;bool linkPower=true; PowerModel power=PowerModel::Marshall;double presence=.5, gain=.5, master=.25, bass=.5, mid=.5, treble=.5; bool spongy=false,tubeRect=false; Tube tube=Tube::EL34; PreampModel preamp=PreampModel::Marshall; JoseEra joseEra=JoseEra::Zener20; bool joseExtraStage=true; double joseGain2=.7; double volume=1.; bool josePostEQ=false; bool joseSAT=false; int brightMode=1; JoseArchitecture joseArchitecture=JoseArchitecture::Legacy; ColdBias joseBias=ColdBias::Native; CathodeBypass joseBypass=CathodeBypass::Native; ColdBias coldBias=ColdBias::Native; CathodeBypass coldBypass=CathodeBypass::Native; RectoVoice rectoVoice=RectoVoice::EarlyRed; EVHVoice evhVoice=EVHVoice::Blue;};
struct Triode {
 double cap=0, blocking=0;
 double process(double input,double dt,double supply,double cathodeR){
  // Solve static plate load line and self-bias approximation by bisection.
  auto solve=[&](double grid){double lo=0,hi=supply;for(int j=0;j<18;++j){double p=(lo+hi)*.5;double i=(supply-p)/100000.;double model=triodeCurrent(p-i*cathodeR,grid-i*cathodeR);if(model>i)hi=p;else lo=p;}return (lo+hi)*.5;};
  double grid=input-blocking;lp(std::max(0.,grid-1.)*.35,blocking,8,dt);
  double p=solve(grid);return p-lp(p,cap,12,dt);
 }
};
struct Engine {
 struct Prepared {double dt=0;PowerModel model=PowerModel::Marshall;bool tubeRect=false;double invRatio,magStep,invFlux,capG,invG,coilG,invDen,indStep,rs,supplyStep,invSupplyR,screenAlpha,feedbackScale;}; Prepared coeff{};
 Circuit circuit; Controls controls; int oversampling=1; double fs=48000,dt=1./48000; Preamp preamp; PhaseInverter pi; GridCoupling grid1,grid2; double magI=0; PresenceNetwork presenceNet; double supply=450, screen=440,bias=-40, fb=0,fbLow=0,flux=0,leak=0,capV=0,indI=0,coilI=0, bassZ=0,trebleZ=0, prev=0,gridBlock=0; double sagCurrentSum=0, sagScreenSum=0; int sagCount=0; double debugPreamp=0; double outputDC=0; double screenEstimate1=440,screenEstimate2=440; double peakPlate=0,cathodeV=0,cutState=0,modernState=0; Tube active=Tube::EL34; bool ready=false; PowerModel activePower=PowerModel::Marshall;
 PowerModel selectedPower() const {if(!controls.linkPower)return controls.power; switch(controls.preamp){case PreampModel::Peavey5150:return PowerModel::Peavey;case PreampModel::DualRectifier:return rectoModern(controls.rectoVoice)?PowerModel::RectoModern:PowerModel::RectoVintage;case PreampModel::VHT:return PowerModel::VHT;case PreampModel::EVH5150III:return PowerModel::EVH;default:return PowerModel::Marshall;}}

 void prepare(double rate){fs=rate;dt=1/(oversampling*fs);reset();}
 void reset(){sagCurrentSum=sagScreenSum=0;sagCount=0;preamp={};pi={};grid1={};grid2={};magI=0;presenceNet={};supply=circuit.B;screen=circuit.screen;fb=fbLow=flux=leak=capV=indI=coilI=bassZ=trebleZ=prev=gridBlock=peakPlate=0;active=controls.tube;outputDC=cathodeV=cutState=modernState=0;ready=false;}
 double biasFor(TubeData t,double v,double s){double lo=-200,hi=0;double target=std::min(t.idle,.65*t.rating/std::max(1.,v));for(int i=0;i<32;++i){double m=(lo+hi)*.5;if(plate(t,v,m,s)>target)hi=m;else lo=m;}return (lo+hi)*.5;}
 double step(double x){auto m=selectedPower();if(m!=activePower){circuit=powerCircuit(m);activePower=m;reset();}auto &q=circuit;
 if(coeff.dt!=dt||coeff.model!=m||coeff.tubeRect!=controls.tubeRect){
 coeff.dt=dt;coeff.model=m;coeff.tubeRect=controls.tubeRect;
 coeff.invRatio=std::sqrt(q.load/q.primary);coeff.magStep=dt/q.transformerL;coeff.invFlux=1/q.fluxLimit;
 constexpr double Lm=.018,omega=6.283185307*95.; // Fixed generic 4x12 reactive impedance
 coeff.capG=1/(omega*omega*Lm*dt);coeff.indStep=dt/Lm;
 coeff.invG=1/(1/(q.load*5.45)+coeff.indStep+coeff.capG);
 coeff.coilG=(.0005+q.leakage*q.load/q.primary)/dt;coeff.rs=q.load*.12;
 coeff.invDen=1/(q.load*.8+coeff.rs+coeff.coilG+coeff.invG);
 coeff.supplyStep=dt/q.reservoir;coeff.invSupplyR=1/(q.supplyR+(controls.tubeRect?65:0));
 coeff.screenAlpha=-std::expm1(-6.283185307*35*dt);coeff.feedbackScale=std::sqrt(q.feedbackTap/q.load);
 }
 auto chosen=controls.autoTube?(m==PowerModel::EVH&&controls.evhVoice==EVHVoice::BlueEL34?Tube::EL34:nativeTube(m)):controls.tube;auto t=data(chosen);auto ps=inverterSpec(m);bool cathodeBias=m==PowerModel::AC30;double target=q.B*(controls.spongy?.85:1.)-(controls.tubeRect?20:0);if(!ready||active!=chosen){bias=biasFor(t,target,q.screen*target/q.B);cathodeV=cathodeBias?-bias:0;grid1.charge=cathodeBias?0:-bias;grid2.charge=grid1.charge;active=chosen;ready=true;}
 // 0 dBu = .775 Vrms; -14 dBFS sine = .1995 peak.
 double v=x*5.493063;
 if(!controls.powerOnly){
 v=preamp.process(v,supply*.65,dt,controls.preamp,controls.gain,controls.bass,controls.mid,controls.treble,controls.joseEra,controls.joseExtraStage,controls.joseGain2,controls.joseArchitecture,controls.preamp==PreampModel::Jose?controls.joseBias:controls.coldBias,controls.preamp==PreampModel::Jose?controls.joseBypass:controls.coldBypass,controls.rectoVoice,controls.evhVoice,controls.eco,controls.joseSAT?1.:controls.volume,controls.joseArchitecture==JoseArchitecture::JosePostEQ,controls.brightMode,controls.presence);
 if(controls.preamp!=PreampModel::Jose)v*=controls.volume;
 } v*=controls.master*controls.master;
 debugPreamp=std::isfinite(v)?std::clamp(v/30.,-1.,1.):0.;
 // Feedback uses previous oversampled secondary voltage; bounded explicit loop.
 double feedback=(controls.disableFeedback||activePower==PowerModel::RectoModern||activePower==PowerModel::AC30)?0.:presenceNet.process(fb*coeff.feedbackScale,dt,q.feedbackR,q.feedbackGround,q.presenceR*(1-clamp(controls.presence,0.,1.)),q.presenceC);
 double d1,d2;if(controls.inverterOnly){pi.process(clamp(v,-30.,30.),0.,supply*.72,dt,d1,d2,ps,controls.eco);return (d1-d2)/100.;}pi.process(clamp(v,-30.,30.),feedback,supply*.72,dt,d1,d2,ps,controls.eco);
 if(m==PowerModel::AC30){double diff=d1-d2;double r=1000.+250000.*(1-controls.presence);double sourceR=100000.;double a=dt/4.7e-9;double i=(diff-cutState)/(r+sourceR+a);cutState+=a*i;d1-=i*sourceR*.5;d2+=i*sourceR*.5;}
 if(m==PowerModel::RectoModern&&!isRecto(controls.preamp)){double diff=d1-d2;double high=diff-lp(diff,modernState,2200,dt);double atten=.65*(1-controls.presence)*high;d1-=atten*.5;d2+=atten*.5;}
 double g1=grid1.process(d1,cathodeBias?0:bias,dt,ps,cathodeV)-cathodeV,g2=grid2.process(d2,cathodeBias?0:bias,dt,ps,cathodeV)-cathodeV;

 // Solve plate/screen current against the same instantaneous reflected voltage.
 struct Branch {double ip,is;};
 auto branch=[&](double vp,double vg){const double plateV=vp-cathodeV;double low=0,high=screen,fl=-screen,fh=screenCurrent(t,plateV,vg,std::max(0.,screen-cathodeV))*q.screenR;double vs=screen,lastScreenCurrent=0;for(int j=0;j<(controls.eco?3:6);++j){vs=low+(high-low)*(-fl)/(fh-fl);lastScreenCurrent=screenCurrent(t,plateV,vg,std::max(0.,vs-cathodeV));double f=vs+lastScreenCurrent*q.screenR-screen;if(std::abs(f)<.05)break;if(f>0){high=vs;fh=f;fl*=.5;}else{low=vs;fl=f;fh*=.5;}}return Branch{plate(t,plateV,vg,std::max(0.,vs-cathodeV)),lastScreenCurrent};};
 // Implicit quasi-static primary load line; speaker dynamic state remains explicit.
 const double loadHistory=coeff.capG*capV-indI;
 const double reflectedHistory=(loadHistory*coeff.invG-coeff.coilG*coilI)/coeff.invRatio;
 const double reflectedZ=1/(coeff.invDen*coeff.invRatio*coeff.invRatio);
 double lo=-2*supply,hi=2*supply;
 Branch cachedA{},cachedB{};double cachedV=0;bool cached=false;
 auto residual=[&](double v){auto a=branch(supply-v*.5,g1),b=branch(supply+v*.5,g2);cachedA=a;cachedB=b;cachedV=v;cached=true;return ((a.ip-b.ip)*q.pairs*.5-magI)*reflectedZ+reflectedHistory-v;};
 double primaryV=0;
 for(int k=0;k<(controls.eco?12:16);++k){double v=(lo+hi)*.5;double f=residual(v);if(std::abs(f)<.002){lo=hi=v;break;}if(f>0)lo=v;else hi=v;}
 primaryV=(lo+hi)*.5;const bool reuse=cached&&cachedV==primaryV;auto a1=reuse?cachedA:branch(supply-primaryV*.5,g1),a2=reuse?cachedB:branch(supply+primaryV*.5,g2);double i1=a1.ip,i2=a2.ip;
 peakPlate=std::max(peakPlate,std::max(i1,i2)*supply);
 double screens=(a1.is+a2.is)*q.pairs;if(cathodeBias){double rk=47.;double ck=250e-6;cathodeV=clamp((cathodeV+dt*((i1+i2)*q.pairs+screens)/ck)/(1+dt/(rk*ck)),0.,80.);}double current=(i1+i2)*q.pairs+screens+.008;
 double recharge=std::max(0.,(target-supply)*coeff.invSupplyR);supply=clamp(supply+coeff.supplyStep*(recharge-current),target*.45,target);
 screen+=coeff.screenAlpha*(clamp(supply-(q.B-q.screen)-screens*100,30.,supply)-screen);
 double emf=primaryV;magI+=coeff.magStep*(emf-magI*100);flux+=dt*(emf-flux*35);
 double normalizedFlux=flux*coeff.invFlux;double sat=1/std::sqrt(1+normalizedFlux*normalizedFlux);
 double source=clamp(emf*sat*coeff.invRatio,-100.,100.);
 double history=loadHistory;
 double nextI=(source+coeff.coilG*coilI-history*coeff.invG)*coeff.invDen;
 capV=(nextI+history)*coeff.invG;indI+=coeff.indStep*capV;coilI=nextI;fb=source-coeff.rs*coilI;return fb/std::sqrt(2.*q.load*100.);
 }
 double processPreampOnly(double x){double v=x*(.775*std::sqrt(2.)/std::pow(10.,-14./20.));
 v=preamp.process(v,powerCircuit(selectedPower()).B*.65,dt,controls.preamp,controls.gain,controls.bass,controls.mid,controls.treble,controls.joseEra,controls.joseExtraStage,controls.joseGain2,controls.joseArchitecture,controls.preamp==PreampModel::Jose?controls.joseBias:controls.coldBias,controls.preamp==PreampModel::Jose?controls.joseBypass:controls.coldBypass,controls.rectoVoice,controls.evhVoice,controls.eco,controls.joseSAT?1.:controls.volume,controls.joseArchitecture==JoseArchitecture::JosePostEQ,controls.brightMode,controls.presence);
 if(controls.preamp!=PreampModel::Jose)v*=controls.volume;
 return v/30.;}
 double process(double x){if(!std::isfinite(x))x=0;double out=0;for(int j=1;j<=oversampling;++j){double y=step(prev+(x-prev)*j/oversampling);if(!std::isfinite(y)){reset();return 0;}out+=y;}prev=x;double y=out/oversampling;return std::clamp(y-lp(y,outputDC,8.,1./fs),-1.,1.);}
};
}
