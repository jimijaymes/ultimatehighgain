#pragma once
#include <cmath>
#include <algorithm>
#include "Tubes.h"
namespace uhg_baseline032 {
// Backward Euler nodal passive tone stack: slope resistor, three capacitors,
// bass rheostat, mid rheostat, treble potentiometer and master input load.
struct ToneStack {
 double history[3]{}; double presenceCharge=0; double inverse[6][6]{}; double key[10]{}; bool cached=false;
 double process(double in,double dt,double bass,double mid,double treble,double slope=33000,double ct=470e-12,double cb=22e-9,double cm=22e-9,double sourceR=0,double presenceResistance=0){
 double a[6][7]{}; // slope, treble top, bass top, lower stack, wiper
 auto stamp=[&](int i,int j,double g,double h=0.){if(i>=0){a[i][i]+=g;a[i][6]+=h;}if(j>=0){a[j][j]+=g;a[j][6]-=h;}if(i>=0&&j>=0){a[i][j]-=g;a[j][i]-=g;}};
 auto cap=[&](int i,int j,double c,int k,double source=0.){double g=c/dt;stamp(i,j,g,g*(history[k]+source));};
 stamp(0,5,1./slope);stamp(5,-1,1./std::max(1.,sourceR),in/std::max(1.,sourceR));
 cap(1,5,ct,0);cap(0,2,cb,1);cap(0,3,cm,2);
 stamp(2,3,1./(100+1e6*bass));stamp(3,-1,1./(100+25000*mid));
 stamp(1,4,1./(100+250000*(1-treble)));stamp(4,2,1./(100+250000*treble));stamp(4,-1,1e-6);
 // Series RC shunt at treble wiper, eliminated analytically using backward Euler.
 const double pg=presenceResistance>0?1./(presenceResistance+dt/3e-9):0.;
 stamp(4,-1,pg,pg*presenceCharge);
 double nextKey[]={dt,bass,mid,treble,slope,ct,cb,cm,sourceR,presenceResistance};
 if(!cached||!std::equal(key,key+10,nextKey)){
 double m[6][12]{};
 for(int i=0;i<6;++i){for(int j=0;j<6;++j)m[i][j]=a[i][j];m[i][i+6]=1.;}
 for(int k=0;k<6;++k){double d=m[k][k];for(int j=0;j<12;++j)m[k][j]/=d;for(int i=0;i<6;++i)if(i!=k){double f=m[i][k];for(int j=0;j<12;++j)m[i][j]-=f*m[k][j];}}
 for(int i=0;i<6;++i)for(int j=0;j<6;++j)inverse[i][j]=m[i][j+6];
 std::copy(nextKey,nextKey+10,key);cached=true;
 }
 double result[6]{};for(int i=0;i<6;++i)for(int j=0;j<6;++j)result[i]+=inverse[i][j]*a[j][6];
 for(int i=0;i<6;++i)a[i][6]=result[i];
 history[0]=a[1][6]-a[5][6];history[1]=a[0][6]-a[2][6];history[2]=a[0][6]-a[3][6];if(pg>0)presenceCharge+=dt/3e-9*pg*(a[4][6]-presenceCharge);else presenceCharge=0;return a[4][6];
 }
};
// Reduced-order shared-tail nonlinear LTP, DC operating point removed.
struct PhaseInverterSpec {double plate1=82000,plate2=100000,tail=10470,biasR=470,coupling=22e-9,gridLeak=220000,gridStop=5600;};
struct PhaseInverter {
 double dc1=0,dc2=0; bool initialized=false;
 static double current(double grid,double cath,double supply,double r){
 double available=std::max(0.,supply-cath);
 constexpr double K=.0012/(1.125*1.0606601717798212);
 double u=std::max(0.,1.125+grid-cath+2+(available-250)/100);
 if(u<=0.||available<=0.)return 0.;
 double lo=0,hi=available/r;
 double i=std::clamp(K*triodeLoad.get(u,K*r/100.),lo,hi);
 for(int k=0;k<20;++k){
 double vp=available-i*r, w=std::max(0.,u-i*r/100.);
 double knee=std::clamp(vp/10.,0.,1.);const double root=std::sqrt(w);
 double f=K*w*root*knee-i;
 if(std::abs(f)<1e-10)return i;
 if(f>0)lo=i;else hi=i;
 double slope=1+1.5*K*root*r/100.*knee;
 if(vp>0&&vp<10)slope+=K*w*root*r/10.;
 double next=i+f/slope;i=(next>lo&&next<hi)?next:(lo+hi)*.5;
 }
 return i;
 }
 double tailEstimate=0, cachedDt=0, alpha=0, cachedBias=0,cachedTail=0,gridFactor=0;
 void process(double input,double feedback,double supply,double dt,double &o1,double &o2,PhaseInverterSpec spec={},bool eco=false){
 if(cachedDt!=dt){cachedDt=dt;alpha=1-std::exp(-6.283185307*2*dt);}if(cachedBias!=spec.biasR||cachedTail!=spec.tail){cachedBias=spec.biasR;cachedTail=spec.tail;gridFactor=1-spec.biasR/spec.tail;}
 double lo=0,hi=supply;double i1=0,i2=0;
 double cath=initialized?std::clamp(tailEstimate,lo,hi):supply*.03;
 double prevCath=0.,prevF=0.;bool havePrev=false;
 for(int k=0;k<(eco?14:20);++k){
 double gridDC=cath*gridFactor;
 i1=current(input+gridDC,cath,supply,spec.plate1);
 i2=current(feedback+gridDC,cath,supply,spec.plate2);
 double f=(i1+i2)*spec.tail-cath;
 if(std::abs(f)<1e-5)break;
 if(f>0)lo=cath;else hi=cath;
 if(k+1==(eco?14:20))break;
 double next=(lo+hi)*.5;
 if(eco&&havePrev&&f!=prevF){const double sec=cath-f*(cath-prevCath)/(f-prevF);if(sec>lo&&sec<hi)next=sec;}
 prevCath=cath;prevF=f;havePrev=true;cath=next;
 }
 tailEstimate=cath;
 double p1=supply-i1*spec.plate1,p2=supply-i2*spec.plate2;if(!initialized){dc1=p1;dc2=p2;initialized=true;}dc1+=alpha*(p1-dc1);dc2+=alpha*(p2-dc2);o1=dc1-p1;o2=dc2-p2;
 }

 // Coupled reduced-order PI output port and backward-Euler grid load.
 // Preserve the engine's inverted plate-output convention (drive = dc - plate).
 void loaded(double input,double feedback,double supply,double dt,double bias,double cathodeV,
             double charge1,double charge2,double& o1,double& o2,PhaseInverterSpec spec,bool fast){
  if(!initialized){process(input,feedback,supply,dt,o1,o2,spec,false);}
  const double gc=spec.coupling/dt, gl=1/spec.gridLeak, gs=1/spec.gridStop;
  const double g=gc+gl, gg=g+gs, threshold=cathodeV;
  const double factor=1-spec.biasR/spec.tail;
  auto solve=[&](double signal,double cath,double r,double dc,double charge){
   auto eval=[&](double p,double& deriv){
    const double drive=dc-p, rhs=bias*gl+gc*(drive-charge);
    const bool conducting=rhs/g>threshold;
    const double vg=conducting?(rhs+gs*threshold)/gg:rhs/g;
    const double load=gc*(drive-charge-vg);
    const double u=std::max(0.,1.125+signal+cath*factor-cath+2+(p-cath-250)/100);
    constexpr double K=.0012/(1.125*1.0606601717798212);
    const double root=std::sqrt(u), knee=std::clamp((p-cath)/10.,0.,1.);
    const double ip=K*u*root*knee;
    const double dip=1.5*K*root*knee/100+(p>cath&&p<cath+10?K*u*root/10:0);
    deriv=1+r*(dip+gc*(1-gc/(conducting?gg:g)));
    return p-supply+r*(ip-load);
   };
   double lo=-supply,hi=2*supply, p=std::clamp(dc,lo,hi);
   for(int j=0;j<40;++j){double slope=0,f=eval(p,slope);if(std::abs(f)<1e-7)break;
    if(f>0)hi=p;else lo=p;
    const double next=p-f/slope;p=fast&&next>lo&&next<hi?next:(lo+hi)*.5;
   }
   const double u=std::max(0.,1.125+signal+cath*factor-cath+2+(p-cath-250)/100);
   constexpr double K=.0012/(1.125*1.0606601717798212);
   return std::array<double,2>{p,K*u*std::sqrt(u)*std::clamp((p-cath)/10.,0.,1.)};
  };
  double lo=0,hi=supply,cath=std::clamp(tailEstimate,lo,hi);
  std::array<double,2> a{},b{};double prevCath=0,prevF=0;bool havePrev=false;
  for(int k=0;k<32;++k){a=solve(input,cath,spec.plate1,dc1,charge1);b=solve(feedback,cath,spec.plate2,dc2,charge2);
   double f=(a[1]+b[1])*spec.tail-cath;if(std::abs(f)<1e-5||k==31)break;
   if(f>0)lo=cath;else hi=cath;double next=(lo+hi)*.5;
   if(fast&&havePrev&&f!=prevF){const double sec=cath-f*(cath-prevCath)/(f-prevF);if(sec>lo&&sec<hi)next=sec;}
   prevCath=cath;prevF=f;havePrev=true;cath=next;
  }
  tailEstimate=cath;
  // Keep the unloaded DC reference: tracking loaded plate DC would erase blocking shifts.
  o1=dc1-a[0];o2=dc2-b[0];
 }
};
struct GridCoupling {double charge=0,keyDt=0,keyC=0,keyR=0,keyRs=0,gc=0,g=0,conductingG=0;double process(double drive,double bias,double dt,PhaseInverterSpec spec={},double threshold=0){double c=spec.coupling,r=spec.gridLeak,rs=spec.gridStop;if(dt!=keyDt||c!=keyC||r!=keyR||rs!=keyRs){keyDt=dt;keyC=c;keyR=r;keyRs=rs;gc=c/dt;g=1/r+gc;conductingG=g+1/rs;}const double rhs=bias/r+gc*(drive-charge);double v=rhs/g; if(v>threshold)v=(rhs+threshold/rs)/conductingG;charge=drive-v;return v;}};
}
