#include "daq_config_handler.h"

DAQConfigHandler::DAQConfigHandler(MainWindow *top, QObject *parent) :QObject(parent), m_mainWindow{top}
{
    //getcwd(m_execPath,sizeof(m_execPath));
}

bool DAQConfigHandler::LoadDAQConf(const char* filename){
    //add config path before file name
    std::string fname = m_mainWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    return LoadDAQConfig(fname);
}

bool DAQConfigHandler::WriteDAQConf(const char* filename){
    std::string fname = m_mainWindow->GetApplicationPath().toStdString(); fname+="/../"; fname+=CONFIG_DIR; fname+="/"; fname+=filename;
    return WriteDAQConfig(fname);
}

bool DAQConfigHandler::LoadDAQConfig(std::string fname){ //load the DAQ configuration from file
    ////***************RESET before loading config file*******************////
    for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
//        rootWindow->daq_act={false};
            for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                m_mainWindow->m_daqs[i].SetFEC(j, false);
                    for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
                       m_mainWindow->m_daqs[i].m_fecs[j].SetHDMI(k, false);
                            for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                                m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].SetHybrid(l, false);
                                    for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                                        m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].SetVMM(m, false);
                                    }
                                }
                            }
                   }
    }
    std::string empty = "";
    std::ifstream f; f.open(fname,std::ifstream::in);
    if(!f.is_open()) {std::cout<< "file "<<fname<<" not found"<<std::endl;return false;}
    while (!f.eof() ){
        std::string line, val, word;
        const char *a;
        std::getline (f,line);
        std::istringstream iss(line);
        iss >> word;
        if (word[0] == '/' && word[1] == '/'){;}//do nothing

        else if (word == "daq"){
            iss >> val;
            std::string daq = val.substr(0, val.size()-1);
            iss >> word;
            if (word == "fecs:"){
                iss >> val;
                unsigned short fecs = atoi(val.c_str());
                for (unsigned short i = 0; i < fecs; i++){
                    std::getline (f,line);
                    std::istringstream iss(line);
                    iss >> word; if(word!= "fec") {std::cout  << "Syntax error in file, looking for \"fec\" in line "<< line << std::endl;return false;}
                    iss >> val; std::string fecnrst = val.substr(0, val.size()-1); unsigned short fecnr = atoi(fecnrst.c_str()); if(fecnr != i) {std::cout  << "Syntax error in file, expecting "<< i << " instead of " << fecnr << " in line "<< line << std::endl;return false;}
                    m_mainWindow->m_daqs[0].SetFEC(fecnr,true); std::cout << "Setting fec " << fecnr << " active" << std::endl;
                    iss >> word; if(word!= "hdmis:") {std::cout  << "Syntax error in file, looking for \"hdmis:\" in line "<< line << std::endl;return false;}
                    iss >> val; unsigned short hdmis = atoi(val.c_str());
                    for (unsigned short j = 0; j < hdmis; j++){
                        std::getline (f,line);
                        std::istringstream iss(line);
                        iss >> word; if(word!= "hdmi") {std::cout  << "Syntax error in file, looking for \"hdmi\" (" << j+1 << "th hdmi, you have set "<<hdmis<<" hdmis) in line "<< line << std::endl;return false;}
                        iss >> val; std::string hdminrst = val.substr(0, val.size()-1);
                        unsigned short hdminr = atoi(hdminrst.c_str()); m_mainWindow->m_daqs[0].m_fecs[fecnr].SetHDMI(hdminr,true); std::cout << "Setting hdmi " << hdminr << " on fec " << fecnr << " active" << std::endl;
                        iss >> word; if(word!= "hybrids:") {std::cout  << "Syntax error in file, looking for \"hybrids:\" in line "<< line << std::endl;return false;}
                        iss >> val; unsigned short hybrids = atoi(val.c_str());
                        for (unsigned short k = 0; k < hybrids; k++){
                            std::getline (f,line);
                            std::istringstream iss(line);
                            iss >> word; if(word!= "hybrid") {std::cout  << "Syntax error in file, looking for \"hybrid\" in line "<< line << std::endl;return false;}
                            iss >> val; std::string hybridnrst = val.substr(0, val.size()-1); unsigned short hybridnr = atoi(hybridnrst.c_str()); if(hybridnr != k) {std::cout  << "Syntax error in file, expecting "<< k << " instead of " << hybridnr << " in line "<< line << std::endl;return false;}
                            m_mainWindow->m_daqs[0].m_fecs[fecnr].m_hdmis[hdminr].SetHybrid(k,true);
                            iss >> val;
                            if(val[0]!= 'x' && val[0] != 'y') {std::cout  << "Syntax error in file, looking for \"x\" or \"y\" in line "<< line << ", got " << val << std::endl;return false;}
                            else if (val == "x"){
                                m_mainWindow->m_daqs[0].m_fecs[fecnr].m_hdmis[hdminr].m_hybrids[hybridnr].SetReg("axis",0);
                            }
                            else if (val == "y"){ m_mainWindow->m_daqs[0].m_fecs[fecnr].m_hdmis[hdminr].m_hybrids[hybridnr].SetReg("axis",1);}
                            else if (val == "z"){ m_mainWindow->m_daqs[0].m_fecs[fecnr].m_hdmis[hdminr].m_hybrids[hybridnr].SetReg("axis",2);}
                            iss >> val; std::string posnrst = val.substr(0, val.size()-1);
                            m_mainWindow->m_daqs[0].m_fecs[i].m_hdmis[hdminr].m_hybrids[k].SetReg("position",atoi(posnrst.c_str()));
                            iss >> word; if(word!= "vmms:") {std::cout  << "Syntax error in file, looking for \"vmms:\" in line "<< line << std::endl;return false;}
                            iss >> val; unsigned short vmms = atoi(val.c_str());
                            std::cout<<"No. VMMS: "<<vmms<<std::endl;
                            for (unsigned short l = 0; l < vmms; l++){
                                //rootWindow->daq[0].fec[fecnr].hdmi[hdminr].hybrid[hybridnr].SetVMM(l,true);
                                std::getline (f,line);
                                std::istringstream iss(line);
                                iss >> word; if(word!= "vmm") {std::cout  << "Syntax error in file, looking for \"vmm\" in line "<< line << std::endl;return false;}
                                iss >> val; std::string vmminrst = val.substr(0, val.size());
                                unsigned short vmminr = atoi(vmminrst.c_str()); m_mainWindow->m_daqs[0].m_fecs[fecnr].m_hdmis[hdminr].m_hybrids[hybridnr].SetVMM(vmminr,true); std::cout << "Setting vmm " << vmminr << " on hybrid " << hybridnr << " active" << std::endl;


                            }
                        }
                    }
                }
            }
            else {std::cout << "Syntax error in file, looking for \"fecs\" after daq "<< daq << std::endl;return false;}
//            for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
//                if (rootWindow->daq_act[i]){
//                    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
//                        if (rootWindow->daq[i].GetFEC(j)){
//                            for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
//                                if(rootWindow->daq[i].fec[j].GetHDMI(k)){
//                                    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
//                                        if (rootWindow->daq[i].fec[j].hdmi[k].GetHybrid(l)){
//                                            for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
//                                                if (rootWindow->daq[i].fec[j].hdmi[k].hybrid[l].GetVMM(m)){
//                                                    std::cout << "vmm " << m << " on hybrid " << l << "(pos " << rootWindow->daq[i].fec[j].hdmi[k].hybrid[l].GetPosNo()<< ", " <<rootWindow->daq[i].fec[j].hdmi[k].hybrid[l].GetPosX() << ") on hmdi "<< k << " on fec " << j << " on daq " << i << " is active" << std::endl;
//                                                }
//                                            }
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//             }
        }
        else if (word == empty){ // for empty line at end of file
            iss >> val;
            if (val == empty){
                break;
            }
        }
        else {
            for (unsigned short i = 0; i < m_mainWindow->m_daqs[0].GetRegSize();i++){
                 if ( word == m_mainWindow->m_daqs[0].GetRegName(i) ){
                     iss >> val;
                     a = val.c_str();
                     const char * reg = word.c_str();
                     m_mainWindow->m_daqs[0].SetReg(reg, a);
                 }
            }
        }
        if( (f.fail()) ) {return false;}
    }
    f.close(); //TODO:UpdateGUI:rootWindow->updateConfigState();
    return true;
}

bool DAQConfigHandler::WriteDAQConfig(std::string fname){
    std::ofstream f; f.open(fname,std::ofstream::out);
    if(!f.is_open()) {return false;}
    // write conneted hardware and vmm position map
    f << "//Conneted hardware and vmm position map. No comment in the following lines, take care of syntax!" << std::endl;
    for (unsigned short i = 0; i < DAQS_PER_GUIWINDOW; i++){
        if (m_mainWindow->m_daq_act[i]) {
            unsigned short countFECS = 0;
            for (unsigned short j = 0; j < FECS_PER_DAQ; j++){
                if (m_mainWindow->m_daqs[i].GetFEC(j)){
                    countFECS ++;
                }
            }
            f << "daq " << i << ": fecs: "<< countFECS << std::endl;
            for (unsigned short j = 0; j < FECS_PER_DAQ; j++){
                if (m_mainWindow->m_daqs[i].GetFEC(j)){
                    unsigned short countHDMIS = 0;
                    bool HDMI_act[HDMIS_PER_FEC] = {false};
                    for (unsigned short k = 0; k < HDMIS_PER_FEC; k++){
                        if (m_mainWindow->m_daqs[i].m_fecs[j].GetHDMI(k)){
                            countHDMIS++;
                            HDMI_act[k] = true;
                        }
                    }
                    f << "\tfec "<< j << ": hdmis: " << countHDMIS << std::endl;
                    for (unsigned short k = 0; k < HDMIS_PER_FEC; k++){
                        if (HDMI_act[k]){
                            unsigned short countHybrids = 0;
                            for (unsigned short l = 0; l < HYBRIDS_PER_HDMI; l++){
                                if (m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].GetHybrid(l)){
                                    countHybrids++;
                                }
                            }
                            f << "\t\thdmi "<< k << ": hybrids: " << countHybrids<< std::endl;
                            for (unsigned short l = 0; l < HYBRIDS_PER_HDMI; l++){
                                if (m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].GetHybrid(l)){
                                    unsigned short countVMMS = 0;
                                    for (unsigned short m = 0; m < VMMS_PER_HYBRID; m++){
                                        if (m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetVMM(m)){
                                            countVMMS++;
                                        }
                                    }
                                    if (m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("axis") == 0){
                                        f << "\t\t\thybrid "<< l << ": x " << m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("position") << ", vmms: "<< countVMMS << std::endl;
                                        std::cout << "DAQ " << i << ", FEC " << j << ", HDMI " << k << ", hybrid " << l << " at position " << m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("position") << " on x axis has " << countVMMS << " active VMMS" << std::endl;
                                    }
                                    else if (m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("axis") == 1){
                                        f << "\t\t\thybrid "<< l << ": y " << m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("position") << ", vmms: "<< countVMMS << std::endl;
                                        std::cout << "DAQ " << i << ", FEC " << j << ", HDMI " << k << ", hybrid " << l << " at position " << m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("position") << " on y axis has " << countVMMS << " active VMMS" << std::endl;
                                    }
                                    else {
                                        f << "\t\t\thybrid "<< l << ": z " << m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("position") << ", vmms: "<< countVMMS << std::endl;
                                        std::cout << "DAQ " << i << ", FEC " << j << ", HDMI " << k << ", hybrid " << l << " at position " << m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("position") << " on y axis has " << countVMMS << " active VMMS" << std::endl;
                                    }
                                    for (unsigned short m = 0; m < VMMS_PER_HYBRID; m++){
                                       if (m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetVMM(m)) f << "\t\t\t\tvmm "<< m<< std::endl;
                                    }
                                } //end if hybrid
                            }
                        }
                    }
                } //end if FEC
            }
        }
    }
    // daq general settings
    f << "// daq general settings" << std::endl;
    for(unsigned short i=0;i<m_mainWindow->m_daqs[0].GetRegSize() ;i++){
        const char *chr = m_mainWindow->m_daqs[0].GetReg(m_mainWindow->m_daqs[0].GetRegName(i));
        std::stringstream str;
        str << chr;
        f << m_mainWindow->m_daqs[0].GetRegName(i) << " " << str.str() << std::endl;
        std::cout << "Writing reg " << m_mainWindow->m_daqs[0].GetRegName(i) << " value: " << chr << std::endl;
        if(f.fail()) {return false;}
    }
    f << "\n";
    f.close();



    return true;
}

DAQConfigHandler::~DAQConfigHandler(){

}
