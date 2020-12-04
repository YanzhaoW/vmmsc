#include "fec.h"
#include <QMessageBox>

FEC::FEC():
    m_hdmiActs (HDMIS_PER_FEC),
    m_msg(0),
    m_socketHandler(0),
    numberOfRegisters(42),
    m_regNames ( new std::vector<const char*> (numberOfRegisters) ),
    m_reg ( new std::vector<unsigned long> (numberOfRegisters) ),
    m_chr ( new char[1000] ) //need for returning const char * in GetReg functions
{
    LoadDefault();

    m_socketHandler = new SocketHandler();
    m_fecConfigModule = new FECConfigModule(this);
    m_fecConfigModule->LoadSocket( GetSocketHandler() );
}


long FEC::GetID(){
    return  (m_reg->at(26) & 0x000000FF);
}

QString FEC::GetIP() {
    QHostAddress ip;
    ip.setAddress(m_reg->at(26));
    return ip.toString();
}

long FEC::GetIP_FEC(){
    return m_reg->at(26);
}

void FEC::SetIP_FEC(long ip) {
    (*m_reg)[26] = ip;
}

long FEC::GetIP_DAQ(){
    return m_reg->at(27);
}

void FEC::SetIP_DAQ(long ip) {
    (*m_reg)[27] = ip;
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
    for(int n=0; n< HDMIS_PER_FEC*VMMS_PER_HYBRID; n++) {
        config_error[n] = 0;
    }

    unsigned long first = 0;
    unsigned long ckbc = 0;
    for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
        if(GetHDMI(k)){
            if(k == 0) {
                first = this->m_hdmis[k].m_hybrids[0].GetReg("CKBC");
            }
            else {
                ckbc = this->m_hdmis[k].m_hybrids[0].GetReg("CKBC");
                if(first != ckbc) {
                    int ret = QMessageBox::warning(nullptr, tr("Hybrid BC clock"),
                                                   tr("Invalid hybrid BC clock setting! All hybrids on the same FEC have to have the same BC clock!"),
                                                   QMessageBox::Ok);
                    return;
                }
            }
        }
    }
    if(ckbc >= 2) {
        SetReg("bcclock_factor",ckbc-2);
    }

    m_fecConfigModule->SetMask();
    m_fecConfigModule->SetTriggerAcqConstants();
    for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
        if(GetHDMI(k)){
            m_fecConfigModule->ConfigTP(k);
            m_fecConfigModule->SetS6clocks(k);
            for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                if (m_hdmis[k].m_hybrids[0].GetVMM(m)){
                    //sleep(1);
                    bool result = m_fecConfigModule->SendConfig(k, m);
                    if(!result) {
                        config_error[k*VMMS_PER_HYBRID+m] = 1;
                    }

                }
            }

        }
    }
    bool iserror = false;
    QString message = "Configuration not loaded on FEC " + QString::number(GetID()) + ":\n";
    for (unsigned short k=0; k < VMMS_PER_HYBRID*HDMIS_PER_FEC; k++){
        if(config_error[k] == 1) {
            iserror = true;
            message = message + "\nVMM " + QString::number(k);
        }
    }
    if(iserror) {
        QMessageBox::warning(nullptr,"Config load error..", message,
                             QMessageBox::Ok,QMessageBox::NoButton);

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
    (*m_regNames)[0] ="tp_offset_first";         (*m_reg)[0] = 100;  //12 bit
    (*m_regNames)[1] ="tp_offset";               (*m_reg)[1] = 1000;  //12 bit
    (*m_regNames)[2] ="tp_latency";              (*m_reg)[2] = 71;  //8 bit
    (*m_regNames)[3] ="tp_number";               (*m_reg)[3] = 1;    //8 bit
    (*m_regNames)[4] ="bcclock_factor";          (*m_reg)[4] = 0;
    (*m_regNames)[5] ="not_used";                (*m_reg)[5] = 0;

    (*m_regNames)[6] ="fec_port";                (*m_reg)[6] = 6007;    //32 bit
    (*m_regNames)[7] ="daq_port";                (*m_reg)[7] = 6006;    //32 bit
    (*m_regNames)[8] ="vmmasic_port";            (*m_reg)[8] = 6603;    //32 bit
    (*m_regNames)[9] ="vmmapp_port";             (*m_reg)[9] = 6600;    //32 bit
    (*m_regNames)[10]="s6_port";                 (*m_reg)[10] = 6602;   //32 bit
    (*m_regNames)[11]="not_used";                (*m_reg)[11] = 0;
    (*m_regNames)[12]="not_used";                (*m_reg)[12] = 0;
    (*m_regNames)[13]="not_used";                (*m_reg)[13] = 0;

    (*m_regNames)[14]="debug_data_format";              (*m_reg)[14] = 0;   // 1bit: 0 for normal data format, 1 for debug format
    (*m_regNames)[15]="register_trigger_timestamp";     (*m_reg)[15] = 0;   //
    (*m_regNames)[16]="first_trigger_starts_acq";       (*m_reg)[16] = 0;   //

    (*m_regNames)[17]="sL0enaV";                 (*m_reg)[17] = 0;   //{"0", "1", "false", "true"}
    (*m_regNames)[18]="sL0ena";                  (*m_reg)[18] = 0;   //{"0", "1", "false", "true"}
    (*m_regNames)[19]="l0offset";                (*m_reg)[19] = 0;   //12 bit
    (*m_regNames)[20]="offset";                  (*m_reg)[20] = 0;   //12 bit
    (*m_regNames)[21]="rollover";                (*m_reg)[21] = 0;   //12 bit
    (*m_regNames)[22]="window";                  (*m_reg)[22] = 0;   //3 bit
    (*m_regNames)[23]="truncate";                (*m_reg)[23] = 0;   //6 bit
    (*m_regNames)[24]="nskip";                   (*m_reg)[24] = 0;   //7 bit
    (*m_regNames)[25]="sL0cktest";               (*m_reg)[25] = 0;   //{"0", "1", "false", "true"}
    (*m_regNames)[26]="ip_fec";                  (*m_reg)[26] = 0x0a000002;   //
    (*m_regNames)[27]="ip_daq";                  (*m_reg)[27] = 0x0a000003;   //

    (*m_regNames)[28]="i2c_port";                (*m_reg)[28] = 6604;   //32 bit
    (*m_regNames)[29]="fec_sys_port";            (*m_reg)[29] = 6023;   //32 bit

    (*m_regNames)[30]="latency_reset";           (*m_reg)[30] = 53;   //8 bit
    (*m_regNames)[31]="latency_data_max";             (*m_reg)[31] = 2560; //12 bit
    (*m_regNames)[32]="latency_data_error";           (*m_reg)[32] = 4;    //8 bit
    (*m_regNames)[33]="half_eye_width_0";                (*m_reg)[33] = 8;
    (*m_regNames)[34]="half_eye_width_1";                (*m_reg)[34] = 8;
    (*m_regNames)[35]="half_eye_width_2";                (*m_reg)[35] = 8;
    (*m_regNames)[36]="half_eye_width_3";                (*m_reg)[36] = 8;
    (*m_regNames)[37]="half_eye_width_4";                (*m_reg)[37] = 8;
    (*m_regNames)[38]="half_eye_width_5";                (*m_reg)[38] = 8;
    (*m_regNames)[39]="half_eye_width_6";                (*m_reg)[39] = 8;
    (*m_regNames)[40]="half_eye_width_7";                (*m_reg)[40] = 8;   //
    (*m_regNames)[41]="not_used";                (*m_reg)[41] = 0;   //

}

// ------------------------------------------------------------------------- //
unsigned short FEC::GetVMM(int hdmi_index, int vmm_index, std::string feature, int ch){
    unsigned short setting = m_hdmis[hdmi_index].m_hybrids[0].m_vmms[vmm_index].GetRegister(feature, ch);
    return setting;
}

bool FEC::SetVMM(int hdmi_index, int vmm_index, std::string feature, int value ,int ch){
    if(m_hdmis[hdmi_index].m_hybrids[0].m_vmms[vmm_index].SetRegi(feature, value, ch)){
        return true;
    }
    else return false;
}

bool FEC::SetVMM(int hdmi_index, int vmm_index, std::string feature, std::string value, int ch){
    if(m_hdmis[hdmi_index].m_hybrids[0].m_vmms[vmm_index].SetRegi(feature, value, ch)){
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
    unsigned long intValue = atol(val);
    if (reg < m_reg->size()){// && reg != 4 && reg != 5  && reg != 11 && reg != 12 && reg != 13){ //others are 32 bit
        if (intValue < 4294967296)found = true;
    }
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
    if(val) {
        sprintf(m_chr, "%d", 1);
    }
    else {
        sprintf(m_chr, "%d", 0);
    }
    if (CheckAllowedVal(regnum, m_chr)){
        Set(regnum,val); return true;
    }
    return false;
}

bool FEC::SetReg(const char *reg, unsigned long val){
    sprintf(m_chr, "%d", val); // convert int to char * to check if in allowed value list
    for (unsigned short i = 0; i < (*m_regNames).size(); i++ ){
        if(ConstCharStar_comp(reg,(*m_regNames)[i])){
            if (CheckAllowedVal(i, m_chr)){
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
    if (abs(regnum) < (*m_reg).size()){ //need to convert number to const char *
        std::stringstream str1;str1 << (*m_reg)[regnum];
        std::strcpy(m_chr,str1.str().c_str());
        return m_chr;
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
        delete[] m_chr;}
    m_chr = NULL;
}
