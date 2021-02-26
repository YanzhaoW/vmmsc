#include "hybrid_config_handler.h"

HybridConfigHandler::HybridConfigHandler(DAQWindow *top,QObject *parent) : QObject(parent), m_daqWindow{top}
{
    //getcwd(m_execPath,sizeof(m_execPath));
}


bool HybridConfigHandler::LoadAllHybridConf(std::string filename){
    return GenericAllHybridConf(1,filename);
}

bool HybridConfigHandler::WriteAllHybridConf(std::string filename){
    return GenericAllHybridConf(0,filename);
}

bool HybridConfigHandler::LoadSingleHybridConf(const char* filename, unsigned short fec, unsigned short hybrid){
    return GenericSingleHybridConf(1, filename, fec, hybrid);
}

bool HybridConfigHandler::LoadSingleHybridConf(const char* filename){//exact file name must be given!
    //add config path before file name
    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    return LoadHybridConfig(fname);
}

bool HybridConfigHandler::WriteSingleHybridConf(const char* filename, unsigned short fec, unsigned short hybrid){
    return GenericSingleHybridConf(0, filename, fec, hybrid);
}

bool HybridConfigHandler::WriteSingleHybridConf(const char* filename){//exact file name must be given!
    //need to extract daq,fec,... from file name
    std::stringstream str1; str1 << filename;
    std::string str(str1.str());
    //fec
    std::string fec_str = str.substr ((str.find("fec")+3),str.find("_",str.find("fec")+3)-(str.find("fec")+3));
    unsigned short fec =atoi(fec_str.c_str());if(!m_daqWindow->m_daq.GetFEC(fec)) {std::cout << "ERROR, fec " << fec << " does not exist "<< std::endl; return false;}
    //hybrid
    std::string hybrid_str = str.substr ((str.find("hybrid")+6),str.find("_",str.find("hybrid")+6)-(str.find("hybrid")+6));
    unsigned short hybrid =atoi(hybrid_str.c_str());if(!m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid)) {std::cout << "ERROR, hybrid " << hybrid << " does not exist "<< std::endl; return false;}
    //add config path before file name
    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;

    return WriteHybridConfig(fname,fec,hybrid);
}
bool HybridConfigHandler::GenericAllHybridConf(bool load, std::string filename){
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if (m_daqWindow->m_daq.GetFEC(j)){
            for (unsigned short k=0; k < HYBRIDS_PER_FEC; k++){
                if(m_daqWindow->m_daq.m_fecs[j].GetHybrid(k)){

                    if (load) std::cout <<"Loading vmm configuraten \""<<filename<<"\" for fec"<<j<<" hybrid"<<k<<std::endl;
                    else std::cout <<"Writing vmm configuraten \""<<filename<<"\" for fec"<<j<<" hybrid"<<k<<std::endl;
                    std::ostringstream oss;
                    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
                    oss << j;
                    fname+="_fec";
                    fname+=oss.str();
                    oss.str("");oss.clear();oss << k;
                    fname+="_hybrid";
                    fname+=oss.str();
                    fname+=".txt"; //build file name with path and extension
                    if (load){
                        if( !LoadHybridConfig(fname) ) {std::cout<<"Error"<<std::endl;return false;}
                    }
                    else{
                        if( !WriteHybridConfig(fname,j,k) ) {std::cout<<"Error"<<std::endl;return false;}
                    }

                }

            }
        }
    }
    return true;
}

bool HybridConfigHandler::GenericSingleHybridConf(bool load, const char* filename, unsigned short fec, unsigned short hybrid){
    if (load) std::cout <<"Loading vmm configuraten \""<<filename<<"\" for fec"<<fec<<" hybrid"<<hybrid<<std::endl;
    else std::cout <<"Loading vmm configuraten \""<<filename<<"\" for fec"<<fec<<" hybrid"<<hybrid<<std::endl;
    std::ostringstream oss;
    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    oss << fec;
    fname+="_fec";
    fname+=oss.str();
    oss.str("");oss.clear();oss << hybrid;
    fname+="_hybrid";
    fname+=oss.str();
    fname+=".txt"; //build file name with path and extension
    if (load) return LoadHybridConfig(fname);
    else return WriteHybridConfig(fname,fec,hybrid);
}

bool HybridConfigHandler::LoadHybridConfig(std::string fname){ //load the VMM configuration from file
    std::string empty = "";
    unsigned short daq = 0, fec = 0, hybrid = 0;
    std::ifstream f; f.open(fname,std::ifstream::in);
    if(!f.is_open()) {std::cout<< "file "<<fname<<" not found"<<std::endl;return false;}
    while (!f.eof() ){
        std::string s, val;
        const char *a, *b;
        f >> s >> val;
        if (s == empty && val == empty) break; // for empty line at end of file
        else if (s == "fec") {fec = atoi(val.c_str());}
        else if (s == "hybrid") {hybrid = atoi(val.c_str());}
        else {
            a = s.c_str(); b = val.c_str();
            if (!m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg(a,b)) return false;
        }
        if( (f.fail()) ) {return false;}
    }
    f.close(); //TODO:UpdateGUI:rootWindow->updateConfigState();
    return true;
}

bool HybridConfigHandler::WriteHybridConfig(std::string fname, unsigned short fec, unsigned short hybrid){
    std::ofstream f; f.open(fname,std::ofstream::out);
    if(!f.is_open()) {return false;}
    f << "fec " << fec << std::endl;
    f << "hybrid " << hybrid << std::endl;
    f << "\n";
    for(auto const entr: m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_hybrid){
        f<<entr.first<< " " <<entr.second<<std::endl;
        if(f.fail()) {return false;}
    }
    f << "\n";

    f.close();
    return true;
}

HybridConfigHandler::~HybridConfigHandler(){

}
