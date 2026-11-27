#include "Amp.h"
#include <iostream>
int main(){for(auto tube:{uhg::Tube::EL34,uhg::Tube::EL84,uhg::Tube::L6})for(double rate:{44100.,48000.,96000.}){uhg::Engine e;e.controls.tube=tube;e.prepare(rate);double sum=0,peak=0;for(int i=0;i<int(rate*.025);++i){double y=e.process(.1*std::sin(6.283185307*440*i/rate));if(!std::isfinite(y))return 1;sum+=y*y;peak=std::max(peak,std::abs(y));}std::cout<<int(tube)<<" "<<rate<<" RMS "<<std::sqrt(sum/int(rate*.025))<<" peak "<<peak<<" supply "<<e.supply<<" bias "<<e.bias<<'\n';if(peak<1e-8)return 2;}}
