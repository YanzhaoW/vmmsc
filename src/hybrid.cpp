#include "hybrid.h"

Hybrid::Hybrid(): m_vmmActs (VMMS_PER_HYBRID)
{
    LoadDefault();
    m_vmmActs[0] = 1;
    m_vmmActs[1] = 1;
    m_art = 0;


}

void Hybrid::LoadDefault(){
    m_hybrid = {{"axis",0}, {"position", 0}, {"CKBC",2}, {"CKBC_skew",0}, {"CKDT",2}, {"TP_skew", 0}, {"TP_width", 0}, {"TP_pol", 0}};
    SetReg("CKBC", (std::string)"40");
    SetReg("CKDT", (std::string)"180");
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
    typedef std::map<std::string, unsigned short> InMap;
    typedef std::pair<std::string, unsigned short> BiPair;
    if(m_hybrid.find(feature)==m_hybrid.end()) return false;
    else{
        if(feature=="axis"){
            if(value == "0"){
                m_hybrid[feature] = 0;
                return true;
            }
            else if(value == "1"){
                m_hybrid[feature] = 1;
                return true;
            }
            else if(value == "2"){
                m_hybrid[feature] = 2;
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

        if(feature=="CKBC"){
            InMap m_val;
            std::string v_val[8] = {"80", "80inv", "40", "20", "10", "5", "2.5", "1.25"};
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
            std::string v_val[4] = {"22.5", "45", "90", "180"};
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
