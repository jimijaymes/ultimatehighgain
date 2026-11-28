#pragma once
#include <cmath>
#include <algorithm>
#include <array>
namespace uhg_max032 {
enum class Tube { EL34, EL84, L6, KT88 };
// Local datasheet anchor fit, not a full curve fit. SI units.
struct TubeData {double mu, gm, va, vg, vs, ia, is, rp, knee, idle, rating;};
inline TubeData data(Tube t){switch(t){case Tube::KT88:return {8,.0115,250,-15,250,.140,.003,12000,25,.050,42};case Tube::EL84:return {19,.0113,250,-7.3,250,.048,.0055,40000,18,.035,12};case Tube::L6:return {13,.006,250,-14,250,.072,.005,22500,30,.035,30};default:return {11,.011,250,-12,265,.100,.0149,15000,25,.035,25};}}
inline double emission(TubeData t,double vg,double vs){double u0=1.5*t.ia/t.gm;double u=std::max(0.,u0+vg-t.vg+(vs-t.vs)/t.mu);return t.ia*(u/u0)*std::sqrt(u/u0);}
// Shared immutable table of exp(-x), linear interpolation at 1/256 spacing.
struct ValveExpTable {
 std::array<double,4097> values{};
 ValveExpTable(){for(size_t i=0;i<values.size();++i)values[i]=std::exp(-double(i)/256.);}
 double get(double x) const {if(x<=0)return 1.;if(x>=16)return 0.;double p=x*256.;auto i=static_cast<size_t>(p);return values[i]+(values[i+1]-values[i])*(p-i);}
};
inline const ValveExpTable valveExp{};
inline double plate(TubeData t,double vp,double vg,double vs){vp=std::max(0.,vp);double knee=(1-valveExp.get(vp/t.knee))/(1-valveExp.get(t.va/t.knee));double slope=std::max(0.,1+(vp-t.va)/(t.rp*t.ia));return emission(t,vg,vs)*knee*slope;}
inline double screenCurrent(TubeData t,double vp,double vg,double vs){double share=t.is/t.ia; double collect=1+2*valveExp.get(std::max(0.,vp)/t.knee);double ref=1+2*valveExp.get(t.va/t.knee);return emission(t,vg,vs)*share*collect/ref*std::max(0.,vs)/t.vs;}
// Factorised lookup of the ECC83 resistive load line. Axis u is effective
// grid overdrive; axis a represents the combined plate/cathode resistance.
struct TriodeLoadTable {
 static constexpr int U=1025,A=257;
 std::array<float,U*A> table{};
 TriodeLoadTable(){for(int j=0;j<A;++j){double a=std::expm1(std::log(257.)*j/(A-1));for(int i=0;i<U;++i){double u=128.*i*i/((U-1.)*(U-1.)),lo=0,hi=std::sqrt(u);for(int k=0;k<28;++k){double z=(lo+hi)*.5;if(z*z+a*z*z*z>u)hi=z;else lo=z;}double z=(lo+hi)*.5;table[j*U+i]=float(z*z*z);}}}
 double get(double u,double a)const{
 if(u<=0)return 0.;
 if(u>128||a<0||a>256){double lo=0,hi=std::sqrt(u);for(int k=0;k<24;++k){double z=(lo+hi)*.5;if(z*z+a*z*z*z>u)hi=z;else lo=z;}double z=(lo+hi)*.5;return z*z*z;}
 double x=std::sqrt(u/128.)*(U-1),y=std::log1p(a)*(A-1)/std::log(257.);int i=std::min(U-2,int(x)),j=std::min(A-2,int(y));double f=x-i,g=y-j;
 double p=table[j*U+i]*(1-f)+table[j*U+i+1]*f,q=table[(j+1)*U+i]*(1-f)+table[(j+1)*U+i+1]*f;return p+(q-p)*g;
 }
};
inline const TriodeLoadTable triodeLoad{};
inline double triodeCurrent(double vp,double vg){ // ECC83S: 1.2mA, 1.6mS, mu 100 at 250V/-2V.
 double u=std::max(0.,1.125+vg+2+(vp-250)/100);return .0012*(u/1.125)*std::sqrt(u/1.125)*std::clamp(vp/10.,0.,1.);}
}
