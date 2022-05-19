#include "hybrid.h"

Hybrid::Hybrid()
{
    LoadDefault();
}

void Hybrid::LoadDefault(){
    m_hybrid = {{"TP_skew", 0}, {"TP_width", 0}, {"TP_pol", 0}};
    m_hybrid_info = {{"firmware_version", ""}, {"geo_id", ""}, {"hybrid_id", ""},{"link_status", "0"}, {"description", ""}};
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
        if(feature=="TP_skew"){
            InMap m_val;
            std::string v_val[16];
            for(int i=0;i < 2; i++) {
                for(int n=0;n< 8; n++) {
                   QString txt = QStringLiteral("%1").arg((i*g_clock_period)+n*g_time_factor);
                   v_val[i*8+n] = txt.toStdString();
                }
            }
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

bool Hybrid::SetInfo(std::string feature, std::string value){
    if(m_hybrid_info.find(feature)==m_hybrid_info.end()) return false;
    else{
       m_hybrid_info[feature] = value;
       return true;
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

std::string Hybrid::GetInfo(std::string feature){

    if(m_hybrid_info.find(feature)!=m_hybrid_info.end()){
        return m_hybrid_info[feature];
    }
    else{
        std::cout<<"ERROR the feature ::"<<feature<<":: does not exist"<<std::endl;
    }
    return 0;
}

Hybrid::~Hybrid()
{

}
