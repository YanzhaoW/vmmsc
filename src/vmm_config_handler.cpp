#include "vmm_config_handler.h"

VMMConfigHandler::VMMConfigHandler(DAQWindow *top, QObject *parent) :QObject(parent), m_daqWindow{top}
{
    //getcwd(m_execPath,sizeof(m_execPath));
}

bool VMMConfigHandler::LoadAllVMMConf(std::string filename){
    return GenericAllVMMConf(1,filename);
}

bool VMMConfigHandler::WriteAllVMMConf(std::string filename){
    return GenericAllVMMConf(0,filename);
}

bool VMMConfigHandler::LoadSingleVMMConf(const char* filename, unsigned short fec, unsigned short hybrid, unsigned short vmm){
    return GenericSingleVMMConf(1, filename, fec, hybrid, vmm);
}

bool VMMConfigHandler::LoadSingleVMMConf(const char* filename){//exact file name must be given!
    //add config path before file name
    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    return LoadVMMConfig(fname);
}

bool VMMConfigHandler::WriteSingleVMMConf(const char* filename, unsigned short fec, unsigned short hybrid, unsigned short vmm){
    return GenericSingleVMMConf(0, filename, fec, hybrid, vmm);
}

bool VMMConfigHandler::WriteSingleVMMConf(const char* filename){//exact file name must be given!
    //need to extract daq,fec,... from file name
    std::stringstream str1; str1 << filename;
    std::string str(str1.str());
    //fec
    std::string fec_str = str.substr ((str.find("fec")+3),str.find("_",str.find("fec")+3)-(str.find("fec")+3));
    unsigned short fec =atoi(fec_str.c_str());if(!m_daqWindow->m_daq.GetFEC(fec)) {std::cout << "ERROR, fec " << fec << " does not exist "<< std::endl; return false;}
    //hybrid
    std::string hybrid_str = str.substr ((str.find("hybrid")+6),str.find("_",str.find("hybrid")+6)-(str.find("hybrid")+6));
    unsigned short hybrid =atoi(hybrid_str.c_str());if(!m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid)) {std::cout << "ERROR, hybrid " << hybrid << " does not exist "<< std::endl; return false;}
    //vmm
    std::string vmm_str = str.substr ((str.find("vmm")+3),str.find("_",str.find("vmm")+3)-(str.find("vmm")+3));
    unsigned short vmm =atoi(vmm_str.c_str());
    //add config path before file name
    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;

    return WriteVMMConfig(fname,fec,hybrid,vmm);
}
bool VMMConfigHandler::GenericAllVMMConf(bool load, std::string filename){
    //    bool VMM_config_handler::GenericAllVMMConf(bool load, const char* filename){
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if (m_daqWindow->m_daq.GetFEC(j)){
            for (unsigned short k=0; k < HYBRIDS_PER_FEC; k++){
                if(m_daqWindow->m_daq.m_fecs[j].GetHybrid(k)){
                    for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){

                        if (load) std::cout <<"Loading vmm configuration \""<<filename<<"\" for fec"<<j<<" hybrid"<<k<<" vmm"<<m<<std::endl;
                        else std::cout <<"Writing vmm configuration \""<<filename<<"\" for fec"<<j<<" hybrid"<<k<<" vmm"<<m<<std::endl;
                        std::ostringstream oss;
                        std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
                        oss << j;
                        fname+="_fec";
                        fname+=oss.str();
                        oss.str("");oss.clear();oss << k;
                        fname+="_hybrid";
                        fname+=oss.str();
                        oss.str("");oss.clear();oss << m;
                        fname+="_vmm";
                        fname+=oss.str();
                        fname+=".txt"; //build file name with path and extension
                        if (load){
                            if( !LoadVMMConfig(fname) ) {std::cout<<"Error"<<std::endl;return false;}
                        }
                        else{
                            if( !WriteVMMConfig(fname,j,k,m) ) {std::cout<<"Error"<<std::endl;return false;}
                        }

                    }


                }
            }
        }
    }
    return true;
}

bool VMMConfigHandler::GenericSingleVMMConf(bool load, const char* filename, unsigned short fec, unsigned short hybrid, unsigned short vmm){
    if (load) std::cout <<"Loading vmm configuration \""<<filename<<"\" for fec"<<fec<<" hybrid"<<hybrid<<" vmm"<<vmm<<std::endl;
    else std::cout <<"Loading vmm configuration \""<<filename<<"\" for fec"<<fec<<" hybrid"<<hybrid<<" vmm"<<vmm<<std::endl;
    std::ostringstream oss;
    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    oss << fec;
    fname+="_fec";
    fname+=oss.str();
    oss.str("");oss.clear();oss << hybrid;
    fname+="_hybrid";
    fname+=oss.str();
    oss.str("");oss.clear();oss << vmm;
    fname+="_vmm";
    fname+=oss.str();
    fname+=".txt"; //build file name with path and extension
    if (load) return LoadVMMConfig(fname);
    else return WriteVMMConfig(fname,fec,hybrid,vmm);
}

bool VMMConfigHandler::LoadVMMConfig(std::string fname){ //load the VMM configuration from file
    std::string channel = "channel";
    std::string empty = "";
    unsigned short fec = 0, hybrid = 0, vmms = 0;
    std::ifstream f; f.open(fname,std::ifstream::in);
    if(!f.is_open()) {std::cout<< "file "<<fname<<" not found"<<std::endl;return false;}
    while (!f.eof() ){
        std::string s, val, chanreg, chanval,a,b;
        //const char *a, *b;
        unsigned short chan;

        std::string line;
        std::getline(f, line);
        std::size_t pos = line.find(" ");
        if (pos!=std::string::npos) {
            s = line.substr(0,pos);
            val = line.substr(pos+1);
            if(s == "channel") {
                pos = val.find(' ');
                if (pos!=std::string::npos) {
                    chan = atoi(val.substr(0,pos).c_str());
                    val = val.substr(pos+1);
                    pos = val.find(' ');
                    if (pos!=std::string::npos) {
                        a = val.substr(0,pos);
                        b = val.substr(pos+1);
                    }
                }
            }
        }
        else {
            continue;
        }

        if (s == empty && val == empty) break; // for empty line at end of file
        else if (s == "fec") {fec = atoi(val.c_str());}
        else if (s == "hybrid") {hybrid = atoi(val.c_str());}
        else if (s == "vmm") {vmms = atoi(val.c_str());}
        else if (s == "description") {
            auto noSpaceEnd = std::remove( val.begin(), val.end(), '"');
            val.erase(noSpaceEnd, val.end());
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmms].SetInfo("description",val.c_str());
        }
        else if (s == channel) {
            if (!m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmms].SetRegi(a.c_str(), atoi(b.c_str()),chan)) return false;
        }
        else {
            a = s.c_str(); b = val.c_str();
            if (!m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmms].SetRegi(a.c_str(), atoi(b.c_str()))) return false;
        }
        if( (f.fail()) ) {return false;}
    }
    f.close(); //TODO:UpdateGUI:root1->updateConfigState();
    return true;
}

bool VMMConfigHandler::WriteVMMConfig(std::string fname, unsigned short fec, unsigned short hybrid, unsigned short vmm){
    std::string description = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetInfo("description");
    if(description == "") {
        description = "\"\"";
    }
    else {
        description =  "\"" + description + "\"";
    }
    std::ofstream f; f.open(fname,std::ofstream::out);
    if(!f.is_open()) {return false;}
    f << "fec " << fec << std::endl;
    f << "hybrid " << hybrid << std::endl;
    f << "vmm " << vmm << std::endl;
    f << "description " << description << std::endl;

    for(const auto &entr: (*m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].m_vmmSettings->m_globalRegs)){
        if(entr.first != "scmx") {
            if(entr.first == "sm5_sm0") {
                if(m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].m_vmmSettings->m_globalRegs->at("scmx") == 0) {
                    f<<entr.first<< " " <<entr.second+63<<std::endl;
                }
                else {
                    f<<entr.first<< " " <<entr.second<<std::endl;
                }

            }
            else {
                f<<entr.first<< " " <<entr.second<<std::endl;
            }
        }
        if(f.fail()) {return false;}
    }
    f << "\n";
    for(unsigned short k=0;k<VMM_CHANNELS;k++){

        for(auto const &entr: m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].m_vmmSettings->m_channels[k].m_channel){
            f << "channel " << k << " " <<entr.first<< " " <<entr.second<<std::endl;
            if(f.fail()) {return false;}
        }
        if(k<63) f << "\n"; // no empty line at end
    }

    f.close();
    return true;
}

VMMConfigHandler::~VMMConfigHandler(){

}
