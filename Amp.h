#pragma once
#include <cmath>
#include <algorithm>
#include "Circuits.h"
#include "Tubes.h"
#include "Preamp.h"
namespace uhg {
inline double clamp(double x,double a,double b){return std::clamp(x,a,b);}
inline double lp(double x,double &z,double f,double dt){z+=(1-std::exp(-6.283185307*f*dt))*(x-z);return z;}
// Reduced-order RC feedback divider, with series presence rheostat and capacitor.
struct PresenceNetwork {
 double depthLow=0; double capacitor=0; double keyDt=0,keyRf=0,keyRg=0,keyRp=0,keyC=0,rth=0,a=0,den=0;
 double process(double tap,double dt,double rf,double rg,double rp,double c,double depth=0){
  depthLow+=(1-std::exp(-6.283185307*120.*dt))*(tap-depthLow);tap-=.75*std::clamp(depth,0.,1.)*depthLow;
  if(dt!=keyDt||rf!=keyRf||rg!=keyRg||rp!=keyRp||c!=keyC){keyDt=dt;keyRf=rf;keyRg=rg;keyRp=rp;keyC=c;rth=rf*rg/(rf+rg);a=dt/c;den=rth+rp+a;}
  const double vth=tap*rg/(rf+rg), i=(vth-capacitor)/den;
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
inline bool nativeDepth(PowerModel m,EVHVoice voice){return m==PowerModel::VHT||m==PowerModel::Peavey||m==PowerModel::SLO||(m==PowerModel::EVH&&(voice==EVHVoice::BlueEL34||voice==EVHVoice::BlueStealth||voice==EVHVoice::RedStealth));}
inline Tube nativeTube(PowerModel m){switch(m){case PowerModel::VHT:return Tube::KT88;case PowerModel::AC30:case PowerModel::MiniRecto:return Tube::EL84;case PowerModel::Peavey:case PowerModel::RectoVintage:case PowerModel::RectoModern:case PowerModel::EVH:case PowerModel::SLO:return Tube::L6;default:return Tube::EL34;}}
inline PhaseInverterSpec inverterSpec(PowerModel m){PhaseInverterSpec p;switch(m){case PowerModel::RectoVintage:case PowerModel::RectoModern:p.coupling=47e-9;p.gridStop=1500;break;case PowerModel::AC30:p.plate1=100000;p.plate2=100000;p.tail=48200;p.biasR=1200;p.coupling=150e-9;p.gridStop=1500;break;case PowerModel::MiniRecto:case PowerModel::Peavey:case PowerModel::EVH:p.coupling=47e-9;p.gridStop=1500;break;case PowerModel::VHT:case PowerModel::SLO:p.coupling=100e-9;p.gridStop=2200;break;default:break;}return p;}
struct Controls {bool pittHi=true,pittBoost=false,pittEdge=false,pittShift=false;bool disableFeedback=false;bool inverterOnly=false;bool powerOnly=false;bool benchResistive=false;bool benchDirect=false;bool eco=false;bool autoTube=true;bool linkPower=true; PowerModel power=PowerModel::Marshall;double resonance=0.,presence=.5, gain=.5, master=.25, bass=.5, mid=.5, treble=.5; bool spongy=false,tubeRect=false; Tube tube=Tube::EL34; PreampModel preamp=PreampModel::Marshall; JoseEra joseEra=JoseEra::Zener20; bool joseExtraStage=true; double joseGain2=.7; double volume=1.; bool josePostEQ=false; bool joseSAT=false; int brightMode=1; JoseArchitecture joseArchitecture=JoseArchitecture::Legacy; ColdBias joseBias=ColdBias::Native; CathodeBypass joseBypass=CathodeBypass::Native; ColdBias coldBias=ColdBias::Native; CathodeBypass coldBypass=CathodeBypass::Native; RectoVoice rectoVoice=RectoVoice::EarlyRed; EVHVoice evhVoice=EVHVoice::Blue;};
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
 struct Prepared {double dt=0;PowerModel model=PowerModel::Marshall;bool tubeRect=false,spongy=false,valid=false;double target=0;double invRatio,magStep,invFlux,capG,invG,coilG,invDen,indStep,rs,supplyStep,invSupplyR,screenAlpha,feedbackScale,reflectedZ,screenDrop,cathDen;PhaseInverterSpec ps;double cutA=0,cutDen=0,modernAlpha=0,outputAlpha=0;double presence=-1,modernGain=0,presenceR=0;Tube tube=Tube::EL34;bool tubeValid=false;TubeData tubeData{};double share=0,ref=0,u0=0,plateRef=0,plateSlopeDen=0;}; Prepared coeff{};
 Circuit circuit; Controls controls; int oversampling=1; double fs=48000,dt=1./48000; Preamp preamp; PhaseInverter pi; GridCoupling grid1,grid2; double magI=0; PresenceNetwork presenceNet; double supply=450, screen=440,bias=-40, fb=0,fbLow=0,flux=0,leak=0,capV=0,indI=0,coilI=0, bassZ=0,trebleZ=0, prev=0,gridBlock=0; double sagCurrentSum=0, sagScreenSum=0; int sagCount=0; double debugPreamp=0; double outputDC=0; double screenEstimate1=440,screenEstimate2=440; double peakPlate=0,cathodeV=0,cutState=0,modernState=0; Tube active=Tube::EL34; bool ready=false; PowerModel activePower=PowerModel::Marshall;
 PowerModel selectedPower() const {if(!controls.linkPower)return controls.power; switch(controls.preamp){case PreampModel::Peavey5150:return PowerModel::Peavey;case PreampModel::DualRectifier:return rectoModern(controls.rectoVoice)?PowerModel::RectoModern:PowerModel::RectoVintage;case PreampModel::VHT:return PowerModel::VHT;case PreampModel::EVH5150III:return PowerModel::EVH;default:return PowerModel::Marshall;}}

 void prepare(double rate){fs=rate;dt=1/(oversampling*fs);reset();}
 void reset(){sagCurrentSum=sagScreenSum=0;sagCount=0;preamp={};pi={};grid1={};grid2={};magI=0;presenceNet={};supply=circuit.B;screen=circuit.screen;fb=fbLow=flux=leak=capV=indI=coilI=bassZ=trebleZ=prev=gridBlock=peakPlate=0;active=controls.tube;outputDC=cathodeV=cutState=modernState=0;ready=false;economyLow=economyHigh=economyCap=economyInd=economyCoil=0;}
 double biasFor(TubeData t,double v,double s){double lo=-200,hi=0;double target=std::min(t.idle,.65*t.rating/std::max(1.,v));for(int i=0;i<32;++i){double m=(lo+hi)*.5;if(plate(t,v,m,s)>target)hi=m;else lo=m;}return (lo+hi)*.5;}
 double preampSupply() const {
 // Independent nominal preamp rail; preserve native family voltage anchors.
 PowerModel native=PowerModel::Marshall;
 switch(controls.preamp){
 case PreampModel::Peavey5150:native=PowerModel::Peavey;break;
 case PreampModel::RectoOrange:case PreampModel::RectoRed:case PreampModel::RectoMulti:case PreampModel::DualRectifier:native=PowerModel::RectoVintage;break;
 case PreampModel::VHT:native=PowerModel::VHT;break;
 case PreampModel::EVHBlue:case PreampModel::EVHRed:case PreampModel::EVH5150III:native=PowerModel::EVH;break;
 default:break;
 }
 return (powerCircuit(native).B*(controls.spongy?.85:1.)-(controls.tubeRect?20.:0.))*(isRecto(controls.preamp)?.90:.65);
 }

 double step(double x){auto m=selectedPower();if(m!=activePower){circuit=powerCircuit(m);activePower=m;reset();}auto &q=circuit;
 const bool modelChanged=!coeff.valid||coeff.model!=m;
 const bool rateChanged=!coeff.valid||coeff.dt!=dt;
 const bool rectChanged=!coeff.valid||coeff.tubeRect!=controls.tubeRect;
 const bool supplyChanged=modelChanged||rectChanged||coeff.spongy!=controls.spongy;
 if(modelChanged){
 coeff.invRatio=std::sqrt(q.load/q.primary);coeff.invFlux=1/q.fluxLimit;coeff.rs=q.load*.12;
 coeff.screenDrop=q.B-q.screen;coeff.ps=inverterSpec(m);coeff.feedbackScale=std::sqrt(q.feedbackTap/q.load);
 }
 if(modelChanged||rateChanged){
 coeff.magStep=dt/q.transformerL;
 constexpr double Lm=.018,omega=6.283185307*95.;
 coeff.capG=1/(omega*omega*Lm*dt);coeff.indStep=dt/Lm;
 coeff.invG=1/(1/(q.load*5.45)+coeff.indStep+coeff.capG);
 coeff.coilG=(.0005+q.leakage*q.load/q.primary)/dt;
 coeff.invDen=1/(q.load*.8+coeff.rs+coeff.coilG+coeff.invG);
 coeff.reflectedZ=1/(coeff.invDen*coeff.invRatio*coeff.invRatio);
 coeff.supplyStep=dt/q.reservoir;
 }
 if(rateChanged){coeff.cathDen=1+dt/(47.*250e-6);coeff.screenAlpha=-std::expm1(-6.283185307*35*dt);coeff.cutA=dt/4.7e-9;coeff.modernAlpha=1-std::exp(-6.283185307*2200*dt);coeff.outputAlpha=1-std::exp(-6.283185307*8.*(1./fs));}
 if(modelChanged||rectChanged)coeff.invSupplyR=1/(q.supplyR+(controls.tubeRect?65:0));
 if(supplyChanged)coeff.target=q.B*(controls.spongy?.85:1.)-(controls.tubeRect?20:0);
 if(modelChanged||rateChanged||coeff.presence!=controls.presence){coeff.presence=controls.presence;coeff.presenceR=q.presenceR*(1-clamp(controls.presence,0.,1.));coeff.cutDen=1000.+250000.*(1-controls.presence)+100000.+coeff.cutA;coeff.modernGain=.65*(1-controls.presence);}
 coeff.valid=true;coeff.dt=dt;coeff.model=m;coeff.tubeRect=controls.tubeRect;coeff.spongy=controls.spongy;
 auto chosen=controls.autoTube?(m==PowerModel::EVH&&controls.evhVoice==EVHVoice::BlueEL34?Tube::EL34:nativeTube(m)):controls.tube;
 if(!coeff.tubeValid||coeff.tube!=chosen){coeff.tubeValid=true;coeff.tube=chosen;coeff.tubeData=data(chosen);const auto& t=coeff.tubeData;coeff.share=t.is/t.ia;coeff.ref=1+2*valveExp.get(t.va/t.knee);coeff.u0=1.5*t.ia/t.gm;coeff.plateRef=1-valveExp.get(t.va/t.knee);coeff.plateSlopeDen=t.rp*t.ia;}
 const auto& t=coeff.tubeData;const auto& ps=coeff.ps;bool cathodeBias=m==PowerModel::AC30;double target=coeff.target;if(!ready||active!=chosen){auto biasData=t;if(m==PowerModel::VHT&&chosen==Tube::EL34)biasData.idle=.035;bias=biasFor(biasData,target,q.screen*target/q.B);cathodeV=cathodeBias?-bias:0;grid1.charge=cathodeBias?0:-bias;grid2.charge=grid1.charge;active=chosen;ready=true;}
 // 0 dBu = .775 Vrms; -14 dBFS sine = .1995 peak.
 double v=x*5.493063;
 if(!controls.powerOnly){
 v=preamp.process(v,preampSupply(),dt,controls.preamp,controls.gain,controls.bass,controls.mid,controls.treble,controls.joseEra,controls.joseExtraStage,controls.joseGain2,controls.joseArchitecture,controls.preamp==PreampModel::Jose?controls.joseBias:controls.coldBias,controls.preamp==PreampModel::Jose?controls.joseBypass:controls.coldBypass,controls.rectoVoice,controls.evhVoice,controls.eco,controls.joseSAT?1.:controls.volume,controls.joseArchitecture==JoseArchitecture::JosePostEQ,controls.brightMode,controls.presence,controls.pittHi,controls.pittBoost,controls.pittEdge,controls.pittShift);
 if(controls.preamp!=PreampModel::Jose)v*=controls.volume;
 } if(!controls.benchDirect)v*=(controls.preamp==PreampModel::Marshall||(controls.preamp==PreampModel::Jose&&controls.joseArchitecture==JoseArchitecture::Stock))?audioPot(controls.master):controls.master*controls.master;
 
 debugPreamp=std::isfinite(v)?std::clamp(v/30.,-1.,1.):0.;
 // Feedback uses previous oversampled secondary voltage; bounded explicit loop.
 if(controls.eco&&!controls.benchDirect&&!controls.inverterOnly)return economyClosedLoop(v,m,q,ps);
 const bool closed=!controls.disableFeedback&&!controls.inverterOnly&&m!=PowerModel::RectoModern&&m!=PowerModel::AC30;
 if(!closed)return detailedTrial(v,m,q,t,ps,cathodeBias,target,0.);
 const auto oldPI=pi;const auto oldPresence=presenceNet;const auto oldGrid1=grid1,oldGrid2=grid2;
 const double oldSupply=supply,oldScreen=screen,oldCath=cathodeV,oldMag=magI,oldFlux=flux,oldCap=capV,oldInd=indI,oldCoil=coilI,oldPeak=peakPlate,oldCut=cutState,oldModern=modernState;
 auto trial=[&](double terminal){pi=oldPI;presenceNet=oldPresence;grid1=oldGrid1;grid2=oldGrid2;supply=oldSupply;screen=oldScreen;cathodeV=oldCath;magI=oldMag;flux=oldFlux;capV=oldCap;indI=oldInd;coilI=oldCoil;peakPlate=oldPeak;cutState=oldCut;modernState=oldModern;return detailedTrial(v,m,q,t,ps,cathodeBias,target,terminal);};
 double lo=-512.,hi=512.,feedbackEstimate=std::clamp(fb,lo,hi),fl=0.,fh=0.;bool haveL=false,haveH=false;
 for(int k=0;k<40;++k){double y=trial(feedbackEstimate),f=fb-feedbackEstimate;if(std::abs(f)<.0001)return y;if(f>0){lo=feedbackEstimate;fl=f;haveL=true;}else{hi=feedbackEstimate;fh=f;haveH=true;}double next=haveL&&haveH?lo+(hi-lo)*fl/(fl-fh):feedbackEstimate+f*.25;if(k%4==3||!(next>lo&&next<hi))next=(lo+hi)*.5;feedbackEstimate=next;}return trial(feedbackEstimate);
 }
 double detailedTrial(double v,PowerModel m,const Circuit& q,const TubeData& t,const PhaseInverterSpec& ps,bool cathodeBias,double target,double terminal){
 double feedback=(controls.disableFeedback||activePower==PowerModel::RectoModern||activePower==PowerModel::AC30)?0.:presenceNet.process(terminal*coeff.feedbackScale,dt,q.feedbackR,q.feedbackGround,coeff.presenceR,q.presenceC,nativeDepth(m,controls.evhVoice)?controls.resonance:0.);
 double d1,d2;if(controls.inverterOnly){pi.process(v,0.,supply*.72,dt,d1,d2,ps,controls.eco);return (d1-d2)/100.*0.251188643150958;}pi.process(v,feedback,supply*.72,dt,d1,d2,ps,controls.eco);
 if(m==PowerModel::AC30){double diff=d1-d2;double sourceR=100000.;double a=coeff.cutA;double i=(diff-cutState)/coeff.cutDen;cutState+=a*i;d1-=i*sourceR*.5;d2+=i*sourceR*.5;}
 if(m==PowerModel::RectoModern&&(controls.powerOnly||!isRecto(controls.preamp)||!rectoModern(controls.rectoVoice))){double diff=d1-d2;modernState+=coeff.modernAlpha*(diff-modernState);double high=diff-modernState;double atten=coeff.modernGain*high;d1-=atten*.5;d2+=atten*.5;}
 
 double g1=grid1.process(d1,cathodeBias?0:bias,dt,ps,cathodeV)-cathodeV,g2=grid2.process(d2,cathodeBias?0:bias,dt,ps,cathodeV)-cathodeV;

 // Solve plate/screen current against the same instantaneous reflected voltage.
 struct Branch {double ip,is;};
 const double screenAvailable=std::max(0.,screen-cathodeV);
 const double gridBase1=coeff.u0+g1-t.vg,gridBase2=coeff.u0+g2-t.vg;
 auto branch=[&](double vp,double vg){const double plateV=vp-cathodeV;const double p=std::max(0.,plateV);const double expPlate=valveExp.get(p/t.knee);const double collect=1+2*expPlate;const double gridBase=vg==g1?gridBase1:gridBase2;auto emit=[&](double vs){double u=std::max(0.,gridBase+(vs-t.vs)/t.mu);return t.ia*(u/coeff.u0)*std::sqrt(u/coeff.u0);};double lastEmission=0.;auto screenAt=[&](double vs){lastEmission=emit(vs);return lastEmission*coeff.share*collect/coeff.ref*std::max(0.,vs)/t.vs;};double low=0,high=screen,fl=-screen,fh=screenAt(screenAvailable)*q.screenR;double vs=screen,lastScreenCurrent=0;for(int j=0;j<(controls.eco?3:6);++j){vs=(fh>fl)?low+(high-low)*(-fl)/(fh-fl):low;lastScreenCurrent=screenAt(std::max(0.,vs-cathodeV));double f=vs+lastScreenCurrent*q.screenR-screen;if(std::abs(f)<.05)break;if(f>0){high=vs;fh=f;fl*=.5;}else{low=vs;fl=f;fh*=.5;}}const double knee=(1-expPlate)/coeff.plateRef;const double slope=std::max(0.,1+(p-t.va)/coeff.plateSlopeDen);return Branch{lastEmission*knee*slope,lastScreenCurrent};};
 // Implicit quasi-static primary load line; speaker dynamic state remains explicit.
 const double loadHistory=coeff.capG*capV-indI;
 const double reflectedHistory=controls.benchResistive?0.:(loadHistory*coeff.invG-coeff.coilG*coilI)/coeff.invRatio;
 const double loadZ=controls.benchResistive?q.primary:coeff.reflectedZ;
 const double magDen=1+100*coeff.magStep;
 const double reflectedZ=loadZ/(1+loadZ*coeff.magStep/magDen);
 double lo=-2*supply,hi=2*supply;
 Branch cachedA{},cachedB{};double cachedV=0;bool cached=false;
 auto residual=[&](double v){auto a=branch(supply-v*.5,g1),b=branch(supply+v*.5,g2);cachedA=a;cachedB=b;cachedV=v;cached=true;return ((a.ip-b.ip)*q.pairs*.5-magI/magDen+reflectedHistory/loadZ)*reflectedZ-v;};
 double primaryV=0;
 for(int k=0;k<24;++k){double v=(lo+hi)*.5;double f=residual(v);if(std::abs(f)<.00001){lo=hi=v;break;}if(f>0)lo=v;else hi=v;}
 primaryV=(lo+hi)*.5;const bool reuse=cached&&cachedV==primaryV;auto a1=reuse?cachedA:branch(supply-primaryV*.5,g1),a2=reuse?cachedB:branch(supply+primaryV*.5,g2);double i1=a1.ip,i2=a2.ip;
 peakPlate=std::max(peakPlate,std::max(i1,i2)*supply);
 double screens=(a1.is+a2.is)*q.pairs;if(cathodeBias){double ck=250e-6;cathodeV=clamp((cathodeV+dt*((i1+i2)*q.pairs+screens)/ck)/coeff.cathDen,0.,80.);}double current=(i1+i2)*q.pairs+screens+.008;
 // Backward Euler reservoir update avoids stiff explicit supply oscillation.
 supply=clamp((supply+coeff.supplyStep*(target*coeff.invSupplyR-current))/(1+coeff.supplyStep*coeff.invSupplyR),target*.45,target);
 screen+=coeff.screenAlpha*(clamp(supply-coeff.screenDrop-screens*100,30.,supply)-screen);
 double emf=primaryV;magI=(magI+coeff.magStep*emf)/magDen;flux+=dt*(emf-flux*35);
 // Use the same transformer voltage in the load solve and state update.
 double source=emf*coeff.invRatio;
 if(controls.benchResistive){fb=source;return 0.251188643150958*fb/std::sqrt(2.*q.load*100.);}
 double history=loadHistory;
 const double oldCoilI=coilI;
 double nextI=(source+coeff.coilG*coilI-history*coeff.invG)*coeff.invDen;
 capV=(nextI+history)*coeff.invG;indI+=coeff.indStep*capV;coilI=nextI;fb=q.load*.8*coilI+capV+(.0005/dt)*(coilI-oldCoilI);return 0.251188643150958*fb/std::sqrt(2.*q.load*100.);
 
 }

 double economyLow=0,economyHigh=0,economyCap=0,economyInd=0,economyCoil=0;
 double economyClosedLoop(double input,PowerModel m,const Circuit& q,const PhaseInverterSpec& ps){
 const bool closed=!controls.disableFeedback&&m!=PowerModel::RectoModern&&m!=PowerModel::AC30;
 const auto oldPI=pi;const auto oldPresence=presenceNet;
 const double low=economyLow,cap=economyCap,ind=economyInd,coil=economyCoil,cut=cutState,modern=modernState;
 auto trial=[&](double terminal){pi=oldPI;presenceNet=oldPresence;economyLow=low;economyCap=cap;economyInd=ind;economyCoil=coil;cutState=cut;modernState=modern;
 double feedback=closed?presenceNet.process(terminal*coeff.feedbackScale,dt,q.feedbackR,q.feedbackGround,coeff.presenceR,q.presenceC,nativeDepth(m,controls.evhVoice)?controls.resonance:0.):0.;
 double a,b;pi.process(input,feedback,supply*.72,dt,a,b,ps,true);
 if(m==PowerModel::AC30){double i=(a-b-cutState)/coeff.cutDen;cutState+=coeff.cutA*i;a-=i*50000.;b+=i*50000.;}
 if(m==PowerModel::RectoModern&&(controls.powerOnly||!isRecto(controls.preamp)||!rectoModern(controls.rectoVoice))){double diff=a-b;modernState+=coeff.modernAlpha*(diff-modernState);double atten=coeff.modernGain*(diff-modernState);a-=atten*.5;b+=atten*.5;}
 return economyPower(a-b,m,q);};
 if(!closed)return trial(0.);
 // Implicit feedback: evaluate from identical old states, commit only the accepted trial.
 double lo=-4096.,hi=4096.,x=std::clamp(fb,lo,hi),y=0.,fl=0.,fh=0.;bool haveL=false,haveH=false;
 for(int k=0;k<32;++k){y=trial(x);double f=fb-x;if(std::abs(f)<1e-5)return y;if(f>0){lo=x;fl=f;haveL=true;}else{hi=x;fh=f;haveH=true;}double next=haveL&&haveH?lo+(hi-lo)*fl/(fl-fh):std::clamp(x+f*.25,lo,hi);if(k%4==3||!(next>lo&&next<hi))next=(lo+hi)*.5;x=next;}return trial(x);
 }
 double economyPower(double v,PowerModel m,const Circuit& q){
 // Linear Thevenin source into passive speaker impedance; no nonlinear tube solver.
 // Open-loop source impedance; closed-loop damping emerges from the PI feedback.
 const double damping=1.1;
 const double rs=q.load*damping;
 const double low=lp(v,economyLow,m==PowerModel::AC30?65.:35.,dt);
 const double ac=v-low;
 // Approximate class-AB full drive: differential grid peak ~= twice idle bias.
 const double watts=m==PowerModel::VHT?(active==Tube::EL34?100.:120.):m==PowerModel::AC30?30.:m==PowerModel::MiniRecto?25.:m==PowerModel::AC50?50.:100.;
 const double gridPeak=2*std::max(5.,std::abs(bias));
 const double drive=ac*(std::sqrt(2*q.load*watts)/gridPeak)*(1+damping);
 constexpr double L=.018,w=6.283185307*95.;
 const double gc=1/(w*w*L*dt), gl=dt/L, z=1/(1/(q.load*5.45)+gc+gl);
 const double hist=gc*economyCap-economyInd;
 const double series=.0005/dt;
 const double current=(drive+series*economyCoil-z*hist)/(rs+q.load*.8+series+z);
 economyCap=z*(current+hist);economyInd+=gl*economyCap;economyCoil=current;
 // Physical speaker terminal voltage, before digital monitoring scaling.
 fb=drive-rs*current;
 return 0.251188643150958*fb/std::sqrt(2.*q.load*100.);
 }
 double preampOnlyStep(double x){double v=x*(.775*std::sqrt(2.)/std::pow(10.,-14./20.));
 v=preamp.process(v,preampSupply(),dt,controls.preamp,controls.gain,controls.bass,controls.mid,controls.treble,controls.joseEra,controls.joseExtraStage,controls.joseGain2,controls.joseArchitecture,controls.preamp==PreampModel::Jose?controls.joseBias:controls.coldBias,controls.preamp==PreampModel::Jose?controls.joseBypass:controls.coldBypass,controls.rectoVoice,controls.evhVoice,controls.eco,controls.joseSAT?1.:controls.volume,controls.joseArchitecture==JoseArchitecture::JosePostEQ,controls.brightMode,controls.presence,controls.pittHi,controls.pittBoost,controls.pittEdge,controls.pittShift);
 if(controls.preamp!=PreampModel::Jose)v*=controls.volume;
 return 0.251188643150958*v/30.;}
 double processPreampOnly(double x){if(!std::isfinite(x))x=0;double out=0;for(int j=1;j<=oversampling;++j)out+=preampOnlyStep(prev+(x-prev)*j/oversampling);prev=x;return out/oversampling;}
 double process(double x){if(!std::isfinite(x))x=0;double out=0;for(int j=1;j<=oversampling;++j){double y=step(prev+(x-prev)*j/oversampling);if(!std::isfinite(y)){reset();return 0;}out+=y;}prev=x;double y=out/oversampling;outputDC+=coeff.outputAlpha*(y-outputDC);return y-outputDC;}
};
}
