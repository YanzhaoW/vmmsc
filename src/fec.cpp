#include "fec.h"
//#include <unistd.h>

FEC::FEC():
    m_hdmiActs (HDMIS_PER_FEC),
    m_msg(0),
    m_socketHandler(0),
    numberOfRegisters(42),
    m_regNames ( new std::vector<const char*> (numberOfRegisters) ),
    m_reg ( new std::vector<unsigned long> (numberOfRegisters) ),
    m_chr ( new char ) //need for returning const char * in GetReg functions
{
    LoadDefault();

    m_socketHandler = new SocketHandler();
    m_fecConfigModule = new FECConfigModule(this);
    m_fecConfigModule->LoadSocket( GetSocketHandler() );
}

QString FEC::GetIP(){
    QString ip;
    ip = QString("%1.%2.%3.%4").arg(m_reg->at(26)).arg(m_reg->at(27)).arg(m_reg->at(28)).arg(m_reg->at(29));
    return ip;
}

int FEC::GetIP_id(){
    return m_reg->at(29);
}

void FEC::SetFirmwareVersion(QString version)
{
    m_firmwareVersion = version;
}

QString FEC::GetFirmwareVersion()
{
    return m_firmwareVersion;
}

// ------------------------------------------------------------------------ //
void FEC::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
    m_fecConfigModule->LoadMessageHandler( m );
    m_socketHandler->LoadMessageHandler( m );
}
// ------------------------------------------------------------------------- //

void FEC::SendAll(){
    /// function to send all configurations to fec, hybrid and vmm

    m_fecConfigModule->SetMask();
    m_fecConfigModule->SetReadoutMode();
    for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
        if(GetHDMI(k)){
            for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                if (m_hdmis[k].GetHybrid(l)){
                    m_fecConfigModule->ConfigTP(k, l);
                    m_fecConfigModule->SetS6clocks(k, l);
                    m_fecConfigModule->SetS6Resets(k, l);
                    for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                        if (m_hdmis[k].m_hybrids[l].GetVMM(m)){
                            //sleep(1);
                            m_fecConfigModule->SendConfig(k, l, m);
                            m_fecConfigModule->SetEventHeaders(k, l, m);
                            m_fecConfigModule->SetTriggerAcqConstants(k, l, m);
                            m_fecConfigModule->SetTriggeredMode(k, l, m);

                        }
                    }
                }
            }
        }
    }



}

// ------------------------------------------------------------------------- //
quint16 FEC::GetChMap(){
    QString chMapString = "0000000000000000";
    bool ok;
    for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
        if(GetHDMI(k)){
            for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                if (m_hdmis[k].GetHybrid(l)){
                    for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                        if (m_hdmis[k].m_hybrids[l].GetVMM(m)){
                            chMapString.replace(15-( k*2+m ) , 1 , QString("1") );
                        }
                    }
                }
            }
        }
    }
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);
    return chMap;
}

// ------------------------------------------------------------------------- //

void FEC::LoadDefault(){
    (*m_regNames)[0] ="tp_delay";                (*m_reg)[0] = 81;      //register 4: 32 bit //max 50000 by gui?
    (*m_regNames)[1] ="readout_cycle";          (*m_reg)[1] = 4094;  //32 bit //max 7FFFFFFF = 31 bit?
    (*m_regNames)[2] ="acq_sync";                (*m_reg)[2] = 100;     //32 bit
    (*m_regNames)[3] ="acq_window";              (*m_reg)[3] = 3700;    //32 bit
    (*m_regNames)[4] ="run_mode";                (*m_reg)[4] = 0;       //{"0", "1", "pulser" (=0), "external" (=1)};
    (*m_regNames)[5] ="bcid_reset";              (*m_reg)[5] = 0;       //max 65535 (16 bit)
    (*m_regNames)[6] ="fec_port";                (*m_reg)[6] = 6007;    //32 bit
    (*m_regNames)[7] ="daq_port";                (*m_reg)[7] = 6006;    //32 bit
    (*m_regNames)[8] ="vmmasic_port";            (*m_reg)[8] = 6603;    //32 bit
    (*m_regNames)[9] ="vmmapp_port";             (*m_reg)[9] = 6600;    //32 bit
    (*m_regNames)[10]="s6_port";                 (*m_reg)[10] = 6602;   //32 bit
    (*m_regNames)[11]="evbld_mode";              (*m_reg)[11] = 0;   //{"Frame_Cnt", "Global_Frame_Cnt", "Timestamp+Frame_Cnt" }
    (*m_regNames)[12]="evbld_infodata";          (*m_reg)[12] = 0;   //{"HINFO+Datalength", "Trigger_Cnt+Datalength", "Trigger_Cnt", "Trigger_Timestamp+Datalength", "Trigger_Timestamp", "Trigger_Cnt+Trigger_Timestamp"}
    (*m_regNames)[13]="highres";                 (*m_reg)[13] = 0;   //{"0", "1", "false", "true"};

    (*m_regNames)[14]="triggermode";             (*m_reg)[14] = 0;   // 0 for external and 1 for pulser
    (*m_regNames)[15]="res2";                    (*m_reg)[15] = 0;   //
    (*m_regNames)[16]="res3";                    (*m_reg)[16] = 0;   //

    (*m_regNames)[17]="sL0enaV";                  (*m_reg)[17] = 0;   //{"0", "1", "false", "true"}
    (*m_regNames)[18]="sL0ena";                   (*m_reg)[18] = 0;   //{"0", "1", "false", "true"}
    (*m_regNames)[19]="l0offset";                 (*m_reg)[19] = 0;   //12 bit
    (*m_regNames)[20]="offset";                   (*m_reg)[20] = 0;   //12 bit
    (*m_regNames)[21]="rollover";                 (*m_reg)[21] = 0;   //12 bit
    (*m_regNames)[22]="window";                   (*m_reg)[22] = 0;   //3 bit
    (*m_regNames)[23]="truncate";                 (*m_reg)[23] = 0;   //6 bit
    (*m_regNames)[24]="nskip";                    (*m_reg)[24] = 0;   //7 bit
    (*m_regNames)[25]="sL0cktest";                (*m_reg)[25] = 0;   //{"0", "1", "false", "true"}
    (*m_regNames)[26]="ip1";                      (*m_reg)[26] = 10;   //
    (*m_regNames)[27]="ip2";                      (*m_reg)[27] = 0;   //
    (*m_regNames)[28]="ip3";                      (*m_reg)[28] = 0;   //
    (*m_regNames)[29]="ip4";                      (*m_reg)[29] = 2;   //

    (*m_regNames)[30]="i2c_port";                 (*m_reg)[30] = 6604;   //32 bit
    (*m_regNames)[31]="fec_sys_port";            (*m_reg)[31] = 6023;   //32 bit
    (*m_regNames)[32]="triggered_mode";          (*m_reg)[32] = 0;   //register 11: {"0", "1", "disabled", "enabled"} - 1 bit
    (*m_regNames)[33]="time_offset_triggerperiod";(*m_reg)[33] = 10;   //register 11: {0-31} - 5 bit
    (*m_regNames)[34]="time_offset_BCID";         (*m_reg)[34] = 0;   //register 11: {0-4095} * BCCLOCK_PERIOD - 12 bit
    (*m_regNames)[35]="time_window_BCID";         (*m_reg)[35] = 0xfff;   //register 11: {0-4095} * BCCLOCK_PERIOD - 12 bit
    (*m_regNames)[36]="trigger_pulse_delay";        (*m_reg)[36] = 0;   //register 15: {0-255} * BCCLOCK_PERIOD - 8 bit, bit 8-15
    (*m_regNames)[37]="clear_S6_fifo";             (*m_reg)[37] = 0; //{"0", "1", "false", "true"};
    (*m_regNames)[38]="acceptance_window";         (*m_reg)[38] = 0; //{"0", "1", "false", "true"};
    (*m_regNames)[39]="globalCKBC";             (*m_reg)[39] = 3; //{"160", "160inv", "80", "40", "20", "10", "5", "2.5"};
    (*m_regNames)[40]="ts_ext_trg";             (*m_reg)[40] = 0; //{"0", "1", "false", "true"};
    (*m_regNames)[41]="open_fec_wr_fifo_outside_acq_win";             (*m_reg)[41] = 1; //{"0", "1", "false", "true"};
}

// ------------------------------------------------------------------------- //
unsigned short FEC::GetVMM(int hdmi_index, int hybrid_index, int vmm_index, std::string feature, int ch){
    unsigned short setting = m_hdmis[hdmi_index].m_hybrids[hybrid_index].m_vmms[vmm_index].GetRegister(feature, ch);
    return setting;
}

bool FEC::SetVMM(int hdmi_index, int hybrid_index, int vmm_index, std::string feature, int value ,int ch){
    if(m_hdmis[hdmi_index].m_hybrids[hybrid_index].m_vmms[vmm_index].SetRegi(feature, value, ch)){
        return true;
    }
    else return false;
}

bool FEC::SetVMM(int hdmi_index, int hybrid_index, int vmm_index, std::string feature, std::string value, int ch){
    if(m_hdmis[hdmi_index].m_hybrids[hybrid_index].m_vmms[vmm_index].SetRegi(feature, value, ch)){
        return true;
    }
    else return false;
}


bool FEC::SetHDMI(unsigned short hdmi, bool OnOff){
    if (hdmi < HDMIS_PER_FEC) {m_hdmiActs[hdmi] = OnOff; return true;}
    else {return false;}
}

bool FEC::GetHDMI(unsigned short hdmi){
    if (hdmi < HDMIS_PER_FEC) {return m_hdmiActs[hdmi];}
    else {return false;}
}

bool FEC::Set(unsigned short reg, unsigned long val){

    if (reg < m_reg->size() ) { (*m_reg)[reg] = val; std::cout << "Register " << reg << " set to " << val << " ." << std::endl; return true;}
    else {std::cout << "ERROR register " << reg << " does not exist." << std::endl;return false;}
}

bool FEC::CheckAllowedVal(unsigned short reg, const char *val){
    bool found = false;

    std::stringstream strValue;
    strValue << val;
    unsigned long intValue;
    strValue >> intValue;
    std::cout << "intValue: " << intValue << std::endl;
    if (reg == 4){ //run_mode
        if (ConstCharStar_comp(val,"0") || ConstCharStar_comp(val,"1") || ConstCharStar_comp(val,"pulser") || ConstCharStar_comp(val,"external") ) found = true;
    }
    if (reg == 5 ){ // 16 bit values allowed for bcid_reset
        if (intValue < 65536)found = true;
    }
    else if (reg < m_reg->size()){// && reg != 4 && reg != 5  && reg != 11 && reg != 12 && reg != 13){ //others are 32 bit
        if (intValue < 4294967296)found = true;
    }
    //if (reg == 11){ //evbld_mode
    //    if (ConstCharStar_comp(val,"Frame_Cnt") || ConstCharStar_comp(val,"Global_Frame_Cnt") || ConstCharStar_comp(val,"Timestamp+Frame_Cnt")) found = true;
    //}
    //if (reg == 12){ //evbld_info
    //    if (ConstCharStar_comp(val,"HINFO+Datalength") || ConstCharStar_comp(val,"Trigger_Cnt+Datalength") || ConstCharStar_comp(val,"Trigger_Cnt") || ConstCharStar_comp(val,"Trigger_Timestamp+Datalength") || ConstCharStar_comp(val,"Trigger_Timestamp") || ConstCharStar_comp(val,"Trigger_Cnt+Trigger_Timestamp") ) found = true;
    //}
    //if (reg == 13){ //timeStampHighRes
    //    if (ConstCharStar_comp(val,"0") || ConstCharStar_comp(val,"1") || ConstCharStar_comp(val,"false") || ConstCharStar_comp(val,"true") ) found = true;
    //}
    return found;
}

bool FEC::SetReg(const char *reg, bool val){ //set a register, name and bool given
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

bool FEC::SetReg(int regnum, bool val){
    const char *chr =  val ? "1" : "0";// convert bool to const char * to check if in allowed value list
    if (CheckAllowedVal(regnum, chr)){
        Set(regnum,val); return true;
    }
    return false;
}

bool FEC::SetReg(const char *reg, unsigned long val){
    std::stringstream str1;str1 << val;const char * chr = str1.str().c_str(); // convert int to const char * to check if in allowed value list
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            if (CheckAllowedVal(i, chr)){
                Set(i,val); return true;
            }
        }
    }
    return false;
}

bool FEC::SetReg(int regnum, unsigned long val){
    std::stringstream str1;str1 << val;const char *chr = str1.str().c_str(); // convert int to const char * to check if in allowed value list
    if (CheckAllowedVal(regnum, chr)){
        Set(regnum,val); return true;
    }
    return false;
}

bool FEC::SetReg(const char *reg, const char *val){
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            if (CheckAllowedVal(i, val)){
                Set(i,FindVecEntry(i,val)); return true;
            }
        }
    }
    return false;
}

bool FEC::SetReg(int regnum, const char *val){
    if (CheckAllowedVal(regnum, val)){
        Set(regnum,FindVecEntry(regnum,val)); return true;
    }
    return false;
}

unsigned long FEC::FindVecEntry(unsigned short regnum, const char *val){
    unsigned long pos = 4294967296;

    if (regnum == 4) {// need to compare the strings at the const char * addresses
        if (ConstCharStar_comp(val,"0")) pos = 0;
        else if (ConstCharStar_comp(val,"1")) pos = 1;
        else if (ConstCharStar_comp(val,"pulser")) pos = 2;
        else if (ConstCharStar_comp(val,"external")) pos = 3;
    }
    else if (regnum == 11) {// need to compare the strings at the const char * addresses
        if (ConstCharStar_comp(val,"Frame_Cnt")) pos = 0;
        else if (ConstCharStar_comp(val,"Global_Frame_Cnt")) pos = 1;
        else if (ConstCharStar_comp(val,"Timestamp+Frame_Cnt")) pos = 2;
    }
    else if (regnum == 12) {// need to compare the strings at the const char * addresses
        if (ConstCharStar_comp(val,"HINFO+Datalength")) pos = 0;
        else if (ConstCharStar_comp(val,"Trigger_Cnt+Datalength")) pos = 1;
        else if (ConstCharStar_comp(val,"Trigger_Cnt")) pos = 2;
        else if (ConstCharStar_comp(val,"Trigger_Timestamp+Datalength")) pos = 3;
        else if (ConstCharStar_comp(val,"Trigger_Timestamp")) pos = 4;
        else if (ConstCharStar_comp(val,"Trigger_Cnt+Trigger_Timestamp")) pos = 5;
    }
    else if (regnum == 13) {// need to compare the strings at the const char * addresses
        if (ConstCharStar_comp(val,"0")) pos = 0;
        else if (ConstCharStar_comp(val,"1")) pos = 1;
        else if (ConstCharStar_comp(val,"false")) pos = 2;
        else if (ConstCharStar_comp(val,"true")) pos = 3;
    }
    else {
        std::stringstream strValue;
        strValue << val;
        unsigned long intValue;
        strValue >> intValue;
        pos = intValue;
    }
    return pos;
}

const char * FEC::GetReg(const char *reg){
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            const char *chr = GetReg((int)i);
            return chr;
        }
    }
    return "ERROR";
}

const char * FEC::GetReg(int regnum){
    if (regnum == 4){// give the name of the value
        if ((*m_reg)[4] == 0 || (*m_reg)[4] == 2 ) return "pulser";
        else if ((*m_reg)[4] == 1 || (*m_reg)[4] == 3) return "external";
        else return "ERROR";
    }
    else if (abs(regnum) < (*m_reg).size() && regnum != 4 && regnum != 11 && regnum != 12 && regnum != 13){ //need to convert number to const char *
        std::stringstream str1;str1 << (*m_reg)[regnum];
        std::strcpy(m_chr,str1.str().c_str());
        return m_chr;
    }
    else if (regnum == 11){
        if ((*m_reg)[11] == 0) return "Frame_Cnt";
        else if ((*m_reg)[11] == 1) return "Global_Frame_Cnt";
        else if ((*m_reg)[11] == 2) return "Timestamp+Frame_Cnt";
        else return "ERROR";
    }
    else if (regnum == 12){
        if ((*m_reg)[12] == 0) return "HINFO+Datalength";
        else if ((*m_reg)[12] == 1) return "Trigger_Cnt+Datalength";
        else if ((*m_reg)[12] == 2) return "Trigger_Cnt";
        else if ((*m_reg)[12] == 3) return "Trigger_Timestamp+Datalength";
        else if ((*m_reg)[12] == 4) return "Trigger_Timestamp";
        else if ((*m_reg)[12] == 5) return "Trigger_Cnt+Trigger_Timestamp";
        else return "ERROR";
    }
    else if (regnum == 13){// give the name of the value
        if ((*m_reg)[13] == 0 || (*m_reg)[13] == 2 ) return "false";
        else if ((*m_reg)[13] == 1 || (*m_reg)[13] == 3) return "true";
        else return "ERROR";
    }
    else return "ERROR";
}


unsigned long FEC::GetRegVal(int regnum){
    if (abs(regnum) < (*m_reg).size() ){
        return (*m_reg)[regnum];
    }
    else return -1;
}

unsigned long FEC::GetRegVal(const char *reg){
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            return GetRegVal((int)i);
            std::cout<<"Position: "<<i<<std::endl;
        }
    }
    return -1;
}

const char * FEC::GetRegName(unsigned short regnum){
    if(regnum < (*m_regNames).size()){return (*m_regNames)[regnum];}
    return "failure";
}

unsigned short FEC::GetRegNumber(const char *reg){
    unsigned short regnum = -1;
    for (unsigned short i = 0; i < (*m_regNames).size(); i++)
    {
        if (reg == (*m_regNames)[i]) {regnum = i;}
    }
    return regnum;
}

unsigned short FEC::GetRegSize(){
    return m_reg->size();
}

bool FEC::ConstCharStar_comp(const char *ccs1, const char *ccs2){
    std::stringstream str1;
    str1 << ccs1;
    std::stringstream str2;
    str2 << ccs2;
    if (str1.str() == str2.str()) {return true;}
    else {return false;}
}





FEC::~FEC()
{
    if( m_regNames != NULL ){
        delete m_regNames;}
    m_regNames = NULL;

    if( m_reg != NULL ){
        delete m_reg;}
    m_reg = NULL;

    if( m_chr != NULL ){
        delete m_chr;}
    m_chr = NULL;
}
