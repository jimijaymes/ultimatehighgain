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
 double capacitor=0;
 double process(double tap,double dt,double rf,double rg,double rp,double c){
  const double rth=rf*rg/(rf+rg), vth=tap*rg/(rf+rg);
  const double a=dt/c, i=(vth-capacitor)/(rth+rp+a);
  capacitor+=a*i;
  return vth-rth*i;
 }
};
struct Circuit {double B=450, screen=440, primary=1700, load=8, pairs=2, reservoir=50e-6, supplyR=65, screenR=1000, feedbackR=100000, feedbackGround=4700, presenceR=25000, presenceC=100e-9, feedbackTap=4, transformerL=20, leakage=.001, fluxLimit=.65;};
struct Controls {double presence=.5, gain=.5, master=.25, bass=.5, mid=.5, treble=.5; bool spongy=false,tubeRect=false; Tube tube=Tube::EL34; PreampModel preamp=PreampModel::Marshall; JoseEra joseEra=JoseEra::Zener20; bool joseExtraStage=true; double joseGain2=.7; JoseArchitecture joseArchitecture=JoseArchitecture::Legacy; ColdBias joseBias=ColdBias::Native; CathodeBypass joseBypass=CathodeBypass::Native; ColdBias coldBias=ColdBias::Native; CathodeBypass coldBypass=CathodeBypass::Native; RectoVoice rectoVoice=RectoVoice::EarlyRed; EVHVoice evhVoice=EVHVoice::Blue;};
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
 Circuit circuit; Controls controls; double fs=48000,dt=1./192000; Preamp preamp; PhaseInverter pi; GridCoupling grid1,grid2; double magI=0; PresenceNetwork presenceNet; double supply=450, screen=440,bias=-40, fb=0,fbLow=0,flux=0,leak=0,capV=0,indI=0,coilI=0, bassZ=0,trebleZ=0, prev=0,gridBlock=0; double peakPlate=0; Tube active=Tube::EL34; bool ready=false;
 void prepare(double rate){fs=rate;dt=1/(4*fs);reset();}
 void reset(){preamp={};pi={};grid1={};grid2={};magI=0;presenceNet={};supply=circuit.B;screen=circuit.screen;fb=fbLow=flux=leak=capV=indI=coilI=bassZ=trebleZ=prev=gridBlock=peakPlate=0;active=controls.tube;ready=false;}
 double biasFor(TubeData t,double v,double s){double lo=-200,hi=0;double target=std::min(t.idle,.65*t.rating/std::max(1.,v));for(int i=0;i<32;++i){double m=(lo+hi)*.5;if(plate(t,v,m,s)>target)hi=m;else lo=m;}return (lo+hi)*.5;}
 double step(double x){auto &q=circuit;auto t=data(controls.tube);double target=q.B*(controls.spongy?.85:1.)-(controls.tubeRect?20:0);if(!ready||active!=controls.tube){bias=biasFor(t,target,q.screen*target/q.B);grid1.charge=-bias;grid2.charge=-bias;active=controls.tube;ready=true;}
 // 0 dBu = .775 Vrms; -14 dBFS sine = .1995 peak.
 double v=x*(.775*std::sqrt(2.)/std::pow(10.,-14./20.));
 v=preamp.process(v,supply*.65,dt,controls.preamp,controls.gain,controls.bass,controls.mid,controls.treble,controls.joseEra,controls.joseExtraStage,controls.joseGain2,controls.joseArchitecture,controls.preamp==PreampModel::Jose?controls.joseBias:controls.coldBias,controls.preamp==PreampModel::Jose?controls.joseBypass:controls.coldBypass,controls.rectoVoice,controls.evhVoice)*controls.master;
 // Feedback uses previous oversampled secondary voltage; bounded explicit loop.
 double feedback=presenceNet.process(fb*std::sqrt(q.feedbackTap/q.load),dt,q.feedbackR,q.feedbackGround,q.presenceR*(1-clamp(controls.presence,0.,1.)),q.presenceC);
 double d1,d2;pi.process(clamp(v,-30.,30.),feedback,supply*.72,dt,d1,d2);
 double g1=grid1.process(d1,bias,dt),g2=grid2.process(d2,bias,dt);
 double ratio=std::sqrt(q.primary/q.load);
 // Solve plate/screen current against the same instantaneous reflected voltage.
 struct Branch {double ip,is;};
 auto branch=[&](double vp,double vg){double lo=0,hi=screen;for(int k=0;k<6;k++){double vs=(lo+hi)*.5;if(vs+screenCurrent(t,vp,vg,vs)*q.screenR>screen)hi=vs;else lo=vs;}double vs=(lo+hi)*.5;return Branch{plate(t,vp,vg,vs),screenCurrent(t,vp,vg,vs)};};
 // Implicit quasi-static primary load line; speaker dynamic state remains explicit.
 double lo=-2*supply,hi=2*supply;
 for(int k=0;k<10;k++){double v=(lo+hi)*.5;auto a=branch(supply-v*.5,g1),b=branch(supply+v*.5,g2);double predicted=((a.ip-b.ip)*q.pairs*.5-magI)*q.primary;if(predicted>v)lo=v;else hi=v;}
 double primaryV=(lo+hi)*.5;auto a1=branch(supply-primaryV*.5,g1),a2=branch(supply+primaryV*.5,g2);double i1=a1.ip,i2=a2.ip;
 peakPlate=std::max(peakPlate,std::max(i1,i2)*supply);
 double screens=(a1.is+a2.is)*q.pairs;double current=(i1+i2)*q.pairs+screens+.008;
 double r=q.supplyR+(controls.tubeRect?65:0);double recharge=std::max(0.,(target-supply)/r);supply=clamp(supply+dt*(recharge-current)/q.reservoir,target*.45,target);
 screen=lp(clamp(supply-(q.B-q.screen)-screens*100,30.,supply),screen,35,dt);
 // Reduced-order transformer: flux state and smooth core compression.
 double emf=primaryV;magI+=dt*(emf-magI*100)/q.transformerL;flux+=dt*(emf-flux*35);double sat=1/std::sqrt(1+std::pow(flux/q.fluxLimit,2));double source=clamp(emf*sat/ratio,-100.,100.);
 // Passive series coil and parallel motional RLC, backward Euler companion solution.
 double R=q.load*.75, L=.00045+q.leakage/(ratio*ratio), Rm=q.load*4,Lm=.018,Cm=1/(std::pow(6.283185307*95,2)*Lm);
 double G=1/Rm+dt/Lm+Cm/dt;double history=Cm/dt*capV-indI;double rs=q.load*.12;double nextI=(source+L/dt*coilI-history/G)/(R+rs+L/dt+1/G);capV=(nextI+history)/G;indI+=dt*capV/Lm;coilI=nextI;fb=source-rs*coilI;return fb/40.;
 }
 double process(double x){double out=0;for(int j=1;j<=4;++j)out+=step(prev+(x-prev)*j/4.);prev=x;return out*.25;}
};
}
