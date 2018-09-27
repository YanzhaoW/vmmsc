#include "hdmi.h"

HDMI::HDMI():m_hybridAct (HYBRIDS_PER_HDMI)
{
    m_hybridAct[0] = 1;
}

bool HDMI::SetHybrid(unsigned short hybrid, bool OnOff){
    if (hybrid < HYBRIDS_PER_HDMI) {m_hybridAct[hybrid] = OnOff; return true;}
    else {return false;}
}

bool HDMI::GetHybrid(unsigned short hybrid){
    if (hybrid < HYBRIDS_PER_HDMI) {return m_hybridAct[hybrid];}
    else {return false;}
}

HDMI::~HDMI()
{

}
