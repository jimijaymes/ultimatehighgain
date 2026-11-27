#pragma once
#include <cmath>
#include <algorithm>
namespace uhg {
enum class Tube { EL34, EL84, L6 };
// Local datasheet anchor fit, not a full curve fit. SI units.
struct TubeData {double mu, gm, va, vg, vs, ia, is, rp, knee, idle, rating;};
inline TubeData data(Tube t){switch(t){case Tube::EL84:return {19,.0113,250,-7.3,250,.048,.0055,40000,18,.035,12};case Tube::L6:return {13,.006,250,-14,250,.072,.005,22500,30,.035,30};default:return {11,.011,250,-12,265,.100,.0149,15000,25,.035,25};}}
inline double emission(TubeData t,double vg,double vs){double u0=1.5*t.ia/t.gm;double u=std::max(0.,u0+vg-t.vg+(vs-t.vs)/t.mu);return t.ia*(u/u0)*std::sqrt(u/u0);}
inline double plate(TubeData t,double vp,double vg,double vs){vp=std::max(0.,vp);double knee=-std::expm1(-vp/t.knee)/(-std::expm1(-t.va/t.knee));double slope=std::max(0.,1+(vp-t.va)/(t.rp*t.ia));return emission(t,vg,vs)*knee*slope;}
inline double screenCurrent(TubeData t,double vp,double vg,double vs){double share=t.is/t.ia; double collect=1+2*std::exp(-std::max(0.,vp)/t.knee);double ref=1+2*std::exp(-t.va/t.knee);return emission(t,vg,vs)*share*collect/ref*std::max(0.,vs)/t.vs;}
inline double triodeCurrent(double vp,double vg){ // ECC83S: 1.2mA, 1.6mS, mu 100 at 250V/-2V.
 double u=std::max(0.,1.125+vg+2+(vp-250)/100);return .0012*(u/1.125)*std::sqrt(u/1.125)*std::clamp(vp/10.,0.,1.);}
}
