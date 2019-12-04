#ifndef mat_h
#define mat_h

#include <Arduino.h>

#define water_dp 4.0  //delta preassure water
#define water_dn 1.0  //density water

// namespace Mth
namespace Mth
{
    float density(float dp);
    float procent_p (float dph,float dpl);
    float procent_m (float dph,float dpl);
    float procent_pm(float pp,float pm);
}

float Mth::density(float dp)
{
    float zn = 0;
    if (dp > 0)
      zn = dp / water_dp;
    return zn;  
}

float Mth::procent_p (float dph,float dpl)
{
    float zn = 0; 
    if (dph > 0 && dpl > 0)
      zn = (density(dph) - density(dpl)) / (density(dph)-water_dn) * 100;
    return zn;  
}

float Mth::procent_m (float dph,float dpl)
{
    float zn = 0; 
    if (dph > 0 && dpl > 0)
      zn = (density(dph) - density(dpl)) / (density(dph)-water_dn) * 100;
    return zn;  
}

float Mth::procent_pm(float pp,float pm)
{
    float zn = 0; 
    if (pp > 0 && pm > 0)
      zn = pm / pp * 100;
    return zn;
}

#endif