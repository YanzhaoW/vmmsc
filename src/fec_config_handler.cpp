#include "fec_config_handler.h"

FECConfigHandler::FECConfigHandler(DAQWindow *top, QObject *parent) : QObject(parent), m_daqWindow{top}
{
    //getcwd(m_execPath,sizeof(m_execPath));
}

bool FECConfigHandler::LoadAllFECConf(std::string filename){
    return GenericAllFECConf(1,filename);
}

bool FECConfigHandler::WriteAllFECConf(std::string filename){
    return GenericAllFECConf(0,filename);
}

bool FECConfigHandler::LoadSingleFECConf(const char* filename, unsigned short fec){
    return GenericSingleFECConf(1, filename, fec);
}

bool FECConfigHandler::LoadSingleFECConf(const char* filename){//exact file name must be given!
    //add config path before file name
    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    return LoadFECConfig(fname);
}

bool FECConfigHandler::WriteSingleFECConf(const char* filename, unsigned short fec){
    return GenericSingleFECConf(0, filename, fec);
}

bool FECConfigHandler::WriteSingleFECConf(const char* filename){//exact file name must be given!
    //need to extract daq,fec from file name
    std::stringstream str1; str1 << filename;
    std::string str(str1.str());
    //fec
    std::string fec_str = str.substr ((str.find("fec")+3),str.find("_",str.find("fec")+3)-(str.find("fec")+3));
    unsigned short fec =atoi(fec_str.c_str());if(!m_daqWindow->m_daq.GetFEC(fec)) {std::cout << "ERROR, fec " << fec << " does not exist "<< std::endl; return false;}
    //add config path before file name
    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;

    return WriteFECConfig(fname,fec);
}

bool FECConfigHandler::GenericAllFECConf(bool load, std::string filename){
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if (m_daqWindow->m_daq.GetFEC(j)){
            if (load) std::cout <<"Loading FEC configuration \""<<filename<<"\" for fec"<<j<<std::endl;
            else std::cout <<"Writing FEC configuration \""<<filename<<"\" for fec"<<j<<std::endl;
            std::ostringstream oss;
            std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
            oss << j;
            fname+="_fec";
            fname+=oss.str();
            fname+=".txt"; //build file name with path and extension
            if (load){
                if( !LoadFECConfig(fname) ) {std::cout<<"Error"<<std::endl;return false;}
            }
            else{
                if( !WriteFECConfig(fname,j) ) {std::cout<<"Error"<<std::endl;return false;}
            }
        }
    }
    return true;
}

bool FECConfigHandler::GenericSingleFECConf(bool load, const char* filename, unsigned short fec){
    if (load) std::cout <<"Loading FEC configuration \""<<filename<<"\" for fec"<<fec<<std::endl;
    else std::cout <<"Writing FEC configuration \""<<filename<<"\" for fec"<<fec<<std::endl;
    std::ostringstream oss;
    std::string fname = m_daqWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    oss << fec;
    fname+="_fec";
    fname+=oss.str();
    fname+=oss.str();
    fname+=".txt"; //build file name with path and extension
    if (load) return LoadFECConfig(fname);
    else return WriteFECConfig(fname,fec);
}

bool FECConfigHandler::LoadFECConfig(std::string fname){ //load the FEC configuration from file
    std::string empty = "";
    unsigned short daq = 0, fec = 0;
    std::ifstream f; f.open(fname,std::ifstream::in);
    if(!f.is_open()) {std::cout<< "file "<<fname<<" not found"<<std::endl;return false;}
    while (!f.eof() ){
        std::string s, val, s2;
        const char *a, *b;
        f >> s >> val;
        if (s == empty && val == empty) break; // for empty line at end of file
        else if (s == "fec") {fec = atoi(val.c_str());}
        else {
            a = s.c_str();
            b = val.c_str();
            if(a == tr("ip_fec") || a == tr("ip_daq")) {
                QHostAddress ip;
                ip.setAddress(b);
                s2 = std::to_string(ip.toIPv4Address());
                b = s2.c_str();  //use char const* as target type
            }
            if (!m_daqWindow->m_daq.m_fecs[fec].SetReg(a,b)) return false;

        }
        if( (f.fail()) ) {return false;}
    }
    f.close(); //TODO:UpdateGUI:rootWindow->updateConfigState();
    return true;
}

bool FECConfigHandler::WriteFECConfig(std::string fname, unsigned short fec){
    std::ofstream f; f.open(fname,std::ofstream::out);
    if(!f.is_open()) {return false;}
    f << "fec " << fec << std::endl;
    f << "\n";
    for(unsigned short j=0;j<m_daqWindow->m_daq.m_fecs[fec].GetRegSize() ;j++){
        if(m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) != tr("not_used")) {
            if(m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) ==  tr("ip_fec") || m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) ==  tr("ip_daq")) {
                QHostAddress ip;
                ip.setAddress(m_daqWindow->m_daq.m_fecs[fec].GetReg(j));
                f << m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) << " " << ip.toString().toStdString() << std::endl;
            }
            else {
                f << m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) << " " << m_daqWindow->m_daq.m_fecs[fec].GetReg(j) << std::endl;
            }
            if(f.fail()) {return false;}
        }
    }

    f.close();
    return true;
}

FECConfigHandler::~FECConfigHandler(){

}
