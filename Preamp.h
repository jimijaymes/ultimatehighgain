#pragma once
#include "Circuits.h"
#include "JoseClip.h"
#include <array>
namespace uhg {
enum class PreampModel { Marshall, Jose, Peavey5150, RectoOrange, RectoRed, RectoMulti, VHT, EVHBlue, EVHRed, DualRectifier, EVH5150III };
enum class EVHVoice { Blue, Red, BlueEL34, BlueStealth, RedStealth };
inline PreampModel resolveEVH(PreampModel m,EVHVoice v){return m==PreampModel::EVH5150III?((v==EVHVoice::Red||v==EVHVoice::RedStealth)?PreampModel::EVHRed:PreampModel::EVHBlue):m;}
enum class RectoVoice { EarlyOrange, EarlyRed, MultiWattModern };
inline bool isRecto(PreampModel m){return m==PreampModel::RectoOrange||m==PreampModel::RectoRed||m==PreampModel::RectoMulti||m==PreampModel::DualRectifier;}
inline PreampModel resolveRecto(PreampModel m, RectoVoice v){if(m!=PreampModel::DualRectifier)return m;return v==RectoVoice::EarlyOrange?PreampModel::RectoOrange:v==RectoVoice::EarlyRed?PreampModel::RectoRed:PreampModel::RectoMulti;}
enum class JoseArchitecture { Legacy, Classic, HBE, Snorkler, Stock };
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
 case PreampModel::DualRectifier:case PreampModel::RectoOrange:case PreampModel::RectoRed:case PreampModel::RectoMulti:p.count=4;p.stages={a,warm,cold,hot,warm};p.stages[2].rk=39000;p.slope=47000;p.ct=500e-12;p.cb=p.cm=47e-9;if(m==PreampModel::RectoOrange)p.stages[3].atten=.08;else if(m==PreampModel::RectoMulti)p.stages[1].cc=10e-9;break;
 case PreampModel::VHT:p.count=4;p.stages={a,hot,cold,warm,hot};p.stages[1].cc=4.7e-9;p.stages[2].cc=10e-9;break;
 case PreampModel::EVH5150III:case PreampModel::EVHBlue:p.count=4;p.stages={a,warm,cold,hot,warm};p.stages[1].cc=10e-9;break;
 case PreampModel::EVHRed:p.count=5;p.stages={a,warm,cold,hot,warm};p.stages[1].cc=10e-9;p.stages[3].atten=.18;break;}
 return p;
}
inline PreampSpec joseSpec(JoseArchitecture arch, ColdBias bias, CathodeBypass bypass){
 auto p=preampSpec(PreampModel::Jose); int cold=2;
 if(arch==JoseArchitecture::Classic||arch==JoseArchitecture::Stock){p=preampSpec(PreampModel::Marshall);cold=1;}
 if(arch==JoseArchitecture::Snorkler){p.stages[2].rk=1500;p.stages[2].ck=1e-6;p.stages[3].rk=1500;p.stages[3].ck=1e-6;p.slope=100000;for(int i=1;i<4;++i)p.stages[i].cc=47e-9;}
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
 double cath=0, coupling=0, peak=0; bool init=false;
 double process(double in,double supply,double dt,const StageSpec& s,double pot=1.,double bright=0.,bool usePot=false){
 // Implicit coupling capacitor/grid leak and piecewise grid conduction.
 // Backward-Euler nodal solve: coupling cap, series peaker, pot and grid load.
 const double sourceR=38000.;
 const double top=usePot?std::max(1.,1e6*(1.-pot)):1.;
 const double bottom=usePot?std::max(1.,1e6*pot):1e12;
 const double series=s.rg*(1./std::max(.001,s.atten)-1.);
 const double rs=std::max(1.,sourceR+series+top);
 const double cp=s.peakC+bright;
 if(!init){coupling=in;peak=0;}
 auto solve=[&](double grid,double fixedCath,bool bypass){double lo=0,hi=supply/s.rp;for(int i=0;i<18;++i){double cur=(lo+hi)*.5;double k=bypass?fixedCath:cur*s.rk;double ip=triodeCurrent(supply-cur*s.rp-k,grid-k);if(ip>cur)lo=cur;else hi=cur;}return (lo+hi)*.5;};
 if(!init){cath=solve(0,0,false)*s.rk;init=true;}
 const double gc=s.cc/dt, gp=cp/dt, gr=1./rs, gl=1./s.rg+1./bottom, gd=1./68000., threshold=cath+.5;
 // Two-node companion model, capacitor across series resistance.
 const double link=gr+gp, den=gc+link;
 const double rhs=gc*(in-coupling);
 const double effective=link*gc/den;
 const double drive=link*rhs/den-gp*peak*gc/den;
 double vg=drive/(effective+gl);
 if(vg>threshold)vg=(drive+gd*threshold)/(effective+gl+gd);
 const double node=(rhs+link*vg+gp*peak)/den;
 coupling=in-node;peak=node-vg;
 double i=solve(vg,cath,s.ck>0);
 if(s.ck>0)cath+=(i*s.rk-cath)*(-std::expm1(-dt/(s.rk*s.ck)));else cath=i*s.rk;
 return supply-i*s.rp;
 }
};
struct Preamp {
 std::array<PreampStage,5> stages{}; ToneStack tone; JoseClip jose; PreampModel active=PreampModel::Marshall; EVHVoice evhActive=EVHVoice::Blue; bool initialized=false; bool extraActive=true,gain2Ready=false; double gain1DC=0; bool gain1Ready=false; double gain2DC=0; double outputDC=0, followerDC=0; JoseArchitecture archActive=JoseArchitecture::Legacy; ColdBias biasActive=ColdBias::Native; CathodeBypass bypassActive=CathodeBypass::Native;
 double process(double input,double supply,double dt,PreampModel model,double gain,double bass,double mid,double treble,JoseEra era=JoseEra::Zener20,bool extraStage=true,double gain2=.7,JoseArchitecture arch=JoseArchitecture::Legacy,ColdBias bias=ColdBias::Native,CathodeBypass bypass=CathodeBypass::Native,RectoVoice recto=RectoVoice::EarlyRed,EVHVoice evh=EVHVoice::Blue){
 bool evhFamily=model==PreampModel::EVH5150III; if(!evhFamily)evh=model==PreampModel::EVHRed?EVHVoice::Red:EVHVoice::Blue; model=resolveEVH(model,evh);
 model=resolveRecto(model,recto);
 if(arch==JoseArchitecture::Legacy)arch=extraStage?JoseArchitecture::HBE:JoseArchitecture::Classic;
 if(!initialized||active!=model||evhActive!=evh||(biasActive!=bias||bypassActive!=bypass||(model==PreampModel::Jose && archActive!=arch))){*this=Preamp{};active=model;evhActive=evh;extraActive=extraStage;archActive=arch;biasActive=bias;bypassActive=bypass;initialized=true;}
 auto p=model==PreampModel::Jose?joseSpec(arch,bias,bypass):(evhFamily?evhSpec(evh,bias,bypass):modifiedSpec(model,bias,bypass)); double v=input;
 for(int i=0;i<p.count;++i){
 bool first=i==p.gainAfter+1;
 bool second=model==PreampModel::Jose && arch!=JoseArchitecture::Stock && i==2;
 double pos=first?std::clamp(gain,0.,1.):second?std::clamp(gain2,0.,1.):1.;
 double bright=first&&(model==PreampModel::Marshall||(model==PreampModel::Jose&&arch==JoseArchitecture::Stock))?470e-12:0.;
 v=stages[i].process(v,supply,dt,p.stages[i],pos,bright,first||second);
 }

 // Reduced cathode follower: unity-ish gain with asymmetric headroom, DC removed.
 double follower=v; if(!(model==PreampModel::Jose && arch==JoseArchitecture::Snorkler)){double lo=0,hi=supply;for(int j=0;j<18;++j){double k=(lo+hi)*.5; if(triodeCurrent(supply-k,v-k)>k/100000.)lo=k;else hi=k;} follower=(lo+hi)*.5;}if(followerDC==0)followerDC=follower;followerDC+=dt/(.02+dt)*(follower-followerDC);v=(follower-followerDC); if(model==PreampModel::Jose && arch==JoseArchitecture::Snorkler)v=(follower-followerDC);
 if(model==PreampModel::Jose && arch!=JoseArchitecture::Stock)v=jose.process(v,dt,era);
 return tone.process(v,dt,bass,mid,treble,p.slope,p.ct,p.cb,p.cm,model==PreampModel::Jose && arch==JoseArchitecture::Snorkler?38000.:600.);
 }
};
}
