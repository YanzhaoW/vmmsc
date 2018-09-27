#include "hybrid_config_handler.h"

HybridConfigHandler::HybridConfigHandler(MainWindow *top,QObject *parent) : QObject(parent), m_mainWindow{top}
{
    getcwd(m_execPath,sizeof(m_execPath));
}


bool HybridConfigHandler::LoadAllHybridConf(std::string filename){
    return GenericAllHybridConf(1,filename);
}

bool HybridConfigHandler::WriteAllHybridConf(std::string filename){
    return GenericAllHybridConf(0,filename);
}

bool HybridConfigHandler::LoadSingleHybridConf(const char* filename, unsigned short daq, unsigned short fec, unsigned short hdmi, unsigned short hybrid){
    return GenericSingleHybridConf(1, filename, daq, fec, hdmi, hybrid);
}

bool HybridConfigHandler::LoadSingleHybridConf(const char* filename){//exact file name must be given!
    //add config path before file name
    std::string fname = m_execPath; fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    return LoadHybridConfig(fname);
}

bool HybridConfigHandler::WriteSingleHybridConf(const char* filename, unsigned short daq, unsigned short fec, unsigned short hdmi, unsigned short hybrid){
    return GenericSingleHybridConf(0, filename, daq, fec, hdmi, hybrid);
}

bool HybridConfigHandler::WriteSingleHybridConf(const char* filename){//exact file name must be given!
    //need to extract daq,fec,... from file name
    std::stringstream str1; str1 << filename;
    std::string str(str1.str());
    //daq
    std::string daq_str = str.substr ((str.find("daq")+3),str.find("_",str.find("daq")+3)-(str.find("daq")+3));
    unsigned short daq =atoi(daq_str.c_str()); if(!m_mainWindow->m_daq_act[daq]) {std::cout << "ERROR, daq " << daq << " does not exist "<< std::endl; return false;}
    //fec
    std::string fec_str = str.substr ((str.find("fec")+3),str.find("_",str.find("fec")+3)-(str.find("fec")+3));
    unsigned short fec =atoi(fec_str.c_str());if(!m_mainWindow->m_daqs[daq].GetFEC(fec)) {std::cout << "ERROR, fec " << fec << " does not exist "<< std::endl; return false;}
    //hdmi
    std::string hdmi_str = str.substr ((str.find("hdmi")+4),str.find("_",str.find("hdmi")+4)-(str.find("hdmi")+4));
    unsigned short hdmi =atoi(hdmi_str.c_str());if(!m_mainWindow->m_daqs[daq].m_fecs[fec].GetHDMI(hdmi)) {std::cout << "ERROR, hdmi " << hdmi << " does not exist "<< std::endl; return false;}
    //hybrid
    std::string hybrid_str = str.substr ((str.find("hybrid")+6),str.find("_",str.find("hybrid")+6)-(str.find("hybrid")+6));
    unsigned short hybrid =atoi(hybrid_str.c_str());if(!m_mainWindow->m_daqs[daq].m_fecs[fec].m_hdmis[hdmi].GetHybrid(hybrid)) {std::cout << "ERROR, hybrid " << hybrid << " does not exist "<< std::endl; return false;}
    //add config path before file name
    std::string fname = m_execPath; fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;

    return WriteHybridConfig(fname,daq,fec,hdmi,hybrid);
}
bool HybridConfigHandler::GenericAllHybridConf(bool load, std::string filename){
//    bool hybrid_config_handler::GenericAllVMMConf(bool load, const char* filename){
    for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
        if (m_mainWindow->m_daq_act[i]){
            for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                if (m_mainWindow->m_daqs[i].GetFEC(j)){
                    for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
                        if(m_mainWindow->m_daqs[i].m_fecs[j].GetHDMI(k)){
                            for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                                if (m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].GetHybrid(l)){
                                    if (load) std::cout <<"Loading vmm configuraten \""<<filename<<"\" for daq"<<i<<" fec"<<j<<" hdmi"<<k<<" hybrid"<<l<<std::endl;
                                    else std::cout <<"Writing vmm configuraten \""<<filename<<"\" for daq"<<i<<" fec"<<j<<" hdmi"<<k<<" hybrid"<<l<<std::endl;
                                    std::ostringstream oss;
                                    std::string fname = m_execPath; fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
                                    oss << i;
                                    fname+="_daq";
                                    fname+=oss.str();
                                    oss.str("");oss.clear();oss << j;
                                    fname+="_fec";
                                    fname+=oss.str();
                                    oss.str("");oss.clear();oss << k;
                                    fname+="_hdmi";
                                    fname+=oss.str();
                                    oss.str("");oss.clear();oss << l;
                                    fname+="_hybrid";
                                    fname+=oss.str();
                                    fname+=".txt"; //build file name with path and extension
                                    if (load){
                                        if( !LoadHybridConfig(fname) ) {std::cout<<"Error"<<std::endl;return false;}
                                    }
                                    else{
                                        if( !WriteHybridConfig(fname,i,j,k,l) ) {std::cout<<"Error"<<std::endl;return false;}
                                    }

                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool HybridConfigHandler::GenericSingleHybridConf(bool load, const char* filename, unsigned short daq, unsigned short fec, unsigned short hdmi, unsigned short hybrid){
    if (load) std::cout <<"Loading vmm configuraten \""<<filename<<"\" for daq"<<daq<<" fec"<<fec<<" hdmi"<<hdmi<<" hybrid"<<hybrid<<std::endl;
    else std::cout <<"Loading vmm configuraten \""<<filename<<"\" for daq"<<daq<<" fec"<<fec<<" hdmi"<<hdmi<<" hybrid"<<hybrid<<std::endl;
    std::ostringstream oss;
    std::string fname = m_execPath; fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    oss << daq;
    fname+="_daq";
    fname+=oss.str();
    oss.str("");oss.clear();oss << fec;
    fname+="_fec";
    fname+=oss.str();
    oss.str("");oss.clear();oss << hdmi;
    fname+="_hdmi";
    fname+=oss.str();
    oss.str("");oss.clear();oss << hybrid;
    fname+="_hybrid";
    fname+=oss.str();
    fname+=".txt"; //build file name with path and extension
    std::cout<< " file name: "  << fname << std::endl;
    if (load) return LoadHybridConfig(fname);
    else return WriteHybridConfig(fname,daq,fec,hdmi,hybrid);
}

bool HybridConfigHandler::LoadHybridConfig(std::string fname){ //load the VMM configuration from file
    std::string empty = "";
    unsigned short daq = 0, fec = 0, hdmi = 0,hybrid = 0;
    std::ifstream f; f.open(fname,std::ifstream::in);
    if(!f.is_open()) {std::cout<< "file "<<fname<<" not found"<<std::endl;return false;}
    while (!f.eof() ){
        std::string s, val;
        const char *a, *b;
        f >> s >> val;
        if (s == empty && val == empty) break; // for empty line at end of file
        else if (s == "daq") {daq = atoi(val.c_str());}
        else if (s == "fec") {fec = atoi(val.c_str());}
        else if (s == "hdmi") {hdmi = atoi(val.c_str());}
        else if (s == "hybrid") {hybrid = atoi(val.c_str());}
        else {
            a = s.c_str(); b = val.c_str();
            if (!m_mainWindow->m_daqs[daq].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg(a,b)) return false;
        }
        if( (f.fail()) ) {return false;}
    }
    f.close(); //TODO:UpdateGUI:rootWindow->updateConfigState();
    return true;
}

bool HybridConfigHandler::WriteHybridConfig(std::string fname, unsigned short daq, unsigned short fec, unsigned short hdmi, unsigned short hybrid){
    std::ofstream f; f.open(fname,std::ofstream::out);
    if(!f.is_open()) {return false;}
    f << "daq " << daq << std::endl;
    f << "fec " << fec << std::endl;
    f << "hdmi " << hdmi << std::endl;
    f << "hybrid " << hybrid << std::endl;
    f << "\n";
    for(auto const entr: m_mainWindow->m_daqs[daq].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].m_hybrid){
        f<<entr.first<< " " <<entr.second<<std::endl;
        if(f.fail()) {return false;}
    }
    f << "\n";

    f.close();
    return true;
}

HybridConfigHandler::~HybridConfigHandler(){

}
