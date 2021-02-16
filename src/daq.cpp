#include "daq.h"
#include <QThread>

DAQ::DAQ():
    m_fecActs (FECS_PER_DAQ),
    m_chr ( new char[1000] ), //need for returning const char * in GetReg functions
    m_messageHandler(0)
{
    m_messageHandler = new MessageHandler();
    m_messageHandler->SetMessageSize(75);
    m_messageHandler->SetGUI(true);
    SetMessageHandler();

}

void DAQ::SendAll(bool useConfigCheck){

    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if ( GetFEC(j) ){
            m_fecs[j].SendAll(useConfigCheck);
        }
    }
}

bool DAQ::CheckHybridPos(unsigned short axis,  unsigned short position, int fec_index, int hybrid_index){
    bool check = true;
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if ( GetFEC(j) ){
            for (unsigned short k=0; k < HYBRIDS_PER_FEC; k++){
                if( m_fecs[j].GetHybrid(k) ){

                    if (!(fec_index==j && hybrid_index==k) ){
                        bool checkAxis   = m_fecs[j].m_hybrids[k].GetReg("axis") == axis;
                        bool checkPos = m_fecs[j].m_hybrids[k].GetReg("position") == position;
                        if(checkAxis && checkPos){
                            return false;
                        }
                        else check = true;

                    }
                }
            }

        }
    }
    return check;
}

int DAQ::CheckIP_FEC(long ip, int fec_index){
    long lastByte = ip & 0x000000FF;
    for (unsigned short n=0; n < FECS_PER_DAQ; n++){
        if ( GetFEC(n)){
            if(fec_index == -1) {
                if(m_fecs[n].GetIP_FEC()==ip) {
                    return n;
                }
            }
            else {
                if(n != fec_index) {
                    long lastByteTest = m_fecs[n].GetIP_FEC() & 0x000000FF;
                    if(lastByteTest==lastByte) {
                        return n;
                    }
                }
            }
        }
    }
    return -1;
}

int DAQ::CheckIP_DAQ(long ip){
    int cnt = 0;
    for (unsigned short n=0; n < FECS_PER_DAQ; n++){
        if ( GetFEC(n)){
            cnt++;
            if(m_fecs[n].GetIP_DAQ()==ip) {
                return n;
            }
        }
    }
    return -1;
}


void DAQ::ApplyVMMs(int fec_index, int hybrid_index, int vmm_index, bool isReset){
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if ( GetFEC(j) ){
            for (unsigned short k=0; k < HYBRIDS_PER_FEC; k++){
                if( m_fecs[j].GetHybrid(k) ){

                    for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                        if (m_fecs[j].m_hybrids[k].GetVMM(m)) {
                            if(isReset) {
                                m_fecs[j].SetVMM(k,m,"reset1", 1);
                                m_fecs[j].SetVMM(k,m,"reset2", 1);
                                m_fecs[j].m_fecConfigModule->SendConfig(k, m);
                                QThread::msleep(100);
                                m_fecs[j].SetVMM(k,m,"reset1", 0);
                                m_fecs[j].SetVMM(k,m,"reset2", 0);
                                m_fecs[j].m_fecConfigModule->SendConfig(k, m);
                            }
                            else {
                                if(!(fec_index==j && hybrid_index==k && vmm_index==m) ){
                                    (*m_fecs[j].m_hybrids[k].m_vmms[m].m_vmmSettings->m_globalReg1) = (*m_fecs[fec_index].m_hybrids[hybrid_index].m_vmms[vmm_index].m_vmmSettings->m_globalReg1);
                                }
                            }
                        }
                    }
                }
            }
            m_fecs[j].m_fecConfigModule->VMMLoadEmit();//dirty trick, does not work to emit signal on daq level
        }
    }

}


void DAQ::ApplyChannelSettingsVMMs(int fec_index, int hybrid_index, int vmm_index){
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if ( GetFEC(j) ){
            for (unsigned short k=0; k < HYBRIDS_PER_FEC; k++){
                if( m_fecs[j].GetHybrid(k) ){
                    for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                        if (m_fecs[j].m_hybrids[k].GetVMM(m)) {
                            if(!(fec_index==j && hybrid_index==k && vmm_index==m) ){
                                for (unsigned short ch=0; ch < 64; ch++){
                                    for(auto &setting: m_fecs[fec_index].m_hybrids[hybrid_index].m_vmms[vmm_index].m_vmmSettings->m_channels[ch].m_channel) {
                                        m_fecs[j].m_hybrids[k].m_vmms[m].SetRegi(setting.first, setting.second, ch);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            m_fecs[j].m_fecConfigModule->VMMUpdateChannelEmit();//dirty trick, does not work to emit signal on daq level
        }
    }

}


void DAQ::ApplyHybrids(int fec_index){
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if ( GetFEC(j) ){
            m_fecs[j].m_fecConfigModule->HybridLoadEmit();//dirty trick, does not work to emit signal on daq level
        }
    }

}


void DAQ::ACQHandler(bool on){
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if ( GetFEC(j)){
            if(on) m_fecs[j].m_fecConfigModule->ACQon(true);
            else if(!on) m_fecs[j].m_fecConfigModule->ACQoff(true);
            break;

        }
    }
}


void DAQ::SetMessageHandler(){
    for(int i=0; i<FECS_PER_DAQ; i++){
        m_fecs[i].LoadMessageHandler(GetMessageHandler() );
    }
}

bool DAQ::SetFEC(unsigned short FEC, bool OnOff){
    if (FEC < FECS_PER_DAQ) {m_fecActs[FEC] = OnOff; return true;}
    else {return false;}
}

bool DAQ::GetFEC(unsigned short FEC){
    if (FEC < FECS_PER_DAQ) {return m_fecActs[FEC];}
    else {return false;}
}



DAQ::~DAQ(){
    if( m_chr != NULL ){
        delete[] m_chr;}
    m_chr = NULL;
}
