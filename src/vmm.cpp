#include "vmm.h"

VMM::VMM():
    Regi ( new VMM_Settings)
{
    //Fill default values to map for Global Register 1
    for(std::string elem: Regi->Names_GReg1){
        Regi->m_GlobalReg1->insert(std::pair<std::string, unsigned short>(elem, 0));
    }
    //Fill default values to map for Global Register 2
    for(std::string elem: Regi->Names_GReg2){
        Regi->m_GlobalReg1->insert(std::pair<std::string, unsigned short>(elem, 0));
    }
    ///Possibility to add more default values and channel deault values have to be added!!!!!
    SetRegi("gain", 2);//corrsponds to 3 mV/fC
    SetRegi("monitoring", "Pulser_DAC");
//    std::cout<<"SCMX: "<<GetRegister("scmx")<<std::endl;
//    Regi->ch_settings[6].VMMSCBool = 1;
//    Regi->ch_settings[17].VMMSMXBool = 1;
//    SetRegi("sbfm", 1);
//    SetRegi("peaktime", 2);
    SetRegi("sdt",(std::string)"300");
    SetRegi("sdp_2",(std::string)"300");
//    SetRegi("monitoring",(std::string)"Pulser_DAC");

}

bool VMM::SetRegi(std::string feature, std::string val, int ch){
    if (SetRegister(feature, val, ch)) return true;
     else{
        std::cout << "ERROR register or entered value does not exist." << std::endl;
         return false;
     }
}


bool VMM::SetRegi(std::string feature, int val, int ch){
    std::string value =std::to_string(val);
    if (SetRegister(feature, value, ch)) return true;
    else{
       std::cout << "ERROR register or entered value does not exist." << std::endl;
        return false;
    }
}

bool VMM::SetRegi(std::string feature, double val, int ch){
    std::string value =std::to_string(val);
    if (SetRegister(feature, value, ch)) return true;
    else{
       std::cout << "ERROR register or entered value does not exist." << std::endl;
        return false;
    }
}


bool VMM::SetRegister(std::string feature, std::string val, int ch ){
    /// Function to set the VMM settings and write them in struct/map.
    typedef std::map<std::string, unsigned short> InMap;
    typedef std::pair<std::string, unsigned short> BiPair;
    InMap m_bin; // map for all settings which are booleans
    std::string bools[14] = {"0","False","false","FALSE", "off", "OFF", "Off", "1", "True", "true", "TRUE", "on", "ON", "On"};
//    InMap m_bin = {{"0", 0},{"False", 0}};//,"false","FALSE", "off", "OFF", "Off", "1", "True", "true", "TRUE", "on", "ON", "On"};
    for(unsigned int i=0 ; i<sizeof(bools)/sizeof(*bools); i++){
        unsigned short bin_val=0;
                if(i>=7) bin_val=1;
        m_bin.insert(BiPair(bools[i], bin_val));
    }
std::cout<<"reigster: "<<feature<<" value: "<<val<<std::endl;
    if(ch==-9999){

        if(feature == "monitoring"){
            InMap m_mon;
            std::cout<<"monitoring: "<<val<<std::endl;
            std::string mm_val[4] = {"Pulser_DAC", "Threshold_DAC", "Bandgap_reference", "Temperature_sensor"};
            for(unsigned int i=1 ; i<=sizeof(mm_val)/sizeof(*mm_val); i++){
                unsigned short bin_val=i;
                m_mon.insert(BiPair(mm_val[i-1], bin_val+63));
                m_mon.insert(BiPair(std::to_string(i+63), bin_val+63));
            }
            if(m_mon.find(val)!=m_mon.end()){
                Regi->m_GlobalReg1->at("monitoring") = m_mon[val];
                Regi->m_GlobalReg1->at("scmx") = 0;
                return true;
            }
            else{
                for(unsigned short i=0; i<64;i++){ //ATTENTION: here starting with Channel 0 to 63 and not with 1
                    if(val == std::to_string(i) ){
                        Regi->m_GlobalReg1->at("monitoring") = i;
                        Regi->m_GlobalReg1->at("scmx") = 1;
                        return true;
                    }
                }
                return false;
            }

        }

        else if(feature == "scmx" ){
            std::cout<<"You cannot set SCMX by hand"<<std::endl;
            return true;
         }
        else if(feature == "sfam" ){
            InMap m_sfam = m_bin;
            m_sfam.insert(BiPair("timing at threshold", 0 ));
            m_sfam.insert(BiPair("Timing at threshold", 0 ));
            m_sfam.insert(BiPair("Timing At Threshold", 0 ));
            m_sfam.insert(BiPair("0", 0 ));
            m_sfam.insert(BiPair("timing at peak", 1 ));
            m_sfam.insert(BiPair("Timing at peak", 1 ));
            m_sfam.insert(BiPair("Timing At Peak", 1 ));
            m_sfam.insert(BiPair("1", 1 ));

             if(m_sfam.find(val)!=m_sfam.end()){
               Regi->m_GlobalReg1->at(feature) = m_sfam[val];
               return true;
             }
             return false;
            /// enable of ART needed or automaticly done in GUI???
        }

        else if(feature == "peaktime" ){
            InMap m_peakt;
            std::string ptime[4] = { "200", "100", "50", "25" };
            for(unsigned int i=0 ; i<sizeof(ptime)/sizeof(*ptime); i++){
                unsigned short bin_val=i;
                m_peakt.insert(BiPair(ptime[i], bin_val));
                m_peakt.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_peakt.find(val)!=m_peakt.end()){
              Regi->m_GlobalReg1->at(feature) = m_peakt[val];
              return true;
            }
            return false;
        }

        else if(feature == "gain" ){
            InMap m_gain;
            std::string gain[8] = {"0.5", "1", "3", "4.5", "6", "9", "12", "16"};
            for(unsigned int i=0 ; i<sizeof(gain)/sizeof(*gain); i++){
                unsigned short bin_val=i;
                m_gain.insert(BiPair(gain[i], bin_val));
                m_gain.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_gain.find(val)!=m_gain.end()){
              Regi->m_GlobalReg1->at(feature) = m_gain[val];
              return true;
            }
            return false;
        }

        else if(feature == "stc" ){
            InMap m_val;
            std::string v_val[4] = {"60", "100", "350", "650"};
            for(unsigned int i=0 ; i<sizeof(v_val)/sizeof(*v_val); i++){
                unsigned short bin_val=i;
                m_val.insert(BiPair(v_val[i], bin_val));
                m_val.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_val.find(val)!=m_val.end()){
              Regi->m_GlobalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }

        else if(feature == "sdt" ){//coarse threshold DAC
            InMap m_val;
            for(unsigned short i=0 ; i<1024; i++){
                m_val.insert(BiPair(std::to_string(i), i));
            }
            if(m_val.find(val)!=m_val.end()){
              Regi->m_GlobalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "sdp_2" ){//test pulse DAC
            InMap m_val;
            for(unsigned short i=0 ; i<1024; i++){
                m_val.insert(BiPair(std::to_string(i), i));
            }
            if(m_val.find(val)!=m_val.end()){
              Regi->m_GlobalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "convtime_10" ){
            InMap m_val;
            m_val.insert(BiPair("200ns", 0 ));//sc010b
            m_val.insert(BiPair("+60ns", 1 ));//sc110b
            m_val.insert(BiPair("0", 0 ));//sc010b
            m_val.insert(BiPair("1", 1 ));//sc110b

            if(m_val.find(val)!=m_val.end()){
              Regi->m_GlobalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "convtime_8" ){
            InMap m_val;
            m_val.insert(BiPair("100ns", 0 ));//sc08b
            m_val.insert(BiPair("+60ns", 1 ));//sc18b
            m_val.insert(BiPair("1", 1 ));//sc18b
            m_val.insert(BiPair("0", 0 ));//sc08b

            if(m_val.find(val)!=m_val.end()){
              Regi->m_GlobalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "convtime_6" ){
            InMap m_val;
            m_val.insert(BiPair("low", 0 ));
            m_val.insert(BiPair("middle", 1 ));
            m_val.insert(BiPair("up", 2 ));
            m_val.insert(BiPair("0", 0 ));
            m_val.insert(BiPair("1", 1 ));
            m_val.insert(BiPair("2", 2 ));

            if(m_val.find(val)!=m_val.end()){
              Regi->m_GlobalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }
        //filling the settings which are booleans
        else if(Regi->m_GlobalReg1->find(feature)!=Regi->m_GlobalReg1->end()){
            if(m_bin.find(val)!=m_bin.end()){
                Regi->m_GlobalReg1->at(feature) = m_bin[val];
                return true;
            }
        }
         std::cout<<"ERROR the feature ::"<<feature<<":: does not exist"<<std::endl;
        return false;
    }
    else if(ch>=0 && ch<64){

        if(feature == "sd"){
            InMap m_val;
            for(unsigned short i=0 ; i<32; i++){
                m_val.insert(BiPair(std::to_string(i), i));
            }
            if(m_val.find(val)!=m_val.end()){
              Regi->ch_settings[ch].m_channel[feature] = m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "ADC0_10"){
            InMap m_val;
            for(unsigned short i=0 ; i<32; i++){
                m_val.insert(BiPair(std::to_string(i), i));
            }
            if(m_val.find(val)!=m_val.end()){
              Regi->ch_settings[ch].m_channel[feature] = m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "ADC0_8"){
            InMap m_val;
            for(unsigned short i=0 ; i<16; i++){
                m_val.insert(BiPair(std::to_string(i), i));
            }
            if(m_val.find(val)!=m_val.end()){
              Regi->ch_settings[ch].m_channel[feature]= m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "ADC0_6"){
            InMap m_val;
            for(unsigned short i=0 ; i<8; i++){
                m_val.insert(BiPair(std::to_string(i), i ));
            }
            if(m_val.find(val)!=m_val.end()){
              Regi->ch_settings[ch].m_channel[feature] = m_val[val];
              return true;
            }
            return false;
        }

        else if(Regi->ch_settings[ch].m_channel.find(feature)!=Regi->ch_settings[ch].m_channel.end()){
            Regi->ch_settings[ch].m_channel[feature]= m_bin[val];
            return true;
        }

        return false;
    }

    return false;
}

unsigned short VMM::GetRegister( std::string feature, int ch ){
    if(ch==-9999){
        if(feature == "monitoring"){
            if(Regi->m_GlobalReg1->at("scmx")==0){
                return (Regi->m_GlobalReg1->at("monitoring")-63);
            }
            else if(Regi->m_GlobalReg1->at("scmx")==1){
                return Regi->m_GlobalReg1->at("monitoring");
            }
            else{
                std::cout<<"ERROR in feature ::"<<feature<<std::endl;
            }
        }

        else if(Regi->m_GlobalReg1->find(feature)!=Regi->m_GlobalReg1->end()){
            return Regi->m_GlobalReg1->at(feature);
        }
        else{
            std::cout<<"ERROR the feature ::"<<feature<<":: does not exist"<<std::endl;
        }
    }
    else if(ch>=0 && ch<64){
        if(Regi->ch_settings[ch].m_channel.find(feature)!=Regi->ch_settings[ch].m_channel.end()){
            return Regi->ch_settings[ch].m_channel[feature];
        }
        else{
           std::cout<<"ERROR the feature "<<feature<<" does not exist in Channel settings"<<std::endl;
        }
    }
}

VMM::~VMM(){


}

