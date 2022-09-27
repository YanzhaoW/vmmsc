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
    for(int n=0; n<FECS_PER_DAQ;n++) {
        m_fecs[n].SetIndex(n);
    }

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


void DAQ::ACQHandler(bool on){
    //Assister (1) and SRS (2) have one IP per card
    if(g_slow_control != 0){
        QStringList subnetList;
        QList<int> fecList;
        for (unsigned short j=0; j < FECS_PER_DAQ; j++){
            if ( GetFEC(j)){
                QString ip = m_fecs[j].GetIP();
                QStringList list = ip.split(".");
                QString subnet = list[0] + "." + list[1] + "." + list[2];
                if(!subnetList.contains(subnet)) {
                    subnetList.append(subnet);
                    fecList.append(j);
                }

            }
        }
        for(int n=0; n<subnetList.size();n++) {
            //std::cout << "FEC ID " << fecList[n] << " subnet " << subnetList[n].toStdString() << std::endl;
            if(on) m_fecs[fecList[n]].m_fecConfigModule->ACQon(true);
            else if(!on) m_fecs[fecList[n]].m_fecConfigModule->ACQoff(true);

        }
    }
    //Master (0) has just one IP
    else {
        for (unsigned short j=0; j < FECS_PER_DAQ; j++){
            if ( GetFEC(j)){
                if(on) m_fecs[j].m_fecConfigModule->ACQon();
                else if(!on) m_fecs[j].m_fecConfigModule->ACQoff();
            }
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
