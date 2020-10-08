#ifndef HDMI_H
#define HDMI_H

#include "globparameter.h"
#include "hybrid.h"

class HDMI: public QObject
{
    Q_OBJECT
public:
    HDMI();
    ~HDMI();
    friend class Commandline;
    friend class CalibrationModule;
    Hybrid m_hybrids[HYBRIDS_PER_HDMI];

    bool SetHybrid(unsigned short m_hybrids, bool OnOff);
    bool GetHybrid(unsigned short m_hybrids);

private:
    std::vector<bool> m_hybridAct;//binary to store which hybrids are activated
};

#endif // HDMI_H
