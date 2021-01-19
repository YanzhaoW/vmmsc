#include "vmm.h"

VMM::VMM():
    m_vmmSettings ( new VMMSettings)
{
LoadDefault();
}

bool VMM::SetRegi(std::string feature, std::string val, int ch){
    if (SetRegister(feature, val, ch)) return true;
     else{
        std::cout << "ERROR register " << feature << " or entered value " << val << " does not exist." << std::endl;
         return false;
     }
}

void VMM::LoadDefault(){

    //Fill default values of channels
    for(int i =0; i<64; i++){
         m_vmmSettings->m_channels[i].m_channel = {{"sc", 0}, {"sl", 0}, {"st", 0}, {"sth", 0}, {"sm", 0}, {"sd", 0}, {"smx", 0}, {"ADC0_10", 0}, {"ADC0_8", 0}, {"ADC0_6", 0}  };
    }

    //Fill default values to map for Global Register 1
    for(std::string elem: m_vmmSettings->m_names_GReg1){
        m_vmmSettings->m_globalReg1->insert(std::pair<std::string, unsigned short>(elem, 0));
    }
    //Fill default values to map for Global Register 2
    for(std::string elem: m_vmmSettings->m_names_GReg2){
        m_vmmSettings->m_globalReg1->insert(std::pair<std::string, unsigned short>(elem, 0));
    }
    ///Possibility to add more default values
    SetRegi("gain", 2);//corrsponds to 3 mV/fC
    SetRegi("monitoring", "Temperature_sensor");
    SetRegi("sdp_2", (std::string)"300");
    SetRegi("sdt",(std::string)"300");
    SetRegi("s10b",1);
    SetRegi("s8b",1);
    SetRegi("stc",0);
    SetRegi("convtime_8",3);
    SetRegi("convtime_10",3);
    SetRegi("sbip",true);
    SetRegi("sbfm",true);
    SetRegi("sdcks",true);

/*
    std::vector<std::string> m_names_GReg1 = {"sp", "sdp", "sbmx", "sbft", "sbfp", "sbfm", "slg", "scmx", "monitoring", "sfa", "sfam", "peaktime", "sfm",
                                              "gain", "sng", "stot", "sttt", "ssh", "stc", "sdt", "sdp_2", "convtime_10", "convtime_8", "convtime_6",
                                              "s8b", "s6b", "s10b", "sdcks", "sdcka", "sdck6b", "sdrv", "stpp", "slvs", "s32", "stcr", "ssart", "srec",
                                              "stlc", "sbip", "srat", "sfrst", "slvsbc", "slvstp", "slvstk",  "slvsdt", "slvsart", "slvstki", "slvsena", "slvs6b", "reset1", "reset2"} ;
*/
}


void VMM::LoadCalibSettings(){

    //Fill default values of channels
    for(int i =0; i<64; i++){
        m_vmmSettings->m_channels[i].m_channel = {{"sc", 0}, {"sl", 0}, {"st", 1}, {"sth", 0}, {"sm", 0}, {"sd", 0}, {"smx", 0}, {"ADC0_10", 0}, {"ADC0_8", 0}, {"ADC0_6", 0}  };
    }

    //Fill default values to map for Global Register 1
    for(std::string elem: m_vmmSettings->m_names_GReg1){
        m_vmmSettings->m_globalReg1->insert(std::pair<std::string, unsigned short>(elem, 0));
    }
    //Fill default values to map for Global Register 2
    for(std::string elem: m_vmmSettings->m_names_GReg2){
        m_vmmSettings->m_globalReg1->insert(std::pair<std::string, unsigned short>(elem, 0));
    }
    ///Possibility to add more default values
    // gain set by user
    //SetRegi("gain", 2);//corrsponds to 3 mV/fC
    // test pulse height set by user
    //SetRegi("sdp_2", (std::string)"300");
    // Thresholds DAC set by user
    //SetRegi("sdt",(std::string)"300");
    // TAC slope set by user
    //SetRegi("stc",1);
    SetRegi("s10b",1);
    SetRegi("s8b",1);
    SetRegi("monitoring", "Pulser_DAC");
    SetRegi("convtime_8",3);
    SetRegi("convtime_10",3);
}


bool VMM::SetRegi(std::string feature, int val, int ch){
    std::string value =std::to_string(val);
    if (SetRegister(feature, value, ch)) return true;
    else{
       std::cout << "ERROR register " << feature << " or entered value " << val << " does not exist." << std::endl;
        return false;
    }
}

bool VMM::SetRegi(std::string feature, double val, int ch){
    std::string value =std::to_string(val);
    if (SetRegister(feature, value, ch)) return true;
    else{
      std::cout << "ERROR register " << feature << " or entered value " << val << " does not exist." << std::endl;
        return false;
    }
}


bool VMM::SetRegister(std::string feature, std::string val, int ch ){
    /// Function to set the VMM settings and write them in struct/map.
    typedef std::map<std::string, unsigned short> InMap;
    typedef std::pair<std::string, unsigned short> BiPair;
    InMap m_bin; // map for all settings which are booleans
    std::string bools[14] = {"0","False","false","FALSE", "off", "OFF", "Off", "1", "True", "true", "TRUE", "on", "ON", "On"};
    for(unsigned int i=0 ; i<sizeof(bools)/sizeof(*bools); i++){
        unsigned short bin_val=0;
                if(i>=7) bin_val=1;
        m_bin.insert(BiPair(bools[i], bin_val));
    }
    if(ch==-9999){

        if(feature == "monitoring"){
            InMap m_mon;
            std::string mm_val[4] = {"Pulser_DAC", "Threshold_DAC", "Bandgap_reference", "Temperature_sensor"};
            for(unsigned int i=1 ; i<=sizeof(mm_val)/sizeof(*mm_val); i++){
                unsigned short bin_val=i;
                m_mon.insert(BiPair(mm_val[i-1], bin_val+63));
                m_mon.insert(BiPair(std::to_string(i+63), bin_val+63));
            }
            if(m_mon.find(val)!=m_mon.end()){
                m_vmmSettings->m_globalReg1->at("monitoring") = m_mon[val];
                m_vmmSettings->m_globalReg1->at("scmx") = 0;
                return true;
            }
            else{
                 for(unsigned short i=0; i<64;i++){ //ATTENTION: here starting with Channel 0 to 63 and not with 1
                    if(val == std::to_string(i) ){
                        m_vmmSettings->m_globalReg1->at("monitoring") = i;
                        m_vmmSettings->m_globalReg1->at("scmx") = 1;
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
               m_vmmSettings->m_globalReg1->at(feature) = m_sfam[val];
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
              m_vmmSettings->m_globalReg1->at(feature) = m_peakt[val];
              return true;
            }
            return false;
        }

        else if(feature == "gain" ){
            InMap m_gain;
            std::string gain[8] = {"0.5", "1", "3", "4.5", "6", "9", "12", "16"};
            for(unsigned int i=0 ; i<sizeof(gain)/sizeof(*gain); i++){
                unsigned short bin_val=i;
//                m_gain.insert(BiPair(gain[i], bin_val));// had to remove this option since it interferes with the indexing option needed by the GUI
                m_gain.insert(BiPair(std::to_string(i), bin_val));
            }
            if(m_gain.find(val)!=m_gain.end()){
              m_vmmSettings->m_globalReg1->at(feature) = m_gain[val];
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
              m_vmmSettings->m_globalReg1->at(feature) = m_val[val];
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
              m_vmmSettings->m_globalReg1->at(feature) = m_val[val];
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
              m_vmmSettings->m_globalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "convtime_10" ){//sc10b: sc010b, sc110b
            InMap m_val;
            m_val.insert(BiPair("200 ns + 120 ns (both)", 0 ));
            m_val.insert(BiPair("200 ns + 60 ns (MSB part)", 1 ));
            m_val.insert(BiPair("200 ns + 60 ns (LSB part)", 2 ));
            m_val.insert(BiPair("200 ns", 3 ));
            m_val.insert(BiPair("0", 0 ));
            m_val.insert(BiPair("1", 1 ));
            m_val.insert(BiPair("2", 2 ));
            m_val.insert(BiPair("3", 3 ));

            if(m_val.find(val)!=m_val.end()){
              m_vmmSettings->m_globalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "convtime_8" ){//sc8b: sc08b, sc18b
            InMap m_val;
            m_val.insert(BiPair("200 ns + 120 ns (both)", 0 ));
            m_val.insert(BiPair("200 ns + 60 ns (MSB part)", 1 ));
            m_val.insert(BiPair("200 ns + 60 ns (LSB part)", 2 ));
            m_val.insert(BiPair("100 ns", 3 ));
            m_val.insert(BiPair("0", 0 ));
            m_val.insert(BiPair("1", 1 ));
            m_val.insert(BiPair("2", 2 ));
            m_val.insert(BiPair("3", 3 ));

            if(m_val.find(val)!=m_val.end()){
              m_vmmSettings->m_globalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }
        else if(feature == "convtime_6" ){//sc6b: sc06b, sc16b, sc26b
            InMap m_val;
            m_val.insert(BiPair("25 ns", 0 ));
            m_val.insert(BiPair("25 ns + 1 ckdt", 1 ));
            m_val.insert(BiPair("25 ns + 2 ckdt", 2 ));
            m_val.insert(BiPair("25 ns + 3 ckdt", 3 ));
            m_val.insert(BiPair("25 ns + 4 ckdt", 4 ));
            m_val.insert(BiPair("25 ns + 5 ckdt", 5 ));
            m_val.insert(BiPair("25 ns + 6 ckdt", 6 ));
            m_val.insert(BiPair("25 ns + 7 ckdt", 7 ));
            m_val.insert(BiPair("0", 0 ));
            m_val.insert(BiPair("1", 1 ));
            m_val.insert(BiPair("2", 2 ));
            m_val.insert(BiPair("3", 3 ));
            m_val.insert(BiPair("4", 4 ));
            m_val.insert(BiPair("5", 5 ));
            m_val.insert(BiPair("6", 6 ));
            m_val.insert(BiPair("7", 7 ));

            if(m_val.find(val)!=m_val.end()){
              m_vmmSettings->m_globalReg1->at(feature) = m_val[val];
              return true;
            }
            return false;
        }
        //filling the settings which are booleans
        else if(m_vmmSettings->m_globalReg1->find(feature)!=m_vmmSettings->m_globalReg1->end()){
            if(m_bin.find(val)!=m_bin.end()){
                m_vmmSettings->m_globalReg1->at(feature) = m_bin[val];
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
              m_vmmSettings->m_channels[ch].m_channel[feature] = m_val[val];
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
              m_vmmSettings->m_channels[ch].m_channel[feature] = m_val[val];
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
              m_vmmSettings->m_channels[ch].m_channel[feature]= m_val[val];
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
              m_vmmSettings->m_channels[ch].m_channel[feature] = m_val[val];
              return true;
            }
            return false;
        }

        else if(m_vmmSettings->m_channels[ch].m_channel.find(feature)!=m_vmmSettings->m_channels[ch].m_channel.end()){
            m_vmmSettings->m_channels[ch].m_channel[feature]= m_bin[val];
            return true;
        }

        return false;
    }

    return false;
}

unsigned short VMM::GetRegister( std::string feature, int ch ){
    if(ch==-9999){
        if(feature == "monitoring"){
            if(m_vmmSettings->m_globalReg1->at("scmx")==0){
                return (m_vmmSettings->m_globalReg1->at("monitoring")-63);
            }
            else if(m_vmmSettings->m_globalReg1->at("scmx")==1){
                return m_vmmSettings->m_globalReg1->at("monitoring");
            }
            else{
                std::cout<<"ERROR in feature ::"<<feature<<std::endl;
            }
        }

        else if(m_vmmSettings->m_globalReg1->find(feature)!=m_vmmSettings->m_globalReg1->end()){
            return m_vmmSettings->m_globalReg1->at(feature);
        }
        else{
            std::cout<<"ERROR the feature ::"<<feature<<":: does not exist"<<std::endl;
        }
    }
    else if(ch>=0 && ch<64){
        if(m_vmmSettings->m_channels[ch].m_channel.find(feature)!=m_vmmSettings->m_channels[ch].m_channel.end()){
            return m_vmmSettings->m_channels[ch].m_channel[feature];
        }
        else{
           std::cout<<"ERROR the feature "<<feature<<" does not exist in Channel settings"<<std::endl;
        }
    }
    return 0;
}

VMM::~VMM(){


}

