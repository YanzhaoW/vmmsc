#include "vmm.h"

VMM::VMM():
             m_vmmSettings ( new VMMSettings)
{
    LoadDefault();
}

void VMM::LoadDefault(){

    //VMM3a channel registers
    for(int i =0; i<64; i++){
        m_vmmSettings->m_channels[i].m_channel = {{"sc", 0}, {"sl", 0}, {"st", 0}, {"sth", 0}, {"sm", 0}, {"sd", 0}, {"smx", 0}, {"sz10b", 0}, {"sz08b", 0}, {"sz06b", 0}  };
    }

    //Fill default values to map for Global Register 1
    for(std::string elem: m_vmmSettings->m_names_GReg1){
        m_vmmSettings->m_globalRegs->insert(std::pair<std::string, unsigned short>(elem, 0));
    }
    //Fill default values to map for Global Register 2
    for(std::string elem: m_vmmSettings->m_names_GReg2){
        m_vmmSettings->m_globalRegs->insert(std::pair<std::string, unsigned short>(elem, 0));
    }
    ///Possibility to add more default values
    SetRegi("sg", 2);//corrsponds to 3 mV/fC
    SetRegi("sm5_sm0", 67); //Temperature_sensor
    SetRegi("sdp10", 300);
    SetRegi("sdt",300);
    SetRegi("s10b",1);
    SetRegi("s8b",1);
    SetRegi("stc",0);
    SetRegi("sc8b",3);
    SetRegi("sc10b",3);
    SetRegi("srat",0);
    SetRegi("sbip",true);
    SetRegi("sbfm",true);
    SetRegi("sdcks",false);

    //    SetRegi("sttt",1);
    //    SetRegi("stot",1);
    //    SetRegi("s6b",0);



}

bool VMM::SetInfo(std::string feature, std::string value){
    if(m_vmm_info.find(feature)==m_vmm_info.end()) return false;
    else{
        m_vmm_info[feature] = value;
        return true;
    }
    return false;
}


std::string VMM::GetInfo(std::string feature){

    if(m_vmm_info.find(feature)!=m_vmm_info.end()){
        return m_vmm_info[feature];
    }
    else{
        std::cout<<"ERROR the feature ::"<<feature<<":: does not exist"<<std::endl;
    }
    return 0;
}


bool VMM::SetRegi(std::string feature, int val, int ch){
    if (SetRegister(feature, val, ch)) return true;
    else{
        std::cout << "ERROR register " << feature << " or entered value " << val << " does not exist." << std::endl;
        return false;
    }
}

bool VMM::SetRegi(std::string feature, bool val, int ch){
    int value = 0;
    if(val == true) {
        value = 1;
    }
    if (SetRegister(feature, value, ch)) return true;
    else{
        std::cout << "ERROR register " << feature << " or entered value " << val << " does not exist." << std::endl;
        return false;
    }
}



bool VMM::SetRegister(std::string feature, int val, int ch ){
    if(ch==-9999){
        if(feature == "sm5_sm0"){
            //Channels
            if(val >= 0 && val <= 63){
                m_vmmSettings->m_globalRegs->at("scmx") = 1;
                m_vmmSettings->m_globalRegs->at("sm5_sm0") = val;
                return true;
            }
            else if (val >= 64 && val <= 67){
                //Attention: Starts at 1: 1=Pulser_DAC, 2=Threshold_DAC, 3=Bandgap_reference, 4=Temperature_sensor
                m_vmmSettings->m_globalRegs->at("scmx") = 0;
                m_vmmSettings->m_globalRegs->at("sm5_sm0") = val-63;
                return true;
            }
            return false;

        }
        else if(feature == "sfam" ){
            //0=timing at threshold, 1=timing at peak
            if(val >= 0 && val <= 1){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }

        else if(feature == "st" ){
            //0=200ns, 1=100ns,2=50ns, 3=25ns
            if(val >= 0 && val <= 3){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }

        else if(feature == "sg" ){
            //0=0.5 mV/fC, 1=1 mV/fC, 2=3 mV/fC, 3=4.5 mV/fC, 4=6 mV/fC, 5=9 mV/fC, 6=12 mV/fC, 7=16 mV/fC
            if(val>=0 && val <=7){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }

        else if(feature == "stc" ){
            //0=60ns, 1=100ns, 2=350ns, 3=650ns
            if(val>=0 && val <=3){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }

        else if(feature == "sdt" ){//coarse threshold DAC
            if(val>=0 && val<=1023){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }
        else if(feature == "sdp10" ){//test pulse DAC
            if(val>=0 && val<=1023){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }
        else if(feature == "sc10b" ){//sc10b: sc010b, sc110b
            //0=200 ns + 120 ns (both), 1=200 ns + 60 ns (MSB part), 2=200 ns + 60 ns (LSB part), 3=200 ns
            if(val>=0 && val<=3){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }
        else if(feature == "sc8b" ){//sc8b: sc08b, sc18b
            //0=100 ns + 120 ns (both), 1=100 ns + 60 ns (MSB part), 2=100 ns + 60 ns (LSB part), 3=100 ns
            if(val>=0 && val<=3){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;

        }
        else if(feature == "truncate_i" ){
            if(val>=0 && val<=63){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }
        else if(feature == "nskip_i" ){
            if(val>=0 && val<=127){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }
        else if(feature == "window_i" ){
            if(val>=0 && val<=7){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }
        else if(feature == "rollover_i" ){
            if(val>=0 && val<=4095){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }
        else if(feature == "L0offset_i" ){
            if(val>=0 && val<=4095){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }
        else if(feature == "offset_i" ){
            if(val>=0 && val<=4095){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }

        //filling the settings which are booleans
        else {
            if(val>=0 && val<=1){
                m_vmmSettings->m_globalRegs->at(feature) = val;
                return true;
            }
            return false;
        }
        std::cout<<"ERROR the feature ::"<<feature<<":: does not exist"<<std::endl;
        return false;
    }
    else if(ch>=0 && ch<64){

        if(feature == "sd"){
            if(val>=0 && val<=31){
                m_vmmSettings->m_channels[ch].m_channel[feature] = val;
                return true;
            }
            return false;
        }
        else if(feature == "sz10b"){
            if(val>=0 && val<=31){
                m_vmmSettings->m_channels[ch].m_channel[feature] = val;
                return true;
            }
            return false;
        }
        else if(feature == "sz08b"){
            if(val>=0 && val<=15){
                m_vmmSettings->m_channels[ch].m_channel[feature] = val;
                return true;
            }
            return false;
        }
        else if(feature == "sz06b"){
            if(val>=0 && val<=7){
                m_vmmSettings->m_channels[ch].m_channel[feature] = val;
                return true;
            }
            return false;
        }
        else if(feature == "sc"){
            if(val>=0 && val<=1){
                m_vmmSettings->m_channels[ch].m_channel[feature]= val;
                return true;
            }
            return false;
        }
        else if(feature == "sl"){
            if(val>=0 && val<=1){
                m_vmmSettings->m_channels[ch].m_channel[feature]= val;
                return true;
            }
            return false;
        }
        else if(feature == "st"){
            if(val>=0 && val<=1){
                m_vmmSettings->m_channels[ch].m_channel[feature]= val;
                return true;
            }
            return false;
        }
        else if(feature == "sth"){
            if(val>=0 && val<=1){
                m_vmmSettings->m_channels[ch].m_channel[feature]= val;
                return true;
            }
            return false;
        }
        else if(feature == "sm"){
            if(val>=0 && val<=1){
                m_vmmSettings->m_channels[ch].m_channel[feature]= val;
                return true;
            }
            return false;
        }
        else if(feature == "smx"){
            if(val>=0 && val<=1){
                m_vmmSettings->m_channels[ch].m_channel[feature]= val;
                return true;
            }
            return false;
        }


        return false;
    }

    return false;
}



double VMM::GetRegister( std::string feature, int ch ){
    if(ch==-9999){
        if(m_vmmSettings->m_globalRegs->find(feature)!=m_vmmSettings->m_globalRegs->end()){
            return m_vmmSettings->m_globalRegs->at(feature);
        }
        else{
            std::cout<<"ERROR the feature ::"<<feature<<":: does not exist"<<std::endl;
        }
    }
    else if(ch>=0 && ch<64){
        if(m_vmmSettings->m_channels[ch].m_channel.find(feature)!=m_vmmSettings->m_channels[ch].m_channel.end()){
            return m_vmmSettings->m_channels[ch].m_channel[feature];
        }
        std::cout<<"ERROR the feature "<<feature<<" does not exist in Channel settings"<<std::endl;
    }
    return 0;
}

VMM::~VMM(){


}

