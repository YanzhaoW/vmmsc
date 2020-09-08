#include "hybrid.h"

Hybrid::Hybrid(): m_vmmActs (VMMS_PER_HYBRID)
{
    LoadDefault();
    m_vmmActs[0] = 1;
    m_vmmActs[1] = 1;
    m_posX = true;
    m_posNo = -1;
    m_art = 0;


}

void Hybrid::LoadDefault(){
    m_hybrid = {{"Xaxis",0}, {"position", 65535}, {"CKTK",0}, {"CKBC",0}, {"CKBC_duty",0}, {"CKBC_skew",0}, {"CKDT",1}, {"TK_Pulses",2},{"period",4094}, {"TP_skew", 0}, {"TP_width", 0}, {"TP_pol", 0}};
    SetReg("CKBC", (std::string)"80");
    SetReg("CKBC_duty", (std::string)"18.725 ns high (standard)");
    SetReg("CKDT", (std::string)"80");
}

bool Hybrid::SetVMM(unsigned short vmm, bool OnOff){
    if (vmm < VMMS_PER_HYBRID) {m_vmmActs[vmm] = OnOff; return true;}
    else {return false;}
}

void Hybrid::SetART(bool OnOff){
    m_art = OnOff;
}

bool Hybrid::GetVMM(unsigned short vmm){
    if (vmm < VMMS_PER_HYBRID) {return m_vmmActs[vmm];}
    else {return false;}
}

bool Hybrid::GetART(){
    return m_art;
}

void Hybrid::SetPosX(bool IsX){
    m_posX = IsX;
}

bool Hybrid::SetPosNo(unsigned short PosNr){
    if(false) return false; //TODO: check if this number is already given to another hybrid on the same axis
    else {m_posNo=PosNr;return true;}
}

bool Hybrid::GetPosX(){
    return m_posX;
}

unsigned short Hybrid::GetPosNo(){
    return m_posNo;
}


bool Hybrid::SetReg(std::string feature, std::string val){
    if (SetRegister(feature, val)) return true;
     else{
        std::cout << "ERROR register " << feature << " or entered value " << val << " for Hybrid does not exist." << std::endl;
         return false;
     }
}

bool Hybrid::SetReg(std::string feature, int val){
    std::string value =std::to_string(val);
    if (SetRegister(feature, value)) return true;
    else{
       std::cout << "ERROR register " << feature << " or entered value " << val << " for Hybrid does not exist." << std::endl;
        return false;
    }
}


bool Hybrid::SetRegister(std::string feature, std::string value){
//{"Xaxis",0}, {"position", 65534}, {"CKTK",0}, {"CKBC",0}, {CKBC_duty,3}, {"CKBC_skew",0}, {"CKDT",1} ,{"TK_Pulses",2},{"period",4094}
    typedef std::map<std::string, unsigned short> InMap;
    typedef std::pair<std::string, unsigned short> BiPair;
    if(m_hybrid.find(feature)==m_hybrid.end()) return false;
    else{
        if(feature=="Xaxis"){
            if(value == "0" || value == "false"){
                m_hybrid[feature] = 0;
                return true;
            }
            else if(value == "1" || value == "true"){
                m_hybrid[feature] = 1;
                return true;
            }
           else return false;
        }
        if(feature=="position"){
           if(std::stoi(value)>=0 && std::stoi(value)<=65534){
                 m_hybrid[feature] = std::stoi(value);
                 return true;
           }
           else return false;
        }

        if(feature=="CKTK"){
            if(value == "0"){
                m_hybrid[feature] = 0;
                return true;
            }
            else if(value == "1" || value == "12.5"){
                m_hybrid[feature] = 1;
                return true;
            }
            else if(value == "2" || value == "25"){
                m_hybrid[feature] = 2;
                return true;
            }
            else return false;
        }

        if(feature=="CKBC"){
            InMap m_val;
            std::string v_val[8] = {"160", "160inv", "80", "40", "20", "10", "5", "2.5"};
            for(unsigned int i=0 ; i<sizeof(v_val)/sizeof(*v_val); i++){
                unsigned short bin_val=i;
                m_val.insert(BiPair(v_val[i], bin_val));
                m_val.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_val.find(value)!=m_val.end()){
              m_hybrid[feature] = m_val[value];
              return true;
            }
            else return false;
        }

        if(feature=="CKBC_duty"){
            InMap m_val;
            std::string v_val[4] = {"50 % high", "75 % high", "25 % high", "18.725 ns high (standard)"};
            for(unsigned int i=0 ; i<sizeof(v_val)/sizeof(*v_val); i++){
                unsigned short bin_val=i;
                m_val.insert(BiPair(v_val[i], bin_val));
                m_val.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_val.find(value)!=m_val.end()){
              m_hybrid[feature] = m_val[value];
              return true;
            }
            else return false;
        }

        if(feature=="CKBC_skew"){
            InMap m_val;
            std::string v_val[4] = {"0", "6.26", "12.52", "18.78"};
            for(unsigned int i=0 ; i<sizeof(v_val)/sizeof(*v_val); i++){
                unsigned short bin_val=i;
                m_val.insert(BiPair(v_val[i], bin_val));
                m_val.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_val.find(value)!=m_val.end()){
              m_hybrid[feature] = m_val[value];
              return true;
            }
            else return false;
        }

        if(feature=="CKDT"){
            InMap m_val;
            std::string v_val[5] = {"10", "40", "80", "160", "160 (will be 200)"};
            for(unsigned int i=0 ; i<sizeof(v_val)/sizeof(*v_val); i++){
                unsigned short bin_val=i;
                m_val.insert(BiPair(v_val[i], bin_val));
                m_val.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_val.find(value)!=m_val.end()){
              m_hybrid[feature] = m_val[value];
              return true;
            }
            else return false;
        }

        else if(feature == "TK_Pulses" ){
            InMap m_val;
            for(unsigned short i=0 ; i<=7; i++){
                m_val.insert(BiPair(std::to_string(i), i));
            }
            if(m_val.find(value)!=m_val.end()){
               m_hybrid[feature] = m_val[value];
              return true;
            }
            return false;
        }

        else if(feature == "period" ){
            InMap m_val;
            for(unsigned short i=0 ; i<=4096; i++){
                m_val.insert(BiPair(std::to_string(i), i));
            }
            if(m_val.find(value)!=m_val.end()){
               m_hybrid[feature] = m_val[value];
              return true;
            }
            return false;
        }
        else if(feature=="TP_skew"){
            InMap m_val;
            std::string v_val[18] = {"0", "3.125", "6.25", "9.375", "12.5", "15.625", "18.75", "21.875", "25", "28.125", "31.25", "34.375", "37.5", "40.625", "43.75", "46.875"};
            for(unsigned int i=0 ; i<sizeof(v_val)/sizeof(*v_val); i++){
                unsigned short bin_val=i;
                m_val.insert(BiPair(v_val[i], bin_val));
                m_val.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_val.find(value)!=m_val.end()){
              m_hybrid[feature] = m_val[value];
              return true;
            }
            else return false;
        }
        else if(feature=="TP_width"){
            InMap m_val;
            std::string v_val[8] = {"128x25", "64x25", "32x25", "15x25", "8x25", "4x25", "2x25", "1x25"};
            for(unsigned int i=0 ; i<sizeof(v_val)/sizeof(*v_val); i++){
                unsigned short bin_val=i;
                m_val.insert(BiPair(v_val[i], bin_val));
                m_val.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_val.find(value)!=m_val.end()){
              m_hybrid[feature] = m_val[value];
              return true;
            }
            else return false;
        }
        else if(feature=="TP_pol"){
            InMap m_val;
            std::string v_val[2] = {"positive", "negative"};
            for(unsigned int i=0 ; i<sizeof(v_val)/sizeof(*v_val); i++){
                unsigned short bin_val=i;
                m_val.insert(BiPair(v_val[i], bin_val));
                m_val.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_val.find(value)!=m_val.end()){
              m_hybrid[feature] = m_val[value];
              return true;
            }
            else return false;
        }

    }
    return false;
}

unsigned short Hybrid::GetReg(std::string feature){

    if(m_hybrid.find(feature)!=m_hybrid.end()){
        return m_hybrid[feature];
    }
    else{
        std::cout<<"ERROR the feature ::"<<feature<<":: does not exist"<<std::endl;
    }
    return 0;
}



Hybrid::~Hybrid()
{

}
