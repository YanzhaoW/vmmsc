#include "calibration_module.h"

calibration_module::calibration_module(MainWindow *top, QObject *parent) :
    QObject(parent),
    root_main{top},
     m_DAQSocket(0),
     m_daqSocket(0),
     m_calibRun(false),
     m_msg(new MessageHandler),
     m_ignore16(false),
     v_calibvar(new vector<vector< vector<int> > >(16, vector< vector<int> >(64,vector<int>(0)))),
     r_mean(new vector<result_mean>)
{

}

// ------------------------------------------------------------------------ //
void calibration_module::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
    // remove monitoring m_daqMonitor->LoadMessageHandler(msg());
}
// ------------------------------------------------------------------------ //
void calibration_module::GetActVMM(){
    act_vmm.clear();
//    root_main->daqwindow->ui->VMM_select->clear();
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if (root_main->daq[0].GetFEC(j) ){
            for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
                if( root_main->daq[0].fec[j].GetHDMI(k) ){
                    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                        if (root_main->daq[0].fec[j].hdmi[k].GetHybrid(l)){
                            for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                                if (root_main->daq[0].fec[j].hdmi[k].hybrid[l].GetVMM(m) ){
                                    act_vmm.push_back(2*k + m);
                                    root_main->daqwindow->ui->VMM_select->addItem(QString("VMM%0").arg(2*k + m));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    r_mean->resize(act_vmm.size());
    connect( root_main->daqwindow->ui->VMM_select, SIGNAL(currentIndexChanged(int)),
                                            this, SLOT(updatePlot()));
}
// ------------------------------------------------------------------------ //
void calibration_module::updatePlot(){
    if(QObject::sender() == root_main->daqwindow->ui->VMM_select){
        PlotADC(root_main->daqwindow->ui->VMM_select->currentIndex());
    }
}

// ------------------------------------------------------------------------ //

void calibration_module::StartCalib(){
    root_main->daqwindow->ui->line_configFile->setText("Calib_config");
    emit root_main->daqwindow->ui->Button_save->clicked();
    root_main->daqwindow->ui->line_configFile->setText("");

    delete r_mean;
    r_mean = new vector<result_mean>;
    delete v_calibvar;
    v_calibvar = new vector<vector< vector<int> > >(16, vector< vector<int> >(64,vector<int>(0)));
    eventcount = 0 ;
    if(root_main->daqwindow->ui->Calib_type->currentIndex() == 0) calibmode = "ADC";
    else if(root_main->daqwindow->ui->Calib_type->currentIndex() == 1){
        calibmode = "TDC";
        number_bits = 8;
    }
    GetActVMM();
    calibrun = "initial";
    CalibADC();
}

// ------------------------------------------------------------------------ //

void calibration_module::CalibADC(){
    emit root_main->daqwindow->ui->openConnection_2->clicked();
    if(! (root_main->daqwindow->ui->connectionLabel_2->text()==QString("all alive"))) {
        std::cout<<"Communication couldn't be established! \n exit calibration"<<std::endl;
        root_main->daqwindow->ui->InfoScreen->setTextColor(Qt::red);
        root_main->daqwindow->ui->InfoScreen->append(QString("Communication couldn't be established! \n exit calibration"));
        root_main->daqwindow->ui->Data->setChecked(true);
        root_main->ResetCalib();
        return;
    }
    if(calibrun=="initial"){
        std::map<std::string, unsigned short> m_high = {{"st", 1}, {"ADC0_10", 0}};
        if(calibmode == "TDC") m_high = {{"st", 1}, {"ADC0_8", 0}};
        for(int i=0; i<act_vmm.size(); i++){
            int hdmi =(act_vmm[i]/2) ;
            int vmm = act_vmm[i]%2;
            root_main->daq[0].fec[0].hdmi[hdmi].hybrid[0].vmm[vmm].LoadDefault(true, m_high );
        }
    }
    root_main->daqwindow->ui->checkBox->setChecked(true);
    emit root_main->daqwindow->ui->checkBox->stateChanged(true);
    emit root_main->daqwindow->ui->trgPulser->clicked();
    delete v_calibvar;
    v_calibvar = new vector<vector< vector<int> > >(16, vector< vector<int> >(64,vector<int>(0)));

    emit root_main->daqwindow->ui->onACQ->clicked();
    connectDAQSocket();
    eventcount = 0 ;



}

// ------------------------------------------------------------------------ //
void calibration_module::Counting(){
    if(eventcount<root_main->daqwindow->ui->Runs->value()){
        eventcount+=1;
    }
    else{
        emit root_main->daqwindow->ui->offACQ->clicked();
        closeDAQSocket();


     for(int m =0; m<act_vmm.size(); m++){
        vector<double> x1;
        vector<double> y1;

        for(unsigned int i = 0; i<64; i++){
//            for(unsigned int i = 0; i<v_calibvar->at(act_vmm[m]).size(); i++){
            x1.push_back(i);
            int sum = 0;
            for(unsigned int j = 0; j<v_calibvar->at(act_vmm[m]).at(i).size(); j++ ){
                sum += v_calibvar->at(act_vmm[m]).at(i).at(j);
                if(j == v_calibvar->at(act_vmm[m]).at(i).size()-1){
                    sum=sum/v_calibvar->at(act_vmm[m]).at(i).size();
                    y1.push_back(sum);
                }
            }
            if(v_calibvar->at(act_vmm[m]).at(i).size()==0)y1.push_back(0);//if channel is dead
        }
        r_mean->at(m).y_fullrange.push_back(y1);
        r_mean->at(m).channel=x1;
     }//loop active VMMs
        if(bincount==number_bits-1){
            if(calibrun == "singleplot"){
                Calib();
                GetCalSetting();
                calibrun = "plotting";
                CalibADC();
            }
            else if(calibrun == "plotting"){
                PlotADC(0);
                root_main->daqwindow->ui->InfoScreen->setReadOnly(true);
                root_main->daqwindow->ui->InfoScreen->clear();
                root_main->daqwindow->ui->InfoScreen->setTextColor(Qt::black);
                root_main->daqwindow->ui->InfoScreen->append("Best common values:");
                for(int i=0; i<act_vmm.size(); i++) root_main->daqwindow->ui->InfoScreen->append(QString("VMM%0  : %1").arg(act_vmm[i]).arg(r_mean->at(i).calib_value));
                calibrun = "writeconfig";
                GetCalSetting();
            }
        }
        else{
            calibrun="singleplot";
            bincount+=1;
            std::map<std::string, unsigned short> m_high = {{"st", 1}, {"ADC0_10", bincount}};
            if(calibmode == "TDC") m_high = {{"st", 1}, {"ADC0_8", bincount}};
            for(int i=0; i<act_vmm.size(); i++){
                int hdmi =(act_vmm[i]/2) ;
                int vmm = act_vmm[i]%2;
                root_main->daq[0].fec[0].hdmi[hdmi].hybrid[0].vmm[vmm].LoadDefault(true, m_high );
            }
            CalibADC();
        }
    }

}

void calibration_module::PlotADC(int m){
    root_main->daqwindow->ui->customPlot->clearGraphs();
    // give the axes some labels:
    root_main->daqwindow->ui->customPlot->xAxis->setLabel("channel");
    root_main->daqwindow->ui->customPlot->yAxis->setLabel("Mean ADC");
     if(calibmode == "TDC") root_main->daqwindow->ui->customPlot->yAxis->setLabel("Mean TDC");
    // set axes ranges, so we see all data:
    root_main->daqwindow->ui->customPlot->xAxis->setRange(0, 66);
    root_main->daqwindow->ui->customPlot->yAxis->setRange(150, 350);
    if(calibmode == "TDC") root_main->daqwindow->ui->customPlot->yAxis->setRange(40, 250);
    root_main->daqwindow->ui->customPlot->legend->setVisible(true);
    root_main->daqwindow->ui->customPlot->legend->setFont(QFont("Helvetica",9));
    for(int i=0; i<number_bits;i++){
        root_main->daqwindow->ui->customPlot->addGraph();
        root_main->daqwindow->ui->customPlot->graph(i)->setData(QVector<double>::fromStdVector(r_mean->at(m).channel), QVector<double>::fromStdVector(r_mean->at(m).y_fullrange.at(i)));
        root_main->daqwindow->ui->customPlot->graph(i)->setPen(QPen(QColor(i*10)));
        root_main->daqwindow->ui->customPlot->legend->removeItem(root_main->daqwindow->ui->customPlot->legend->itemCount()-1);
    }
    int n_graph = root_main->daqwindow->ui->customPlot->graphCount();
    root_main->daqwindow->ui->customPlot->addGraph();
    root_main->daqwindow->ui->customPlot->graph(n_graph)->setData(QVector<double>::fromStdVector(r_mean->at(m).channel), QVector<double>::fromStdVector(r_mean->at(m).v_calval));
    root_main->daqwindow->ui->customPlot->graph(n_graph)->setPen(QPen(Qt::red,4,Qt::SolidLine));
    root_main->daqwindow->ui->customPlot->graph(n_graph)->setName("Best common value");
    root_main->daqwindow->ui->customPlot->addGraph();
    root_main->daqwindow->ui->customPlot->graph(n_graph+1)->setData(QVector<double>::fromStdVector(r_mean->at(m).channel), QVector<double>::fromStdVector(r_mean->at(m).y_fullrange.at(number_bits)));
    root_main->daqwindow->ui->customPlot->graph(n_graph+1)->setPen(QPen(Qt::green,4,Qt::SolidLine));
    root_main->daqwindow->ui->customPlot->graph(n_graph+1)->setName("Calibrated curve");

    root_main->daqwindow->ui->customPlot->replot();
}

void calibration_module::Calib(){
    // Function to calculate the best common ADC value for all channels
    for(int m=0; m<act_vmm.size(); m++){
        vector<double> sorted_0mV = r_mean->at(m).y_fullrange.at(0);
        vector<double> sorted_31mV = r_mean->at(m).y_fullrange.at(number_bits-1);
        std::sort(sorted_0mV.begin(), sorted_0mV.end());
        std::sort(sorted_31mV.begin(), sorted_31mV.end());
        int state =0;
        int check = 0;
        while( (sorted_0mV[check-state]-sorted_31mV[63-check]) < 0 ){
            if(state == 0){
                state =1;
                check+=1;
            }
            else if(state == 1){
                state =0;
            }

        }
        cout<<"Best common value "<<(sorted_0mV[check-state]+sorted_31mV[63-check])/2<<" value of high: "<< sorted_31mV[check-state]<<" value of low"<<sorted_0mV[63-check]<<endl;
        r_mean->at(m).calib_value = (sorted_0mV[check-state]+sorted_31mV[63-check])/2;
        for(unsigned int i =0; i<64; i++){
            r_mean->at(m).v_calval.push_back(r_mean->at(m).calib_value);
        }
    }
}

void calibration_module::GetCalSetting(){
    // function to calculate the best bin value for each channel
    if(calibrun=="writeconfig")root_main->daqwindow->LoadConfig("Calib_config");

//    }
    for(int m=0; m<act_vmm.size(); m++){
        for(unsigned int i =0; i<64; i++){
            double difference = 9999.;
            int bin_number = 0;
            for(int j = 0; j<number_bits; j++){
                double diff = pow(pow(r_mean->at(m).calib_value - r_mean->at(m).y_fullrange.at(j).at(i),2),0.5);
                if( diff < difference ) {
                    difference = diff;
                    bin_number = j;
                }
                if(j == number_bits-1){
                    int hdmi =(act_vmm[m]/2) ;
                    int vmm = act_vmm[m]%2;
                    string regi = "ADC0_10";
                    if(calibmode == "TDC") regi = "ADC0_8";
                    root_main->daq[0].fec[0].hdmi[hdmi].hybrid[0].vmm[vmm].SetRegi(regi, bin_number , i );
                }
            }
        }
    }
    if(calibrun=="writeconfig"){
        root_main->daqwindow->ui->line_configFile->setText("Calib_config");
        emit root_main->daqwindow->ui->Button_save->clicked();
        root_main->daqwindow->ui->line_configFile->setText("");
        root_main->daqwindow->LoadConfig("Calib_config");
    }


 }

void calibration_module::connectDAQSocket()
{
    stringstream sx;

    int daqport = 6006;

    if(!m_DAQSocket) {
        msg()("Initializing DAQ socket...","calibration_module::connectDAQSocket");
        m_DAQSocket = new QUdpSocket();
        connect(m_DAQSocket, SIGNAL(readyRead()), this, SLOT(readEvent()));
    }

    if(m_DAQSocket->state() == QAbstractSocket::UnconnectedState) {
        if(dbg()){
            sx << "About to re-bind DAQ socket";
            msg()(sx,"calibration_module::connectDAQSocket"); sx.str("");
        }
        bool bnd = m_DAQSocket->bind(daqport, QUdpSocket::ShareAddress);
        if(!bnd) {
            sx << "ERROR Unable to re-bind DAQ socket to port " << daqport;
            msg()(sx, "calibration_module::connectDAQSocket"); sx.str("");
            if(dbg()) {
                sx << "Closing and disconnecting DAQ socket";
                msg()(sx,"calibration_module::connectDAQSocket"); sx.str("");
            }
            m_DAQSocket->close();
            m_DAQSocket->disconnectFromHost();
        } // not bnd correctly
        else {

            if(dbg()) {
                sx << "DAQ socket successfully bound to port " << daqport;
                msg()(sx,"calibration_module::connectDAQSocket"); sx.str("");
            }
        } // bnd ok
    }
}
// ------------------------------------------------------------------------ //
void calibration_module::closeDAQSocket()
{
    // close the socket
    if(dbg()) msg()("Closing DAQ socket", "calibration_module::closeDAQSocket");
    m_DAQSocket->close();
    m_DAQSocket->disconnectFromHost();
}
void calibration_module::readEvent()
{
    stringstream sx;

    bool ok_to_read = true;

    QHostAddress vmmip;
    QByteArray datagram;
    //datagram.clear();

    while(m_DAQSocket->hasPendingDatagrams()) {

        datagram.resize(m_DAQSocket->pendingDatagramSize());
        m_DAQSocket->readDatagram(datagram.data(), datagram.size(), &vmmip);
            decodeAndWriteData(datagram);

    } // while loop
    return;
}
// ------------------------------------------------------------------------ //
void calibration_module::decodeAndWriteData(const QByteArray& datagram)
{
    bool ok;
   stringstream sx;

   bool verbose = false;

   QString frameCounterStr = datagram.mid(0,4).toHex(); //Frame Counter

   ///////////////////////////////////////
   //event data incoming from chip
   ///////////////////////////////////////
   if(frameCounterStr != "fafafafa") {
       QString headerID = datagram.mid(4,3).toHex();

       ///////////////////////////////////////////////
       // readout the VMM2 event data [begin]
       ///////////////////////////////////////////////
       if(headerID == "564d32") {

           QString fullEventDataStr, headerStr, chipNumberStr, trigCountStr, trigTimeStampStr;
           chipNumberStr    = datagram.mid(7,1).toHex();
           trigCountStr     = datagram.mid(8,2).toHex();
           trigTimeStampStr = datagram.mid(10,2).toHex();
            int chipnumber = chipNumberStr.toInt(&ok,16);
           if(dbg()) {
//           if(true){
               sx.str("");
               headerStr        = datagram.mid(4,4).toHex();
               fullEventDataStr = datagram.mid(12, datagram.size()).toHex();
               sx << "*****************************************************\n"
                  << " Data from chip # : " << chipNumberStr.toInt(&ok,16) << "\n" //toStdString() << "\n"
                  << "  > Header        : " << headerStr.toStdString() << "\n"
                  << "  > Data          : " << fullEventDataStr.toStdString() << "\n"
                  << "*****************************************************";
               cout << sx.str() << endl;
               //msg()(sx,"calibration_module::decodeAndWriteData");
           } //dbg

           if(datagram.size()==12 && dbg()) {
               sx.str("");
               sx << "Empty event from chip #: " << chipNumberStr.toInt(&ok,16);
               cout << sx.str() << endl;
               msg()(sx,"calibration_module::decodeAndWriteData"); sx.str("");
           }
           if(datagram.size()==12) return;
           if(chipnumber==act_vmm[0]) Counting();
           // data containers for this chip
           _pdo.clear();
           _tdo.clear();
           _bcid.clear();
           _gray.clear();
           _channelNo.clear();
           _flag.clear();
           _thresh.clear();
           _neighbor.clear();

           // -------------- begin loop over chip's channels ---------------- //
           for(int i = 12; i < datagram.size(); ) {
               quint32 bytes1 = reverse32(datagram.mid(i, 4).toHex());
               quint32 bytes2 = reverse32(datagram.mid(i+4, 4).toHex());

               // --- flag --- //
               uint flag = (bytes2 & 0x1);
               _flag.push_back(flag);

               // --- threshold --- //
               uint threshold = (bytes2 & 0x2) >> 1;
               _thresh.push_back(threshold);

               // --- channel number --- //
               int channel_no = (bytes2 & 0xfc) >> 2; // 0xfc = 0000 0000 1111 1100

               if(calibRun()) {
                   if(m_channel_for_calib < 0) {
                       sx.str("");
                       sx << "Channel for calibration has not been set!\n"
                          << "Make sure that it is set correctly in"
                          << " calibration_module::setCalibrationChannel";
                       msg()(sx, "calibration_module::decodeAndWriteData"); sx.str("");
                       _neighbor.push_back(0);
                   }
                   else {
                       _neighbor.push_back(!(m_channel_for_calib == channel_no));
                   }
               }
               int unmapped_channel = channel_no;
               _channelNo.push_back(channel_no);

               // use QString methods instead of using bytes1 (for now)
               QString bytes1_str = "00000000000000000000000000000000"; // 32bit
               QString tmpStr     = "00000000000000000000000000000000"; // 32bit
               quint32 bytes1_ex  = datagram.mid(i, 4).toHex().toUInt(&ok,16);
               tmpStr = tmpStr.number(bytes1_ex,2);
               for(int j = 0; j < tmpStr.size(); j++) {
                   QString tmp = tmpStr.at(tmpStr.size()-1-j);
                   bytes1_str.replace(j,1,tmp); // bytes1_str is now QString of bytes1
               } // j

               // --- amplitude / pdo --- //
               QString q_1 = bytes1_str.left(8);
               QString q_2 = bytes1_str.mid(14,2);
               QString q_final;
               q_final.append(q_2);
               q_final.append(q_1);
               uint outCharge_ = 0;
               if(q_final.right(4)=="0000" && ignore16()) {
                   outCharge_ = 1025;
               } else {
                   outCharge_ = q_final.toUInt(&ok,2);
               }
               _pdo.push_back(outCharge_);

               // --- TAC / tdo --- //
               QString tac_1 = bytes1_str.mid(8,6);
               QString tac_2 = bytes1_str.mid(22,2);
               QString tac_final;
               tac_final.append(tac_2);
               tac_final.append(tac_1);
               uint outTac_ = tac_final.toUInt(&ok,2);
               _tdo.push_back(outTac_);

               // --- bcid --- //
               QString bcid_1 = bytes1_str.mid(16,6);
               QString bcid_2 = bytes1_str.mid(26,6);
               QString bcid_final;
               bcid_final.append(bcid_2);
               bcid_final.append(bcid_1);
               uint outBCID_ = bcid_final.toUInt(&ok,2);
               _bcid.push_back(outBCID_);

               // --- gray --- //
               uint gray = grayToBinary(outBCID_);
               _gray.push_back(gray);
                verbose=true;

               if(dbg() && verbose) {
               //if(true) {
                   sx.str("");
                   sx << "channel          : " << channel_no << "\n"
                      << "flag             : " << flag << "\n"
                      << "threshold        : " << threshold << "\n"
                      << "adc              : " << outCharge_ << "\n"
                      << "q_1              : " << q_1.toStdString() << "\n"
                      << "q_2              : " << q_2.toStdString() << "\n"
                      << "q_final          : " << q_final.toStdString() << "\n"
                      << "tac              : " << outTac_ << "\n"
                      << "bcid-gray        : " << gray << "\n"
                      << "bcid             : " << outBCID_ << "\n";
                   cout << sx.str() << endl;
                   //msg()(sx,"calibration_module::decodeAndWriteData");
                   //msg()(" "," ");
               } // dbg
                if(calibmode == QString("ADC")) (*v_calibvar)[chipnumber][channel_no].push_back(outCharge_);
                if(calibmode == QString("TDC")) (*v_calibvar)[chipnumber][channel_no].push_back(outTac_);
               // move to next channel (8 bytes forward)
               i += 8;
           } // i

       } // VMM2 event data
       ///////////////////////////////////////////////
       // readout the VMM2 event data [end]
       ///////////////////////////////////////////////


   } // != fafafafa

   ///////////////////////////////////////
   // reading out chip complete
   ///////////////////////////////////////


}
// ------------------------------------------------------------------------ //
quint32 calibration_module::reverse32(QString hex)
{
    bool ok;
    QBitArray received(32,false);
    QString bin, tmp;
    bin = tmp.number(hex.toUInt(&ok,16),2); // convert input to binary
    if(bin.size()>32) {
        cout << "calibration_module::reverse32    Input datagram is larger than 32 bits!" << endl;
        cout << "calibration_module::reverse32    >>> Exiting." << endl;
        exit(1);
    }
    // turn input array into QBitArray
    for(int i = 0; i < bin.size(); i++) {
        QString bit = bin.at(i);
        received.setBit(32-bin.size() + i, bit.toUInt(&ok,10)); // pad left with 0's
    } // i

    // now reverse
    QBitArray reversed(32, false);
    for(int j = 0; j < 32; j++) {
        reversed.setBit(31-j, received[j]);
    } // j

    // turn into QByteArray and return
    QByteArray reversed_byte = bitsToBytes(reversed);
    return reversed_byte.toHex().toUInt(&ok,16);
}

// ------------------------------------------------------------------------ //
uint calibration_module::grayToBinary(uint num)
{
    uint mask;
    for( mask = num >> 1; mask != 0; mask = mask >> 1)
    {
        num = num ^ mask;
    }
    return num;
}
// ------------------------------------------------------------------------ //
QByteArray calibration_module::bitsToBytes(QBitArray bits)
{
    QByteArray outbytes;
    outbytes.resize(bits.count()/8);
    outbytes.fill(0);

    for(int b = 0; b < bits.count(); ++b)
        outbytes[b/8] = ( outbytes.at(b/8) | ( (bits[b]?1:0) << (7-(b%8))));
    return outbytes;
}
// ------------------------------------------------------------------------ //
QBitArray calibration_module::bytesToBits(QByteArray bytes)
{
    QBitArray outbits;
    outbits.resize(bytes.count()*8);

    for(int i = 0; i < bytes.count(); ++i) {
        for(int b = 0; b < 8; ++b) {
            outbits.setBit( i*8+b, bytes.at(i)&(1<<(7-b)) );
        } // b
    } // i
    return outbits;
}
// ------------------------------------------------------------------------ //


