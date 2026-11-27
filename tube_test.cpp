#include "Tubes.h"
#include <iostream>
int main(){for(auto type:{uhg::Tube::EL34,uhg::Tube::EL84,uhg::Tube::L6}){auto t=uhg::data(type);double i=uhg::plate(t,t.va,t.vg,t.vs),is=uhg::screenCurrent(t,t.va,t.vg,t.vs);double gm=(uhg::plate(t,t.va,t.vg+.001,t.vs)-uhg::plate(t,t.va,t.vg-.001,t.vs))/.002;std::cout<<i<<' '<<is<<' '<<gm<<'\n';if(std::abs(i-t.ia)>1e-8||std::abs(is-t.is)>1e-8||std::abs(gm-t.gm)>1e-6)return 1;}if(std::abs(uhg::triodeCurrent(250,-2)-.0012)>1e-9)return 2;}
