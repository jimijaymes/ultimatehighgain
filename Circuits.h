#pragma once
#include <cmath>
#include <algorithm>
#include "Tubes.h"
namespace uhg {
// Backward Euler nodal passive tone stack: slope resistor, three capacitors,
// bass rheostat, mid rheostat, treble potentiometer and master input load.
struct ToneStack {
 double history[3]{}; double inverse[6][6]{}; double key[9]{}; bool cached=false;
 double process(double in,double dt,double bass,double mid,double treble,double slope=33000,double ct=470e-12,double cb=22e-9,double cm=22e-9,double sourceR=0){
 double a[6][7]{}; // slope, treble top, bass top, lower stack, wiper
 auto stamp=[&](int i,int j,double g,double h=0.){if(i>=0){a[i][i]+=g;a[i][6]+=h;}if(j>=0){a[j][j]+=g;a[j][6]-=h;}if(i>=0&&j>=0){a[i][j]-=g;a[j][i]-=g;}};
 auto cap=[&](int i,int j,double c,int k,double source=0.){double g=c/dt;stamp(i,j,g,g*(history[k]+source));};
 stamp(0,5,1./slope);stamp(5,-1,1./std::max(1.,sourceR),in/std::max(1.,sourceR));
 cap(1,5,ct,0);cap(0,2,cb,1);cap(0,3,cm,2);
 stamp(2,3,1./(100+1e6*bass));stamp(3,-1,1./(100+25000*mid));
 stamp(1,4,1./(100+250000*(1-treble)));stamp(4,2,1./(100+250000*treble));stamp(4,-1,1e-6);
 double nextKey[]={dt,bass,mid,treble,slope,ct,cb,cm,sourceR};
 if(!cached||!std::equal(key,key+9,nextKey)){
 double m[6][12]{};
 for(int i=0;i<6;++i){for(int j=0;j<6;++j)m[i][j]=a[i][j];m[i][i+6]=1.;}
 for(int k=0;k<6;++k){double d=m[k][k];for(int j=0;j<12;++j)m[k][j]/=d;for(int i=0;i<6;++i)if(i!=k){double f=m[i][k];for(int j=0;j<12;++j)m[i][j]-=f*m[k][j];}}
 for(int i=0;i<6;++i)for(int j=0;j<6;++j)inverse[i][j]=m[i][j+6];
 std::copy(nextKey,nextKey+9,key);cached=true;
 }
 double result[6]{};for(int i=0;i<6;++i)for(int j=0;j<6;++j)result[i]+=inverse[i][j]*a[j][6];
 for(int i=0;i<6;++i)a[i][6]=result[i];
 history[0]=a[1][6]-a[5][6];history[1]=a[0][6]-a[2][6];history[2]=a[0][6]-a[3][6];return a[4][6];
 }
};
// Reduced-order shared-tail nonlinear LTP, DC operating point removed.
struct PhaseInverterSpec {double plate1=82000,plate2=100000,tail=10470,biasR=470,coupling=22e-9,gridLeak=220000,gridStop=5600;};
struct PhaseInverter {
 double dc1=0,dc2=0; bool initialized=false;
 static double current(double grid,double cath,double supply,double r){double lo=0,hi=supply/r;double i=0;for(int k=0;k<6;k++){double p=supply-i*r;double m=triodeCurrent(p-cath,grid-cath);double slope=(triodeCurrent(p-cath+.1,grid-cath)-m)*10.;double next=std::clamp(i+(m-i)/(1+r*std::max(0.,slope)),lo,hi);i=next;}return i;}
 // The resistive triode load line depends on effective grid drive and available plate supply.
 static double lookupCurrent(double grid,double cath,double supply,double r){
  // Analytic inverse approximation of I = K*(u - I*r/100)^1.5.
  // Solve in sqrt(overdrive) using two bounded Newton steps, no nested current loop.
  double available=std::max(0.,supply-cath);
  double u=std::max(0.,1.125+grid-cath+2+(available-250)/100);
  constexpr double K=.0012/(1.125*1.0606601717798212);
  double a=K*r/100.; return std::min(available/r,K*triodeLoad.get(u,a));
 }
 double tailEstimate=0;
 void process(double input,double feedback,double supply,double dt,double &o1,double &o2,PhaseInverterSpec spec={},bool eco=false){
 double lo=0,hi=supply*.5;double i1=0,i2=0;
 if(eco){
  double cath=initialized?std::clamp(tailEstimate,lo,hi):supply*.03;
  for(int k=0;k<3;++k){
   double gridDC=cath*(1-spec.biasR/spec.tail);
   i1=lookupCurrent(input+gridDC,cath,supply,spec.plate1);
   i2=lookupCurrent(feedback+gridDC,cath,supply,spec.plate2);
   double err=(i1+i2)*spec.tail-cath;
   if(std::abs(err)<.001)break;
   if(err>0)lo=cath;else hi=cath;
   double next=cath+err*.65;
   cath=(next>lo&&next<hi)?next:(lo+hi)*.5;
  }
  tailEstimate=cath;
 }else{
 for(int k=0;k<10;k++){double cath=(lo+hi)*.5;double gridDC=cath*(1-spec.biasR/spec.tail);i1=current(input+gridDC,cath,supply,spec.plate1);i2=current(feedback+gridDC,cath,supply,spec.plate2);if((i1+i2)*spec.tail>cath)lo=cath;else hi=cath;}
 }
 double p1=supply-i1*spec.plate1,p2=supply-i2*spec.plate2;if(!initialized){dc1=p1;dc2=p2;initialized=true;}double alpha=1-std::exp(-6.283185307*2*dt);dc1+=alpha*(p1-dc1);dc2+=alpha*(p2-dc2);o1=dc1-p1;o2=dc2-p2;
 }
};
struct GridCoupling {double charge=0;double process(double drive,double bias,double dt,PhaseInverterSpec spec={}){double c=spec.coupling,r=spec.gridLeak,rs=spec.gridStop;double g=1/r+c/dt;double v=(bias/r+c/dt*(drive-charge))/g; if(v>0)v=(bias/r+c/dt*(drive-charge))/(g+1/rs);charge=drive-v;return v;}};
}
