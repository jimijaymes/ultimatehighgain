#pragma once
#include <cmath>
#include <algorithm>
namespace uhg_baseline032 {
enum class JoseEra { Open, Zener20, Zener12, Asymmetric };
// Reduced-order AC-coupled shunt clipper. Voltages are physical volts.
struct JoseClip {
 double cap=0;
 double process(double input,double dt,JoseEra mode,double volume=1.){
  constexpr double c=.33e-6, load=1e6, rd=350.;
  volume=std::clamp(volume,0.,1.);const double rs=10000.+load*volume*(1-volume);input*=volume;
  const double a=dt/c, drive=input-cap;
  const double pos=mode==JoseEra::Zener12?12.65:20.65;
  const double neg=mode==JoseEra::Asymmetric?12.65:pos;
  auto current=[&](double v){if(mode==JoseEra::Open)return 0.;return (std::max(0.,v-pos)-std::max(0.,-v-neg))/rd;};
  double lo=std::min(0.,drive), hi=std::max(0.,drive);
  for(int j=0;j<24;++j){double v=(lo+hi)*.5; if(v+(rs+a)*(v/load+current(v))>drive)hi=v;else lo=v;}
  double v=(lo+hi)*.5;cap+=a*(v/load+current(v));return v;
 }
};
}
