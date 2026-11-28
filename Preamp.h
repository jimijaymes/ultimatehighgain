#pragma once
#include "Circuits.h"
#include "JoseClip.h"
#include <array>
namespace uhg {
enum class PreampModel { Marshall, Jose, Peavey5150, RectoOrange, RectoRed, RectoMulti, VHT, EVHBlue, EVHRed, DualRectifier, EVH5150III };
enum class EVHVoice { Blue, Red, BlueEL34, BlueStealth, RedStealth };
inline PreampModel resolveEVH(PreampModel m,EVHVoice v){return m==PreampModel::EVH5150III?((v==EVHVoice::Red||v==EVHVoice::RedStealth)?PreampModel::EVHRed:PreampModel::EVHBlue):m;}
enum class RectoVoice { EarlyOrange, EarlyRed, MultiWattModern, EarlyOrangeModern, EarlyRedVintage, MultiOrangeVintage, MultiOrangeModern, MultiRedVintage, RoadKing3Vintage, RoadKing3Modern, RoadKing4Vintage, RoadKing4Modern };
inline bool rectoModern(RectoVoice v){return v==RectoVoice::EarlyRed||v==RectoVoice::MultiWattModern||v==RectoVoice::EarlyOrangeModern||v==RectoVoice::MultiOrangeModern||v==RectoVoice::RoadKing3Modern||v==RectoVoice::RoadKing4Modern;}
inline bool isRecto(PreampModel m){return m==PreampModel::RectoOrange||m==PreampModel::RectoRed||m==PreampModel::RectoMulti||m==PreampModel::DualRectifier;}
inline PreampModel resolveRecto(PreampModel m, RectoVoice v){if(m!=PreampModel::DualRectifier)return m;return (v==RectoVoice::EarlyOrange||v==RectoVoice::EarlyOrangeModern)?PreampModel::RectoOrange:(v==RectoVoice::EarlyRed||v==RectoVoice::EarlyRedVintage)?PreampModel::RectoRed:PreampModel::RectoMulti;}
enum class JoseArchitecture { Legacy, Classic, HBE, Snorkler, Stock, JosePostEQ, Molecular };
enum class ColdBias { Native, Warm, Modified, Marshall, Cold, Soldano, Custom47k };
enum class CathodeBypass { Native, Off, Small, Medium, Large };
struct StageSpec { double rp,rk,ck,cc,rg,atten,peakC; };
struct PreampSpec { std::array<StageSpec,5> stages; int count; int gainAfter; double slope,ct,cb,cm,clip; };
// Marshall values are reference-based; other configurations are provisional engineering seeds, not schematic transcriptions.
inline PreampSpec preampSpec(PreampModel m){
 const StageSpec a{100e3,2700,.68e-6,22e-9,1e6,1,0}, cold{100e3,10000,0,22e-9,470e3,.10,470e-12}, hot{100e3,820,.68e-6,22e-9,470e3,.12,0}, warm{100e3,1500,1e-6,22e-9,470e3,.12,0};
 PreampSpec p{{a,cold,hot,warm,hot},3,0,33000,470e-12,22e-9,22e-9,0};
 switch(m){case PreampModel::Marshall:break;
 case PreampModel::Jose:p.count=4;p.stages={a,hot,cold,hot,warm};p.clip=12;break;
 case PreampModel::Peavey5150:p.count=5;p.stages={a,warm,cold,hot,warm};p.stages[2].rk=39000;p.slope=47000;break;
 case PreampModel::DualRectifier:case PreampModel::RectoOrange:case PreampModel::RectoRed:case PreampModel::RectoMulti:p.count=4;p.stages={a,warm,cold,hot,warm};p.stages[0]={220e3,1800,1e-6,20e-9,1e6,1,0};p.stages[1]={100e3,1800,1e-6,20e-9,1e6,.64,2.082e-9};p.stages[2]={100e3,39000,0,20e-9,1e6,.68,0};p.stages[3]={220e3,1800,1e-6,20e-9,330e3,.60,0};p.slope=47000;p.ct=500e-12;p.cb=p.cm=20e-9;break;
 case PreampModel::VHT:p.count=4;p.stages={a,hot,cold,warm,hot};p.stages[1].cc=4.7e-9;p.stages[2].cc=10e-9;break;
 case PreampModel::EVH5150III:case PreampModel::EVHBlue:p.count=4;p.stages={a,warm,cold,hot,warm};p.stages[1].cc=10e-9;break;
 case PreampModel::EVHRed:p.count=5;p.stages={a,warm,cold,hot,warm};p.stages[1].cc=10e-9;p.stages[3].atten=.18;break;}
 return p;
}
inline PreampSpec joseSpec(JoseArchitecture arch, ColdBias bias, CathodeBypass bypass){
 auto p=preampSpec(PreampModel::Jose); int cold=2;
 if(arch==JoseArchitecture::Classic||arch==JoseArchitecture::JosePostEQ||arch==JoseArchitecture::Stock||arch==JoseArchitecture::Molecular){p=preampSpec(PreampModel::Marshall);cold=1;}
 if(arch==JoseArchitecture::Snorkler){p.stages[2].rk=1500;p.stages[2].ck=1e-6;p.stages[3].rk=1500;p.stages[3].ck=1e-6;p.slope=100000;for(int i=1;i<4;++i)p.stages[i].cc=47e-9;}
 if(arch==JoseArchitecture::Molecular){p.stages[0].ck=.68e-6;p.stages[1].peakC=500e-12;p.ct=500e-12;} // provisional layout-inspired seed
 const double r[]={0,1500,4700,10000,22000,39000,47000}; const double c[]={0,0,.68e-6,1e-6,22e-6};
 if(bias!=ColdBias::Native){p.stages[cold].rk=r[static_cast<int>(bias)];p.stages[cold].ck=0;}
 if(bypass!=CathodeBypass::Native)p.stages[cold].ck=c[static_cast<int>(bypass)];return p;
}
// Stage indices refer to the current prototype, not verified hardware revisions.
inline int coldStage(PreampModel m){return m==PreampModel::Marshall?1:2;}
inline PreampSpec modifiedSpec(PreampModel m, ColdBias bias, CathodeBypass bypass){
 auto p=preampSpec(m); auto& stage=p.stages[coldStage(m)];
 constexpr double r[]={0,1500,4700,10000,22000,39000,47000};
 constexpr double c[]={0,0,.68e-6,1e-6,22e-6};
 if(bias!=ColdBias::Native){stage.rk=r[static_cast<int>(bias)];stage.ck=0;}
 if(bypass!=CathodeBypass::Native)stage.ck=c[static_cast<int>(bypass)];
 return p;
}
// Revision-inspired approximations: not a full schematic transcription.
inline PreampSpec evhSpec(EVHVoice voice,ColdBias bias,CathodeBypass bypass){
 auto p=modifiedSpec((voice==EVHVoice::Red||voice==EVHVoice::RedStealth)?PreampModel::EVHRed:PreampModel::EVHBlue,bias,bypass);
 if(voice==EVHVoice::BlueEL34) p.stages[1].cc=22e-9; // provisional effective parallel coupling capacitance
 if(voice==EVHVoice::BlueStealth){
  p.stages[1].atten=.24; // reduced interstage series attenuation, provisional equivalent
  p.stages[1].cc=4.7e-9;
  p.stages[3].ck=22e-6;
 }
 if(voice==EVHVoice::RedStealth){
  // Engineering approximation of documented increased gain and low-control range.
  p.stages[3].atten=.22; p.cb=47e-9;
 }
 return p;
}
struct PreampStage {
 double cath=0, coupling=0, peak=0, lastCurrent=0, plateFiltered=0; bool init=false;
 std::array<double,7> coeffKey{};bool coeffReady=false;double gc=0,gp=0,gr=0,gl=0,link=0,den=0,effective=0;
 double process(double in,double supply,double dt,const StageSpec& s,double pot=1.,double bright=0.,bool usePot=false,bool eco=false,double volume=1.,bool postEQ=false){
 // Implicit coupling capacitor/grid leak and piecewise grid conduction.
 // Backward-Euler nodal solve: coupling cap, series peaker, pot and grid load.
 const std::array<double,7> key{dt,s.cc,s.rg,s.atten,s.peakC+bright,pot,usePot?1.:0.};
 if(!coeffReady||key!=coeffKey){
 const double top=usePot?std::max(1.,1e6*(1.-pot)):1.;
 const double bottom=usePot?std::max(1.,1e6*pot):1e12;
 const double rs=std::max(1.,38000.+s.rg*(1./std::max(.001,s.atten)-1.)+top);
 gc=s.cc/dt;gp=(s.peakC+bright)/dt;gr=1./rs;gl=1./s.rg+1./bottom;
 link=gr+gp;den=gc+link;effective=link*gc/den;
 coeffKey=key;coeffReady=true;
 }
 if(!init){coupling=in;peak=0;}
 auto solve=[&](double grid,double fixedCath,bool bypass){
 double lo=0,hi=std::max(0.,(supply-(bypass?fixedCath:0.))/(s.rp+(bypass?0.:s.rk)));
 auto residual=[&](double cur){double k=bypass?fixedCath:cur*s.rk;return triodeCurrent(supply-cur*s.rp-k,grid-k)-cur;};
 constexpr double K=.0012/(1.125*1.0606601717798212);
 double k=bypass?fixedCath:0.;
 double u=std::max(0.,1.125+grid-k+2+(supply-k-250)/100);
 double a=K*(s.rp/100+(bypass?0.:s.rk*1.01));
 const double slopeFactor=s.rp/100+(bypass?0.:s.rk*1.01);const double plateResistance=s.rp+(bypass?0.:s.rk);
 double cur=std::clamp(lastCurrent,lo,hi);
 if(eco && std::abs(residual(cur))>2e-5)cur=std::clamp(K*triodeLoad.get(u,a),lo,hi);
 for(int n=0;n<24;++n){
 double f=residual(cur);if(std::abs(f)<(eco?1e-8:1e-10))break;
 if(f>0)lo=cur;else hi=cur;
 double k=bypass?fixedCath:cur*s.rk;
 double vp=supply-cur*s.rp-k;
 double u=std::max(0.,1.125+grid-k+2+(vp-250)/100);
 double slope=1.5*K*std::sqrt(u)*slopeFactor*std::clamp(vp/10.,0.,1.);
 if(vp>0&&vp<10)slope+=K*u*std::sqrt(u)*plateResistance/10.;
 double next=cur+f/(1+slope);cur=(next>lo&&next<hi)?next:(lo+hi)*.5;
 }
 lastCurrent=cur;return cur;};
 if(!init){cath=solve(0,0,false)*s.rk;init=true;}
 const double gd=1./68000., threshold=cath+.5;
 // Two-node companion model, capacitor across series resistance.
 const double rhs=gc*(in-coupling);
 const double drive=link*rhs/den-gp*peak*gc/den;
 double vg=drive/(effective+gl);
 if(vg>threshold)vg=(drive+gd*threshold)/(effective+gl+gd);
 const double node=(rhs+link*vg+gp*peak)/den;
 coupling=in-node;peak=node-vg;
 double i=solve(vg,cath,s.ck>0);
 if(s.ck>0 && eco){
 // Eliminate the implicit capacitor unknown analytically: cath = base + effectiveR * current.
 const double h=dt/s.ck, den=1+h/s.rk, base=cath/den, effectiveR=h/den;
 const double R=s.rp+effectiveR;
 constexpr double K=.0012/(1.125*1.0606601717798212);
 const double u=1.125+vg-base+2+(supply-base-250)/100;
 const double slopeU=s.rp/100+1.01*effectiveR;
 double lo=0,hi=std::max(0.,(supply-base)/R);i=std::clamp(i,lo,hi);
 for(int n=0;n<24;++n){
 double vp=supply-base-R*i,w=std::max(0.,u-slopeU*i),root=std::sqrt(w),knee=std::clamp(vp/10.,0.,1.);
 double f=K*w*root*knee-i;if(std::abs(f)<1e-8)break;
 if(f>0)lo=i;else hi=i;
 double derivative=1+1.5*K*root*slopeU*knee;
 if(vp>0&&vp<10)derivative+=K*w*root*R/10.;
 double trial=i+f/derivative;i=(trial>lo&&trial<hi)?trial:(lo+hi)*.5;
 }
 cath=base+effectiveR*i;lastCurrent=i;
 }else if(s.ck>0){
 // Implicit cathode capacitor update; monotonic current decreases with cathode voltage.
 double h=dt/s.ck, old=cath, lo=0,hi=std::max(old,supply);
 const double cathDen=1+h/s.rk;double next=std::clamp((old+h*i)/cathDen,lo,hi);
 for(int n=0;n<16;++n){
 i=solve(vg,next,true);double f=old+h*i-next*cathDen;
 if(std::abs(f)<1e-7)break;
 if(f>0)lo=next;else hi=next;
 double trial=(old+h*i)/cathDen;
 next=(trial>lo&&trial<hi)?trial:(lo+hi)*.5;
 }
 cath=next;i=solve(vg,cath,true);
 }else cath=i*s.rk;
 double plate=supply-i*s.rp;if(s.rk==39000&&s.rp==100000){if(plateFiltered==0)plateFiltered=plate;plateFiltered+=(dt/(dt+100000*1e-9))*(plate-plateFiltered);return plateFiltered;}return plate;
 }
};
struct Preamp {
 std::array<PreampStage,5> stages{}; ToneStack tone; JoseClip jose; PreampModel active=PreampModel::Marshall; EVHVoice evhActive=EVHVoice::Blue; bool initialized=false; bool extraActive=true,gain2Ready=false; double gain1DC=0; bool gain1Ready=false; double gain2DC=0; double outputDC=0, followerDC=0; JoseArchitecture archActive=JoseArchitecture::Legacy; ColdBias biasActive=ColdBias::Native; CathodeBypass bypassActive=CathodeBypass::Native;
 double process(double input,double supply,double dt,PreampModel model,double gain,double bass,double mid,double treble,JoseEra era=JoseEra::Zener20,bool extraStage=true,double gain2=.7,JoseArchitecture arch=JoseArchitecture::Legacy,ColdBias bias=ColdBias::Native,CathodeBypass bypass=CathodeBypass::Native,RectoVoice recto=RectoVoice::EarlyRed,EVHVoice evh=EVHVoice::Blue,bool eco=false,double volume=1.,bool postEQ=false,int brightMode=1,double presence=.5){
 bool evhFamily=model==PreampModel::EVH5150III; if(!evhFamily)evh=model==PreampModel::EVHRed?EVHVoice::Red:EVHVoice::Blue; model=resolveEVH(model,evh);
 model=resolveRecto(model,recto);
 if(arch==JoseArchitecture::Legacy)arch=extraStage?JoseArchitecture::HBE:JoseArchitecture::Classic;
 if(!initialized||active!=model||evhActive!=evh||(biasActive!=bias||bypassActive!=bypass||(model==PreampModel::Jose && archActive!=arch))){*this=Preamp{};active=model;evhActive=evh;extraActive=extraStage;archActive=arch;biasActive=bias;bypassActive=bypass;initialized=true;}
 auto p=model==PreampModel::Jose?joseSpec(arch,bias,bypass):(evhFamily?evhSpec(evh,bias,bypass):modifiedSpec(model,bias,bypass)); double v=input;
 // Shared gain stages: avoid artificial channel gain differences. Treble capacitor values are secondary schematic reports; Multi-Watt follows mode.
 if(model==PreampModel::RectoOrange)p.ct=500e-12;
 if(model==PreampModel::RectoRed)p.ct=680e-12;
 if(model==PreampModel::RectoMulti){const bool road=recto==RectoVoice::RoadKing3Vintage||recto==RectoVoice::RoadKing3Modern||recto==RectoVoice::RoadKing4Vintage||recto==RectoVoice::RoadKing4Modern;p.ct=road?500e-12:(rectoModern(recto)?680e-12:500e-12);}
 for(int i=0;i<p.count;++i){
 bool first=i==p.gainAfter+1;
 bool second=model==PreampModel::Jose && arch!=JoseArchitecture::Stock && arch!=JoseArchitecture::Molecular && i==2;
 double pos=first?std::clamp(gain,0.,1.):second?std::clamp(gain2,0.,1.):1.;
 // Ascending capacitance order; additional values are generic modification options.
 const double caps[]={0.,100e-12,220e-12,470e-12,1e-9,2.2e-9,4.7e-9,5e-9};
 const double native=(model==PreampModel::Marshall||arch==JoseArchitecture::Stock)?470e-12:0.; // provisional until individual layouts are traced
 double bright=first&&(model==PreampModel::Marshall||model==PreampModel::Jose)?(brightMode==8?native:caps[std::clamp(brightMode,0,7)]):0.;
 v=stages[i].process(v,supply,dt,p.stages[i],pos,bright,first||second,eco);
 }

 // Reduced cathode follower: unity-ish gain with asymmetric headroom, DC removed.
 double follower=v; if(!(model==PreampModel::Jose && arch==JoseArchitecture::Snorkler)){if(eco){
 constexpr double K=.0012/(1.125*1.0606601717798212);
 double u=std::max(0.,1.125+v+2+(supply-250)/100);
 follower=std::min(supply,100000.*K*triodeLoad.get(u,K*101000.));
 }else{double lo=0,hi=supply;for(int j=0;j<22;++j){double k=(lo+hi)*.5; if(triodeCurrent(supply-k,v-k)>k/100000.)lo=k;else hi=k;} follower=(lo+hi)*.5;}}if(followerDC==0)followerDC=follower;followerDC+=dt/(.02+dt)*(follower-followerDC);v=(follower-followerDC); if(model==PreampModel::Jose && arch==JoseArchitecture::Snorkler)v=(follower-followerDC);
 const bool clipping=model==PreampModel::Jose && arch!=JoseArchitecture::Stock;
 if(clipping&&!postEQ)v=jose.process(v,dt,era,volume);
 const bool rectoFamily=isRecto(model);const bool modern=rectoFamily&&rectoModern(recto);
 const bool narrow=recto==RectoVoice::EarlyOrangeModern||recto==RectoVoice::RoadKing3Modern;
 const double presenceR=modern&&recto!=RectoVoice::EarlyOrangeModern?22000.+((narrow||recto==RectoVoice::EarlyRed)?25000.:100000.)*std::clamp(presence,0.,1.):0.;
 if(rectoFamily){p.cb=20e-9;p.cm=20e-9;}
 v=tone.process(v,dt,bass,mid,treble,p.slope,p.ct,p.cb,p.cm,model==PreampModel::Jose && arch==JoseArchitecture::Snorkler?38000.:600.,presenceR);
 if(clipping&&postEQ)v=jose.process(v,dt,era,volume);return v;
 }
};
}
