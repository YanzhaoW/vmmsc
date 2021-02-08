#include "daq.h"
#include <QThread>

DAQ::DAQ():
    m_regNames ( new std::vector<const char*> (11) ),
    m_reg ( new std::vector<unsigned short> (3) ),
    m_regText ( new std::vector<std::string> (11) ),
    m_regVals ( new std::vector<std::vector<const char*> > (3, std::vector<const char*>() ) ),
    m_fecActs (FECS_PER_DAQ),
    m_chr ( new char[1000] ), //need for returning const char * in GetReg functions
    m_messageHandler(0)
{
    (*m_regNames)[0] ="ignore16";                (*m_reg)[0] = 0;   (*m_regVals)[0]={"0", "1", "false", "true"};
    (*m_regNames)[1] ="debug";                   (*m_reg)[1] = 0;   (*m_regVals)[1]={"0", "1", "false", "true"};
    (*m_regNames)[2] ="run_count";               (*m_reg)[2] = 20;  // lets allow 16 bit, so unsigned short is ok

    (*m_regNames)[3] ="mapping_file";            (*m_regText)[3] = "mini2_map.txt";
    (*m_regNames)[4] ="output_filename";         (*m_regText)[4] = "binary_dump.txt";
    (*m_regNames)[5] ="config_filename";         (*m_regText)[5] = "";
    (*m_regNames)[6] ="vmm_id_list";             (*m_regText)[6] = "";
    (*m_regNames)[7] ="ip_list";                 (*m_regText)[7] = "";
    (*m_regNames)[8] ="config_version";          (*m_regText)[8] = "";
    (*m_regNames)[9] ="comment";                 (*m_regText)[9] = "None";
    (*m_regNames)[10] ="output_path";            (*m_regText)[10] = "";

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

bool DAQ::CheckHybridPos(unsigned short axis,  unsigned short position, int fec_index, int hdmi_index, int hybrid_index){
    bool check = true;
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if ( GetFEC(j) ){
            for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
                if( m_fecs[j].GetHDMI(k) ){
                    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                        if (m_fecs[j].m_hdmis[k].GetHybrid(l) && !(fec_index==j && hdmi_index==k && hybrid_index==l) ){
                            bool checkAxis   = m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("axis") == axis;
                            bool checkPos = m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("position") == position;
                            if(checkAxis && checkPos){
                                return false;
                            }
                            else check = true;

                        }
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


void DAQ::ApplyVMMs(int fec_index, int hdmi_index, int hybrid_index, int vmm_index, bool isReset){
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if ( GetFEC(j) ){
            for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
                if( m_fecs[j].GetHDMI(k) ){
                    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                        if (m_fecs[j].m_hdmis[k].GetHybrid(l)){
                            for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                                if (m_fecs[j].m_hdmis[k].m_hybrids[l].GetVMM(m)) {
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
                                        if(!(fec_index==j && hdmi_index==k && hybrid_index==l &&vmm_index==m) ){
                                            (*m_fecs[j].m_hdmis[k].m_hybrids[l].m_vmms[m].m_vmmSettings->m_globalReg1) = (*m_fecs[fec_index].m_hdmis[hdmi_index].m_hybrids[hybrid_index].m_vmms[vmm_index].m_vmmSettings->m_globalReg1);
                                        }
                                    }
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


void DAQ::ApplyChannelSettingsVMMs(int fec_index, int hdmi_index, int hybrid_index, int vmm_index){
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if ( GetFEC(j) ){
            for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
                if( m_fecs[j].GetHDMI(k) ){
                    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                        if (m_fecs[j].m_hdmis[k].GetHybrid(l)){
                            for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                                if (m_fecs[j].m_hdmis[k].m_hybrids[l].GetVMM(m)) {
                                    if(!(fec_index==j && hdmi_index==k && hybrid_index==l &&vmm_index==m) ){
                                        //(*m_fecs[j].m_hdmis[k].m_hybrids[l].m_vmms[m].m_vmmSettings->m_channels) = (*m_fecs[fec_index].m_hdmis[hdmi_index].m_hybrids[hybrid_index].m_vmms[vmm_index].m_vmmSettings->m_channels);
                                        for (unsigned short ch=0; ch < 64; ch++){
                                            for(auto &setting: m_fecs[fec_index].m_hdmis[hdmi_index].m_hybrids[hybrid_index].m_vmms[vmm_index].m_vmmSettings->m_channels[ch].m_channel) {
                                               m_fecs[j].m_hdmis[k].m_hybrids[l].m_vmms[m].SetRegi(setting.first, setting.second, ch);
                                            }
                                        }
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


void DAQ::ApplyHybrids(int fec_index, int hdmi_index, int hybrid_index){
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

bool DAQ::Set(unsigned short reg, unsigned short val){

    if (reg < (*m_reg).size() ) { (*m_reg)[reg] = val; return true;}
    else {std::cout << "ERROR register " << reg << " does not exist." << std::endl;return false;}
}

bool DAQ::SetText(unsigned short reg, const char * text){
    std::stringstream str;
    str << text;
    if (reg < (*m_regNames).size() &&  reg >= (*m_reg).size() ) { (*m_regText)[reg] = str.str(); std::cout << "Text register " << reg << " set to " << text << " ." << std::endl; return true;}
    else {std::cout << "ERROR register " << reg << " does not exist." << std::endl;return false;}
}

bool DAQ::SetReg(const char *reg, bool val){ //set a register, name and bool given
    const char *chr =  val ? "true" : "false";// convert bool to const char * to check if in allowed value list
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            if (CheckAllowedVal(i, chr)){
                Set(i,val); return true;
            }
        }
    }
    return false;
}

bool DAQ::SetReg(int regnum, bool val){
    const char *chr =  val ? "1" : "0";// convert bool to const char * to check if in allowed value list
    if (CheckAllowedVal(regnum, chr)){
        Set(regnum,val); return true;
    }
    return false;
}

bool DAQ::SetReg(const char *reg, int val){
    std::stringstream str1;str1 << val;const char * chr = str1.str().c_str(); // convert int to const char * to check if in allowed value list
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            if (CheckAllowedVal(i, chr) && i < (*m_reg).size()){ //numbers only allowed for registers 0-2
                Set(i,val); return true;
            }
        }
    }
    return false;
}

bool DAQ::SetReg(int regnum, int val){
    std::stringstream str1;str1 << val;const char *chr = str1.str().c_str(); // convert int to const char * to check if in allowed value list
    if (CheckAllowedVal(regnum, chr) && abs(regnum) < (*m_reg).size()){ //numbers only allowed for registers 0-2
        Set(regnum,val); return true;
    }
    return false;
}

bool DAQ::SetReg(const char *reg, const char *val){
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            if (CheckAllowedVal(i, val)){
                if (i < (*m_reg).size()){Set(i,FindVecEntry(i,val)); return true;} // numbers
                else if (i < (*m_regNames).size() &&  i >= (*m_reg).size() ) {SetText(i,val); return true;} //text
            }
        }
    }
    return false;
}

bool DAQ::SetReg(int regnum, const char *val){
    if (CheckAllowedVal(regnum, val)){
        if (abs(regnum) < (*m_reg).size()){Set(regnum,FindVecEntry(regnum,val)); return true;} // numbers
        else if (abs(regnum) < (*m_regNames).size() &&  abs(regnum) >= (*m_reg).size() ) {SetText(regnum,val); return true;} //text
    }
    return false;
}

const char * DAQ::GetReg(const char *reg){
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            const char *chr = GetReg((int)i);
            return chr;
        }
    }
    return "ERROR";
}

const char * DAQ::GetReg(int regnum){
    if (regnum == 0){// give the name of the value
        if ((*m_reg)[0] == 0 || (*m_reg)[0] == 2 ) return "false";
        else if ((*m_reg)[0] == 1 || (*m_reg)[0] == 3) return "true";
        else return "ERROR";
    }
    else if (abs(regnum) < (*m_reg).size() && regnum != 4){ //need to convert number to const char *
        std::stringstream str1;str1 << (*m_reg)[regnum];
        std::strcpy(m_chr,str1.str().c_str());
        return m_chr;
    }
    else if (abs(regnum) < (*m_regNames).size() &&  abs(regnum) >= (*m_reg).size() ) { //directly return text
        std::strcpy(m_chr,(m_regText->at(regnum)).c_str());
        return m_chr;
    }
    else return "ERROR";
}

unsigned short DAQ::GetRegVal(int regnum){
    if (abs(regnum) < (*m_reg).size() ){ //only reg 0-2
        return (*m_reg)[regnum];
    }
    else return -1;
}

unsigned short DAQ::GetRegVal(const char *reg){
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            return GetRegVal((int)i);
        }
    }
    return -1;
}

const char * DAQ::GetRegName(unsigned short regnum){
    if(regnum < (*m_regNames).size()){return (*m_regNames)[regnum];}
    return "failure";
}

unsigned short DAQ::GetRegNumber(const char *reg){
    unsigned short regnum = -1;
    for (unsigned short i = 0; i < (*m_regNames).size(); i++)
    {
        if (reg == (*m_regNames)[i]) {regnum = i;}
    }
    return regnum;
}

unsigned short DAQ::GetRegSize(){
    return m_regNames->size();
}

bool DAQ::CheckAllowedVal(unsigned short reg, const char *val){
    bool found = false;
    std::stringstream strValue;
    strValue << val;
    int intValue;
    strValue >> intValue;
    if (reg == 0 || reg == 1){ //ignore16 or debug
        if (ConstCharStar_comp(val,"0") || ConstCharStar_comp(val,"1") || ConstCharStar_comp(val,"true") || ConstCharStar_comp(val,"false") ) found = true;
    }
    if (reg == 2 ){ // 16 bit values allowed for run_count
        if (intValue < 65536)found = true;
    }
    else if (reg < (*m_regNames).size() && reg != 0 && reg != 1 && reg != 2 ){ //others are text, so can be anything
        found = true;
    }
    return found;
}

unsigned short DAQ::FindVecEntry(unsigned short regnum, const char *val){
    unsigned short pos = -1;

    if (regnum == 0 || regnum == 1) {// need to compare the strings at the const char * addresses
        if (ConstCharStar_comp(val,"0")) pos = 0;
        else if (ConstCharStar_comp(val,"1")) pos = 1;
        else if (ConstCharStar_comp(val,"false")) pos = 2;
        else if (ConstCharStar_comp(val,"true")) pos = 3;
    }
    else {
        std::stringstream strValue;
        strValue << val;
        unsigned short intValue;
        strValue >> intValue;
        pos = intValue;
    }
    return pos;
}

bool DAQ::ConstCharStar_comp(const char *ccs1, const char *ccs2){
    std::stringstream str1;
    str1 << ccs1;
    std::stringstream str2;
    str2 << ccs2;
    if (str1.str() == str2.str()) {return true;}
    else {return false;}
}

DAQ::~DAQ(){
    if( m_regNames != NULL ){
        delete m_regNames;}
    m_regNames = NULL;

    if( m_reg != NULL ){
        delete m_reg;}
    m_reg = NULL;

    if( m_regText != NULL ){
        delete m_regText;}
    m_regText = NULL;

    if( m_regVals != NULL ){
        delete m_regVals;}
    m_regVals = NULL;

    if( m_chr != NULL ){
        delete[] m_chr;}
    m_chr = NULL;
}
