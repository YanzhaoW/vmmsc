#include "fec_config_module.h"

FECConfigModule::FECConfigModule(FEC *top, QObject *parent) :
    QObject(parent),
    m_fec{top},
    m_dbg(false),
    m_socketHandler(0),
    m_messageHandler(0)
  //    m_configHandler(0)
{

    //    std::cout<<"FEC -ip4 orig: "<<fec->GetRegVal("ip4") <<std::endl;
    //    std::cout<<"FEC -ip4 Set: "<<fec->SetReg("ip4",(unsigned long) 23) <<std::endl;
    //    std::cout<<"FEC -ip4 changed: "<<fec->GetRegVal("ip4") <<std::endl;
}

// ------------------------------------------------------------------------ //
void FECConfigModule::LoadMessageHandler(MessageHandler& m)
{
    m_messageHandler = &m;
}
// ------------------------------------------------------------------------- //
void FECConfigModule::VMMLoadEmit(){
    emit ReloadVMM();
}
// ------------------------------------------------------------------------ //
FECConfigModule& FECConfigModule::LoadSocket(SocketHandler& socket)
{
    m_socketHandler = &socket;
    if(!m_socketHandler) {
        GetMessageHandler()("ERROR SocketHandler instance is null", "Configuration::LoadSocket", true);
        //        return;
    }
    else if(IsDbgEnabled()) {
        GetMessageHandler()("SocketHandler instance loaded", "Configuration::LoadSocket");
        m_socketHandler->Print();
    }

    return *this;
}
// ------------------------------------------------------------------------ //
void FECConfigModule::SendConfig(int hdmi_index, int hybrid_index, int vmm_index)
{
    stringstream sx;
    // NEED TO ADD SOCKET STATE CHECK

    bool ok;

    // configuration of the VMMs
    int send_to_port = m_fec->GetRegVal("vmmasic_port");

    ///////////////////////////////////////////////////
    // build the configuration word(s) to be sent
    // to the front end
    ///////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    // Global SPI_1
    /////////////////////////////////////////////////
    std::vector<QString> globalRegisters;
    globalRegisters.clear();
    FillGlobalRegisters(globalRegisters, hdmi_index,  hybrid_index,  vmm_index);
    if(globalRegisters.size()!=3){
        GetMessageHandler()("ERROR Global SPI does not have 3 words", "FEC_config_module::SendConfig", true);
        return;
    }
    ///////////////////////////////////////////////////
    // Channel Registers
    ///////////////////////////////////////////////////
    std::vector<QString> channelRegisters;
    channelRegisters.clear();
    FillChannelRegisters(channelRegisters, hdmi_index,  hybrid_index,  vmm_index);
    if(channelRegisters.size()!=64){
        GetMessageHandler()("ERROR Channel registers do not have 64 values", "FEC_config_module::SendConfig", true);
        return;
    }
    ///////////////////////////////////////////////////
    // Global SPI_2
    ///////////////////////////////////////////////////
    std::vector<QString> globalRegisters2;
    globalRegisters2.clear();
    FillGlobalRegisters2(globalRegisters2, hdmi_index,  hybrid_index,  vmm_index);
    if(globalRegisters2.size()!=3){
        GetMessageHandler()("ERROR Global SPI does not have 3 words", "FEC_config_module::SendConfig", true);
        return;
    }
    ///////////////////////////////////////////////////
    // Now begin to send out the word
    ///////////////////////////////////////////////////
    QByteArray datagram;
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";
    unsigned int firstGlobalRegSPI_2  = 0;
    unsigned int lastGlobalRegSPI_2   = 2;
    unsigned int firstChRegSPI = 3;
    unsigned int lastChRegSPI  = 66;
    unsigned int firstGlobalRegSPI_1  = 67;
    unsigned int lastGlobalRegSPI_1   = 69;


    QString ip = m_fec->GetIP();
    // update global command counter

    GetSocketHandler().UpdateCommandCounter();
    //debug
    if(IsDbgEnabled()) {
        sx << "sending command SPI at comamnd #: " << GetSocketHandler().GetCommandCounter();
        GetMessageHandler()(sx,"Configuration::SendConfig");sx.str("");
    }
    // std::cout << "datagram clear" << std::endl;
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind


    QString chMapString = "0000000000000000";
    chMapString.replace( 15 - (hdmi_index*2+1-vmm_index) , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint32)chMap //[4,7]
        << (quint32)cmd.toUInt(&ok,16) //[8,11]
        << (quint32) 0; //[12,15]
    // global SPI / VMM3: global bank 0
    for(unsigned int i = firstGlobalRegSPI_2; i <= lastGlobalRegSPI_2; ++i) {
        out << (quint32)(i)
            << (quint32)globalRegisters2[i-firstGlobalRegSPI_2].toUInt(&ok,2);
    }

    //channel SPI
    for(unsigned int i = firstChRegSPI; i <= lastChRegSPI; ++i) {
        out << (quint32)(i)
            << (quint32)channelRegisters[i-firstChRegSPI].toUInt(&ok,2);
    } // i
    //[12,523]

    // global SPI / VMM3: global bank 1
    for(unsigned int i = firstGlobalRegSPI_1; i <= lastGlobalRegSPI_1; ++i) {
        out << (quint32)(i)
            << (quint32)globalRegisters[i-firstGlobalRegSPI_1].toUInt(&ok,2);
    } // i
    //[524,547]

    out << (quint32)128 //[548,551]
        << (quint32) 1; //[552,555]

    bool readOK = true;
    //debug
    if(IsDbgEnabled()) {
        sx.str("");
        sx << "Send config to port: " << send_to_port;
        GetMessageHandler()(sx);sx.str("");
    }
    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::SendConfig");
    //sleep(2);
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...", "FEC_config_module::SendConfig");
        GetSocketHandler().ProcessReply("fec", ip);
    }
    else {
        GetMessageHandler()("Timeout while waiting for replies from VMM", "FEC_config_module::SendConfig");
        GetSocketHandler().CloseAndDisconnect("fec","Configuration::SendConfig");
        return;
    }

    //send config
    // socket().closeAndDisconnect("fec","Configuration::SendConfig");
}
// ------------------------------------------------------------------------ //
void FECConfigModule::FillGlobalRegisters(std::vector<QString>& global, int hdmi_index, int hybrid_index, int vmm_index)
{
    stringstream sx;

    if(IsDbgEnabled())GetMessageHandler()("Loading global register configuration...", "Configuration::fillGlobalRegisters");

    global.clear();
    int sequence = 0;

    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 0
    //////////////////////////////////////////////////////////////////////

    QString spi0 = "00000000000000000000000000000000";
    QString tmp;

    //[0,3] reserved
    sequence+=4;

    //direct output IOs
    // [4]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slvs" ) ) );
    sequence++;

    // skips ch 16-47
    // [5]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"s32" ) ) );
    sequence++;

    //auto-reset at end ramp
    // [6]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"stcr" ) ) );
    sequence++;

    // ART flag synchronization
    // [7]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"ssart" ) ) );
    sequence++;

    // fast recovery from high charge
    // [8]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"srec" ) ) );
    sequence++;

    // mild tail cancellation
    // [9]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"stlc" ) ) );
    sequence++;

    // bipolar shape
    // [10]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sbip" ) ) );
    sequence++;

    // timing ramp at threshold
    // [11]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"srat" ) ) );
    sequence++;

    // fast reset at 6-b completion
    // [12]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sfrst" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckbc
    // [13]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slvsbc" ) ) );
    sequence++;

    // slvs 100 Ohm termination on cktp
    // [14]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slvstp" ) ) );
    sequence++;

    // slvs 100 Ohm termination on cktk
    // [15]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slvstk" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckdt
    // [16]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slvsdt" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckart
    // [17]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slvsart" ) ) );
    sequence++;

    // slvs 100 Ohm termination on cktki
    // [18]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slvstki" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckena
    // [19]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slvsena" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ck6b
    // [20]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slvs6b" ) ) );
    sequence++;

    // mixed signal functions
    // [21]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetRegVal("sL0enaV") ) );
    sequence++;

    //[22,29] not used
    sequence+=8;

    // reset (1)
    // [30]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"reset1" ) ) );
    sequence++;

    // reset (2)
    // [31]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"reset2" ) ) );
    sequence++;







    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[0]: "<<spi0.toStdString();
        GetMessageHandler()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi0);


    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 1
    //////////////////////////////////////////////////////////////////////

    QString spi1 = "00000000000000000000000000000000";
    sequence = 0;

    //threshold DAC lowest 6 bits
    // [0,5]
    tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sdt" ) ,
                             10,2,QChar('0'));
    spi1.replace(sequence,1,tmp[4]);
    sequence += 1;
    spi1.replace(sequence,1,tmp[5]);
    sequence += 1;
    spi1.replace(sequence,1,tmp[6]);
    sequence += 1;
    spi1.replace(sequence,1,tmp[7]);
    sequence += 1;
    spi1.replace(sequence,1,tmp[8]);
    sequence += 1;
    spi1.replace(sequence,1,tmp[9]);
    sequence += 1;

    //pulse DAC
    // [6,15]
    tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sdp_2" ) ,
                             10,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //10bit ADC
    // [16,17]
    tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "convtime_10" ) ,2,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8bit ADC
    // [18,19]
    tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "convtime_8" ) ,2,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //6bit
    // [20,22]
    tmp = QString("%1").arg(  m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "convtime_6" )  ,3,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8-bit enable
    // [23]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"s8b" ) ) );
    sequence++;

    //6-bit enable
    // [24]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"s6b" ) ) );
    sequence++;

    //ADC enable
    // [25]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"s10b" ) ) );
    sequence++;

    //dual clock serialized
    // [26]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sdcks" ) ) );
    sequence++;

    //dual clock ART
    // [27]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sdcka" ) ) );
    sequence++;

    //dual clock 6-bit
    // [28]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sdck6b" ) ) );
    sequence++;

    //analog tri-states
    // [29]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sdrv" ) ) );
    sequence++;

    //timing out 2
    // [30]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"stpp" ) ) );
    sequence++;

    //[31] reserved


    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[1]: " << spi1.toStdString();
        GetMessageHandler()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi1);

    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 2
    //////////////////////////////////////////////////////////////////////

    QString spi2 = "00000000000000000000000000000000";
    sequence = 0; //was 16 for VMM2 (global reg only 96 - 16 = 80 bit)

    //polarity, sp
    //[0]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sp" ) ) );
    sequence++;

    //disable at peak, sdp
    //[1]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sdp" ) ) );
    sequence++;

    //analog monitor to pdo, sbmx
    //[2]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sbmx" ) ) );
    sequence++;

    //tdo buffer
    //[3]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sbft" ) ) );
    sequence++;

    //pdo buffer
    //[4]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sbfp" ) ) );
    sequence++;

    //mo buffer
    //[5]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sbfm" ) ) );
    sequence++;

    //leakage current
    //[6]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"slg" ) ) );
    sequence++;

    //channel to monitor
    //[7,12]
    tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"monitoring" ) ,
                             6,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //multiplexer
    //[13]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"scmx" ) ) );
    sequence++;

    //ART enable
    //[14]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sfa" ) ) );
    sequence++;

    //ART mode
    //[15]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sfam" ) ) );
    sequence++;

    //peak_time
    // [16,17]
    tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"peaktime" ) ,2,2,QChar('0'));
    spi2.replace(sequence, tmp.size(),tmp);
    sequence += tmp.size();

    //double leakage (doubles the leakage current)
    // [18]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sfm" ) ) );
    sequence++;

    //gain
    // [19,21]
    tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"gain" ) ,3,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //neighbor trigger
    // [22]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sng" ) ) );
    sequence++;

    //direct outputs settings
    // [23]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"stot" ) ) );
    sequence++;

    //direct timing
    // [24]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sttt" ) ) );
    sequence++;

    //sub-hysteresis
    // [25]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"ssh" ) ) );
    sequence++;

    //TAC slope adjustment
    // [26,27]
    tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"stc" ) ,
                             2,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //threshold DAC highest 4 bits
    // [28,31]
    tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sdt" ) ,
                             10,2,QChar('0'));
    spi2.replace(sequence,1,tmp[0]);
    sequence += 1;
    spi2.replace(sequence,1,tmp[1]);
    sequence += 1;
    spi2.replace(sequence,1,tmp[2]);
    sequence += 1;
    spi2.replace(sequence,1,tmp[3]);
    sequence += 1;


    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[2]: "<<spi2.toStdString();
        GetMessageHandler()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi2);

}
// ------------------------------------------------------------------------ //
void FECConfigModule::FillChannelRegisters(std::vector<QString>& registers, int hdmi_index, int hybrid_index, int vmm_index)
{
    stringstream sx;
    if(IsDbgEnabled())GetMessageHandler()("Loading channel configuration...", "Configuration::fillChannelRegisters");

    registers.clear();
    int sequence;
    QString tmp;
    QString reg;

    bool do_check = false;
    for(int i = 0; i < 64; ++i){
        sequence=8;
        reg = "00000000000000000000000000000000";

        //SC [8]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "sc", i ) ) );
        sequence++;
        if(do_check) std::cout << " SC : " << reg.toStdString() << std::endl;

        //SL [9]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "sl", i ) ) );
        sequence++;
        if(do_check) std::cout << " SL : " << reg.toStdString() << std::endl;

        //ST [10]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "st", i ) ) );
        sequence++;
        if(do_check) std::cout << " ST : " << reg.toStdString() << std::endl;

        //STH [11]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "sth", i ) ) );
        sequence++;
        if(do_check) std::cout << " STH : " << reg.toStdString() << std::endl;

        //SM [12]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "sm", i ) ) );
        sequence++;
        if(do_check) std::cout << " SM : " << reg.toStdString() << std::endl;

        //SMX [13]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "smx", i ) ) );
        sequence++;
        if(do_check) std::cout << " SMX : " << reg.toStdString() << std::endl;

        //trim [14,18]
        tmp = "0000";
        tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "sd", i ) ,
                                 5,2,QChar('0'));
        //According to ATLAS software, still needed
        std::reverse(tmp.begin(),tmp.end()); //bug in VMM2, needs to be reversed
        reg.replace(sequence, tmp.size(), tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " TRIM : " << reg.toStdString() << std::endl;



        //10 bit adc lsb [19,23]
        tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "ADC0_10", i ) ,
                                 5,2,QChar('0'));
        //According to ATLAS software, has to be reversed
        std::reverse(tmp.begin(),tmp.end());
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 10bit : " << reg.toStdString() << std::endl;

        //8 bit adc lsb [24,27]
        tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "ADC0_8", i ) ,
                                 4,2,QChar('0'));
        //According to ATLAS software, has to be reversed
        std::reverse(tmp.begin(),tmp.end());
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 8bit : " << reg.toStdString() << std::endl;

        //6 bit adc lsb [28,30]
        tmp = QString("%1").arg( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index, "ADC0_6", i ) ,
                                 3,2,QChar('0'));
        //According to ATLAS software, has to be reversed
        std::reverse(tmp.begin(),tmp.end());
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 6bit : " << reg.toStdString() << std::endl;

        //[31] not used

        if(m_dbg) {
            //            using boost::format;
            std::stringstream chan;
            chan.str("");
            //            chan << " Chan["<< format("%02i") % i <<"]: " << reg.toStdString();
            GetMessageHandler()(chan, "Configuration::fillChannelRegisters");
            //chan << format("%02i") % i;
            //
            //std::cout << "-----------------------------------------------" << std::endl;
            //std::cout << " Channel [" << chan.str() << "] register "
            //     << reg.toStdString() << std::endl;
        }

        registers.push_back(reg);

    } // i


}



void FECConfigModule::FillGlobalRegisters2(std::vector<QString>& global, int hdmi_index, int hybrid_index, int vmm_index)
{
    stringstream sx;

    if(IsDbgEnabled())GetMessageHandler()("Loading global register configuration...", "Configuration::fillGlobalRegisters");

    global.clear();
    int sequence = 0;

    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 0
    //////////////////////////////////////////////////////////////////////

    QString spi0 = "00000000000000000000000000000000";
    QString tmp;

    //[0,30] not used
    sequence+=31;

    // magic number on BCID
    // [31]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"nskipm_i" ) ) );
    sequence++;


    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[0]: "<<spi0.toStdString();
        GetMessageHandler()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi0);


    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 1
    //////////////////////////////////////////////////////////////////////

    QString spi1 = "00000000000000000000000000000000";
    sequence = 0;

    // clocks when L0 core disabled
    // [0]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetRegVal("sL0cktest") ) );
    sequence++;

    //invert DCK
    // [1]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sL0dckinv" ) ) );
    sequence++;

    //invert BCCLK
    // [2]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hdmi_index,  hybrid_index,  vmm_index,"sL0ckinv" ) ) );
    sequence++;

    //L0 core
    // [3]
    spi1.replace(sequence,1,
                 QString::number(  m_fec->GetRegVal( "sL0ena" ) ) );
    sequence++;

    //Max hits per L0
    // [4,9]
    tmp = QString("%1").arg( m_fec->GetRegVal( "truncate" ) ,
                             6,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    // # L0 triggers to skip on overflow
    // [10,16]
    tmp = QString("%1").arg( m_fec->GetRegVal( "nskip" ) ,
                             7,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    // size trigger window
    // [17,19]
    tmp = QString("%1").arg( m_fec->GetRegVal( "window" ) ,
                             3,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    // Channel tagging BC rollover
    // [20,31]
    tmp = QString("%1").arg( m_fec->GetRegVal( "rollover" ) ,
                             12,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();



    //fec->GetRegVal("sL0enaV")


    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[1]: " << spi1.toStdString();
        GetMessageHandler()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi1);

    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 2
    //////////////////////////////////////////////////////////////////////

    QString spi2 = "00000000000000000000000000000000";
    sequence = 0; //was 16 for VMM2 (global reg only 96 - 16 = 80 bit)

    // L0 BC offset
    // [0,11]
    tmp = QString("%1").arg( m_fec->GetRegVal( "l0offset" ) ,
                             12,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    // Channel tagging BC offset
    // [12,23]
    tmp = QString("%1").arg( m_fec->GetRegVal( "offset" ) ,
                             12,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //[24,31] not used

    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[2]: "<<spi2.toStdString();
        GetMessageHandler()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi2);

}


int FECConfigModule::Connect()
{


    QString ip =  m_fec->GetIP();

#ifdef __linux__
    int status_code = QProcess::execute("ping", QStringList()<<"-c1"<<ip);
#elif __APPLE__
    int status_code = QProcess::execute("ping", QStringList()<<"-t1"<<ip);
#endif
    //////////////////////////////////
    bool pingOK = false;
    if(status_code == 0){
        pingOK = true;
        GetSocketHandler().IsPinged(true);
        std::cout<<"Ping successful"<<std::endl;

        ///////////////////////////////////////////////////////
        // now that we know we are connected OK, add and bind
        // the sockets
        ///////////////////////////////////////////////////////
        GetSocketHandler().AddSocket("FEC", m_fec->GetRegVal("fec_port"), QUdpSocket::ShareAddress);
        //        connect(vmmRunModule, SIGNAL(checkLinks()), this, SLOT(writeFECStatus()));
        return 1;
    }
    else {
        pingOK = false;
        GetSocketHandler().IsPinged(false);
        std::cout<<"ERROR Unable to successfully ping the IP: "<<ip.toStdString()<<std::endl;
        return 0;

    }
}




void FECConfigModule::ConfigTP( int hdmi_index, int hybrid_index)
{
    if(IsDbgEnabled())GetMessageHandler()("Configuring the pulser...","FEC_config_module::configTP");

    bool ok;
    QByteArray datagram;

    // send call to s6 port
    int send_to_port = m_fec->GetRegVal("s6_port");

    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";

    QString ip = m_fec->GetIP();

    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    int tpskew = m_fec->m_hdmis[hdmi_index].m_hybrids[hybrid_index].GetReg("TP_skew");
    int tpwidth = m_fec->m_hdmis[hdmi_index].m_hybrids[hybrid_index].GetReg("TP_width");
    int tppolarity = m_fec->m_hdmis[hdmi_index].m_hybrids[hybrid_index].GetReg("TP_pol");

    ////////////////////////////
    // header
    ////////////////////////////
    QString hdmiMapString = "00000000";
    hdmiMapString.replace( 7 - hdmi_index , 1 , QString("1") );
    quint8 hdmiMap = (quint8)hdmiMapString.toInt(&ok,2);
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint32) hdmiMap //[4,7]
        << (quint32) cmd.toUInt(&ok,16); //[8,11]

    ////////////////////////////
    // command
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 2 //[16,19]
        << (quint32) (tpskew + (tpwidth*16) + (tppolarity*128)); //[20,23]


    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::configTP");

    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::configTP");
        GetSocketHandler().ProcessReply("fec",ip);
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::configTP", true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::configTP");
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::configTP");

}


// ------------------------------------------------------------------------ //
void FECConfigModule::SetEventHeaders(int hdmi_index, int hybrid_index, int vmm_index)
{
    if(IsDbgEnabled())GetMessageHandler()("Setting event headers...","FEC_config_module::setEventHeaders");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = m_fec->GetRegVal("vmmapp_port");
    //get settings
    const int bld_info = m_fec->GetRegVal("evbld_infodata");
    const int bld_mode = m_fec->GetRegVal("evbld_mode");
    bool highRes = m_fec->GetRegVal("highres");

    // headers
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";

    // setup the word
    quint32 evbldinfo = 0;
    if(bld_info==0)             evbldinfo = 0;
    else if(bld_info==1)        evbldinfo = 256;
    else if(bld_info==2)        evbldinfo = 512;
    else if(bld_info==3)        evbldinfo = 768;
    else if(bld_info==4)        evbldinfo = 1024;
    else if(bld_info==5)        evbldinfo = 1280;
    //quint32 evbldinfo = (quint32) 256*bld_info;
    quint32 evbldmode = (quint32)bld_mode;

    //resolution
    quint32 resolutionBits = 0;
    if(highRes)
        resolutionBits = 32768;

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 15 - (hdmi_index*2+1-vmm_index) , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint32) chMap //[4,7]
        << (quint32) cmd.toUInt(&ok,16) //[8,11]
        << (quint32) 0; //[12,15]

    ///////////////////////////
    // event header
    ///////////////////////////
    out << (quint32) 10 //[16,19]
        << (quint32) evbldmode //[20,23]
        << (quint32) 12 //[24,27]
        << (quint32) (evbldinfo + resolutionBits); //[28,31]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::setEventHeaders");
    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::setEventHeaders");
        GetSocketHandler().ProcessReply("fec", ip);
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::setEventHeaders",true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::setEventHeaders");
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::setEventHeaders");
}


// ------------------------------------------------------------------------ //
void FECConfigModule::SetTriggerAcqConstants(int hdmi_index, int hybrid_index, int vmm_index)
{
    if(IsDbgEnabled())GetMessageHandler()("Sending trigger ACQ constants...","FEC_config_module::setTriggerAcqConstants");

    bool ok;
    QByteArray datagram;

    // send T/DAQ constants to VMMAPP port
    int send_to_port = m_fec->GetRegVal("vmmapp_port");

    QString ip = m_fec->GetIP();

    GetSocketHandler().UpdateCommandCounter();

    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); // rewind

    ///////////////////////////
    // header info
    ///////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 15 - (hdmi_index*2+1-vmm_index) , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    QString cmd, cmdType, cmdLength, msbCounter;
    cmd         = "AA";
    cmdType     = "AA";
    cmdLength   = "FFFF";
    msbCounter  = "0x80000000";
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) chMap //[6,7]
           //<< (quint32) config().getHDMIChannelMap() //[8,11]
        << (quint8)  cmd.toUInt(&ok,16) //[8]
        << (quint8)  cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok, 16); //[10,11]

    ///////////////////////////
    // trigger constants
    ///////////////////////////
    out << (quint32) 0 //[12,15]
           //trigger period
        << (quint32) 2 //[16,19]
        << (quint32) m_fec->GetRegVal("trigger_period") //[20,23]
           //pulser delay
        << (quint32) 4 //[24,27]
        << (quint32) m_fec->GetRegVal("tp_delay") //[28,31]
           //acq. sync
        << (quint32) 5 //[32,35]
        << (quint32) m_fec->GetRegVal("acq_sync") //[36,39]
           //acq. window
        << (quint32) 6 //[40,43]
        << (quint32) m_fec->GetRegVal("acq_window") //[44,47]
           //bcid reset
        << (quint32) 9 //[48,51]
        << (quint32) m_fec->GetRegVal("bcid_reset"); //[52,55]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::setTriggerAcqConstants");

    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::setTriggerAcqConstants");
        GetSocketHandler().ProcessReply("fec", ip);
    }
    else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::setTriggerAcqConstants", true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::setTriggerAcqConstants");
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::setTriggerAcqConstants");
}


// ------------------------------------------------------------------------ //
void FECConfigModule::SetTriggeredMode(int hdmi_index, int hybrid_index, int vmm_index)
{
    if(IsDbgEnabled())GetMessageHandler()("Sending trigger ACQ constants...","FEC_config_module::setTriggerAcqConstants");

    bool ok;
    QByteArray datagram;

    // send T/DAQ constants to VMMAPP port
    int send_to_port = m_fec->GetRegVal("vmmapp_port");

    QString ip = m_fec->GetIP();

    GetSocketHandler().UpdateCommandCounter();

    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); // rewind

    ///////////////////////////
    // header info
    ///////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 15 - (hdmi_index*2+1-vmm_index) , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    QString cmd, cmdType, cmdLength, msbCounter;
    cmd         = "AA";
    cmdType     = "AA";
    cmdLength   = "FFFF";
    msbCounter  = "0x80000000";
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) chMap //[6,7]
           //<< (quint32) config().getHDMIChannelMap() //[8,11]
        << (quint8)  cmd.toUInt(&ok,16) //[8]
        << (quint8)  cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok, 16); //[10,11]

    quint32 val = 0x00000000;
    quint32 val2 = 0x00000000;
    if(m_fec->GetRegVal("triggered_mode") == 1)
    {
        val = 0xC0000000;
        val += (m_fec->GetRegVal("time_offset_triggerperiod") << 24);
        val += (m_fec->GetRegVal("time_offset_BCID") << 12);
        val += (m_fec->GetRegVal("time_window_BCID"));
        val2 = (m_fec->GetRegVal("trigger_pulse_delay") << 8);
     }

    ///////////////////////////
    // trigger constants
    ///////////////////////////
    out << (quint32) 0 //[12,15]
           //triggered mode
        << (quint32) 11 //[16,19]
        << (quint32) val //[20,23]
        << (quint32) 15 //[24,27]
        << (quint32) val2; //[28,31]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec", "FEC_config_module::SetTriggeredMode");

    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::SetTriggeredMode");
        GetSocketHandler().ProcessReply("fec", ip);
    }
    else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::SetTriggeredMode", true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::SetTriggeredMode");
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::SetTriggeredMode");
}


// ------------------------------------------------------------------------ //
void FECConfigModule::SetS6clocks(int hdmi_index, int hybrid_index)
{
    if(IsDbgEnabled())GetMessageHandler()("Setting S6 clocks...","FEC_config_module::s6clocks");

    bool ok;
    QByteArray datagram;

    //get settings
    int cktk = m_fec->m_hdmis[hdmi_index].m_hybrids[hybrid_index].GetReg("CKTK");
    int ckbc = m_fec->m_hdmis[hdmi_index].m_hybrids[hybrid_index].GetReg("CKBC");
    int ckbc_skew = m_fec->m_hdmis[hdmi_index].m_hybrids[hybrid_index].GetReg("CKBC_skew");
    int ckdt = m_fec->m_hdmis[hdmi_index].m_hybrids[hybrid_index].GetReg("CKDT");

    // send call to s6 port
    int send_to_port = m_fec->GetRegVal("s6_port");

    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    QString hdmiMapString = "00000000";
    hdmiMapString.replace(7 -  hdmi_index , 1 , QString("1") );
    quint8 hdmiMap = (quint8)hdmiMapString.toInt(&ok,2);

    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint32) hdmiMap //[4,7]
        << (quint32) cmd.toUInt(&ok,16); //[8,11]

    ////////////////////////////
    // command
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 6 //[16,19]
        << (quint32) (cktk*16) //[20,23]
        << (quint32) 7 //[24,27]
           //<< (quint32) ( ckbc + (ckbc_skew*16) ); //[28,31]
        << (quint32) ( ckbc + (ckbc_skew*16) + 192) //[32,35] // +192 added for VMM3 to make ckbc high very short. Implemented in firmware: highest bits "11" 18.75 ns long high, "10"/"01" 25%/75% duty cycle, "00" is 50% duty cycle. From George: ckbc must be shorter than 20 ns and longer than 12.5 ns. With this hack, the ckbc of higher than 40 MHz will not work.
        << (quint32) 5 //[36,39], CKDT control register
        << (quint32) 0 + (ckdt*2); //[40,43] lowest bit: old clock selector, unused, put to 0, bits [1,2,3] CKDT selection (readout clock)

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::s6clocks");

    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::s6clocks");
        GetSocketHandler().ProcessReply("fec",ip);
    } else {
        GetMessageHandler()("Timout while waiting for replies from VMM",
                            "FEC_config_module::s6clocks", true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::s6clocks");
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::s6clocks");

}
// ------------------------------------------------------------------------ //
void FECConfigModule::SetS6Resets(int hdmi_index, int hybrid_index)
{
    if(IsDbgEnabled())GetMessageHandler()("Setting s6 reset settings...","FEC_config_module::setS6Resets");

    bool ok;
    QByteArray datagram;

    // send call to s6 port
    int send_to_port = m_fec->GetRegVal("s6_port");
    //get settings
    int s6_tk_pulses = m_fec->m_hdmis[hdmi_index].m_hybrids[hybrid_index].GetReg("TK_Pulses");
    bool set_s6_autoReset = false;
    bool set_s6_fecReset = false;
    int s6_fec_periodRest=m_fec->m_hdmis[hdmi_index].m_hybrids[hybrid_index].GetReg("period");

    // header
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";
    int s6_auto_reset = 0;
    int s6_fec_reset = 0;
    bool fec_reset = false;
    if(set_s6_autoReset) s6_auto_reset = 8;
    if(set_s6_fecReset) { s6_fec_reset = 32; fec_reset = true; }

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    QString hdmiMapString = "00000000";
    hdmiMapString.replace( 7 - hdmi_index , 1 , QString("1") );
    quint8 hdmiMap = (quint8)hdmiMapString.toInt(&ok,2);

    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint32) hdmiMap //[4,7]
        << (quint32) cmd.toUInt(&ok,16); //[8,11]

    ////////////////////////////
    // command
    ////////////////////////////
    out << (quint32) 0; //[12,15]

    out << (quint32) 9 //[16,19]
        << (quint32)( s6_tk_pulses + s6_auto_reset + s6_fec_reset); //[20,23]


    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::setS6Resets");

    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...", "FEC_config_module::setS6Resets");
        GetSocketHandler().ProcessReply("fec", ip);
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::setS6Resets", true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::setS6Resets");
        return;
    }

    ////////////////////////////////
    // set periodic reset
    ////////////////////////////////

    bool resetSeek = out.device()->reset();
    if(resetSeek) {
        //        emit s6resetStatus(true);
        GetSocketHandler().UpdateCommandCounter();
        out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16))
            << (quint32) hdmiMap
            << (quint32) cmd.toUInt(&ok,16);

        /////////////////////
        // command
        /////////////////////
        out << (quint32) 9
            << (quint32) s6_fec_periodRest;

        GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                        "FEC_config_module::setS6Resets");

        readOK = GetSocketHandler().WaitForReadyRead("fec");
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies [2]...", "FEC_config_module::setS6Resets");
            GetSocketHandler().ProcessReply("fec", ip);
        } else {
            GetMessageHandler()("Timeout while waiting for replies from VMM [2]",
                                "FEC_config_module::setS6Resets", true);
            GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::setS6Resets");
            return;
        } // readok
    } //resetSeek
    else {
        GetMessageHandler()("Error upon resetting datastream seek. Unable to send period reset command for FEC",
                            "FEC_config_module::setS6Resets");
        //        emit s6resetStatus(false);
    }

    GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::setS6Resets");

}
// ------------------------------------------------------------------------ //
void FECConfigModule::CheckLinkStatus(bool& readOK, QString & message)
{
    if(IsDbgEnabled())GetMessageHandler()("Checking link status...","FEC_config_module::checkLinkStatus");

    bool ok;
    QByteArray datagram;

    // send call to vmmapp port
    int send_to_port = m_fec->GetRegVal("vmmapp_port");

    // header
    QString cmd = "BBAAFFFF";
    QString msbCounter = "0x80000000";

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
           //        << (quint32) chMap //[4,7]
        << (quint32) m_fec->GetChMap() //[4,7]
        << (quint32) cmd.toUInt(&ok,16); //[8,11]

    ////////////////////////////
    // command
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 16; //[16,19]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::checkLinkStatus");

    readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");

    if(!readOK) {
        //emit CheckLinks();
        //if(dbg())GetMessageHandler()("Processing replies...","FEC_config_module::checkLinkStatus");
        //socket().processReply("fec", ip);
        //} else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::checkLinkStatus", true);
        GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::checkLinkStatus");
    }
    else
    {
        QByteArray buff;
        buff.clear();
        //buff.resize(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->GetSocketHandler().GetFECSocket().pendingDatagramSize() );
        // m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->GetSocketHandler().GetFECSocket().readDatagram(buff.data(), buff.size());
        buff.resize(GetSocketHandler().GetFECSocket().pendingDatagramSize() );
        GetSocketHandler().GetFECSocket().readDatagram(buff.data(), buff.size());
        if(buff.size()==0) return;
        bool ok;
        QString sizeOfPackageReceived, datagramCheck;
        datagramCheck = buff.mid(0,4).toHex();
        quint32 check = datagramCheck.toUInt(&ok,16);

        sizeOfPackageReceived = sizeOfPackageReceived.number(buff.size(),10);

        if(check<1000000) {
            stringstream ss;
            ss << " ****** NEW PACKET RECEIVED ****** " << endl;
            ss << " Data received size: " << sizeOfPackageReceived.toStdString()
               << " bytes" << endl;
            QString bin, hex;
            for(int i = 0; i < buff.size()/4; i++) {
                hex = buff.mid(i*4, 4).toHex();
                quint32 tmp32 = hex.toUInt(&ok,16);
                if(i==0) ss << " Rec'd ID: " << bin.number(tmp32,10).toStdString() << endl;
                else {
                    ss << " Data, " << i << ": " << bin.number(tmp32,16).toStdString() << endl;
                }
            } // i
            message = QString::fromStdString(ss.str());
            // m_linkState = QString::fromStdString(ss.str());
            // DisplayDebugScreen(QString::fromStdString(ss.str()));

            //        ui->debugScreen->append(QString::fromStdString(ss.str()));
            //        ui->debugScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        }
    }

}
// ------------------------------------------------------------------------ //
void FECConfigModule::ResetLinks()
{
    //    if(dbg())
    GetMessageHandler()("Resetting links...","FEC_config_module::resetLinks");

    bool ok;
    QByteArray datagram;

    // send call to vmmapp port
    int send_to_port = m_fec->GetRegVal("vmmapp_port");

    QString cmd, cmdType, cmdLength, msbCounter, cmdReset;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";
    cmdReset = "FF";

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ////////////////////////////
    // header (1)
    ////////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) m_fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ////////////////////////////
    // command (1)
    ////////////////////////////
    out << (quint32) 13 //[12,15]
        << (quint32) cmdReset.toUInt(&ok,16); //[16,19]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::resetLinks");

    datagram.clear();
    out.device()->seek(0);
    GetSocketHandler().UpdateCommandCounter();
    ////////////////////////////
    // header (2)
    ////////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) m_fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok, 16); //[10,11]

    ////////////////////////////
    // command (2)
    ////////////////////////////
    out << (quint32) 13 //[12,15]
        << (quint32) 0; //[16,19]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::resetLinks");

    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::resetLinks");

}
// ------------------------------------------------------------------------ //
void FECConfigModule::ResetFEC(bool do_reset)
{
    //    if(dbg())
    GetMessageHandler()("Resetting FEC...","FEC_config_module::resetFEC");

    bool ok;
    QByteArray datagram;

    // send reset call to FEC port
    int send_to_port = m_fec->GetRegVal("fec_port");
    // headers
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    // setup
    QString address = "FFFFFFFF";
    QString value = "";
    if(do_reset) {
        value = "FFFF8000";
        if(IsDbgEnabled())GetMessageHandler()("Rebooting FEC...","FEC_config_module::resetFEC");
    } else {
        value = "FFFF0001";
        if(IsDbgEnabled())GetMessageHandler()("WarmInit FEC...","FEC_config_module::resetFEC");
    }

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) m_fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ///////////////////////////
    // word
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) address.toUInt(&ok,16) //[16,19]
        << (quint32) value.toUInt(&ok,16); //[20,23]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::resetFEC");
    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::resetFEC");
        GetSocketHandler().ProcessReply("fec", ip);
    } else {
        if(IsDbgEnabled())GetMessageHandler()("Timeout while waiting for replies from VMM",
                                              "FEC_config_module::resetFEC",true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::resetFEC");
        //            exit(1);
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::resetFEC");
}
// ------------------------------------------------------------------------ //
void FECConfigModule::ReadSystemRegisters(QMap<QString, QString>& registers)
{
    //    if(dbg())
    GetMessageHandler()("Reading FEC system registers...","FEC_config_module::readSysReg");

    bool ok;
    QByteArray datagram;

    // send reset call to FEC port
    int send_to_port = m_fec->GetRegVal("fec_port");
    // headers
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "BB"; //read
    cmdType = "AA"; // list
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) m_fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ///////////////////////////
    // word
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0 //[12,15]
        << (quint32) 1 // FEC MAC part 1: Vendor identifier
        << (quint32) 2 // FEC MAC part 2: Device identifier
        << (quint32) 3 // FEC ip
        << (quint32) 4
        << (quint32) 5
        << (quint32) 6
        << (quint32) 7
        << (quint32) 8
        << (quint32) 9
        << (quint32) 10 //DAQ IP
        << (quint32) 11
        << (quint32) 12
        << (quint32) 13
        << (quint32) 14
        << (quint32) 15;

    QString testa= datagram.mid(0,datagram.size()).toHex();
    std::cout << datagram.size() << std::endl;
    std::cout << testa.toStdString() << std::endl;
    std::cout << " " << std::endl;
    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::ReadSystemRegisters");
    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");

    QByteArray read_datagram;
    while(GetSocketHandler().GetFECSocket().hasPendingDatagrams()) {

        read_datagram.resize(GetSocketHandler().GetFECSocket().pendingDatagramSize());
        GetSocketHandler().GetFECSocket().readDatagram(read_datagram.data(), read_datagram.size());
for(int n =0; n< read_datagram.size();n++)
 {
    std::cout << n << " " << read_datagram.mid(n,1).toHex().toStdString() << std::endl;
}

        QString FirmwareVers = read_datagram.mid(22,2).toHex();
        QString MACvendor = read_datagram.mid(29,3).toHex();
        QString MACdevice = read_datagram.mid(37,3).toHex();
        QString FECip = read_datagram.mid(44,4).toHex();
        QHostAddress theFECIP;
        bool ok;
        long lFECIP = FECip.toLong(&ok,16);
        theFECIP.setAddress(lFECIP);
        QString strFECip = theFECIP.toString();
        QString UDPdata = read_datagram.mid(54,2).toHex();
        QString UDPsc = read_datagram.mid(62,2).toHex();
        QString UDPdelay = read_datagram.mid(70,2).toHex();
        QString DATEflow = read_datagram.mid(78,2).toHex();
        QString Eth = read_datagram.mid(86,2).toHex();
        QString SCmode = read_datagram.mid(94,2).toHex();
        QString DAQip = read_datagram.mid(100,4).toHex();
        QHostAddress theDACIP;

        long lDACIP = DAQip.toLong(&ok,16);
        theDACIP.setAddress(lDACIP);
        QString strDACip = theDACIP.toString();

        QString DTClink = read_datagram.mid(108,4).toHex();
        QString MainClock = read_datagram.mid(116,4).toHex();
        QString MainClockStatus = read_datagram.mid(124,4).toHex();
        QString reserved = read_datagram.mid(132,4).toHex();
        QString FirmwareReg = read_datagram.mid(140,4).toHex();

        registers["FirmwareVers"] = FirmwareVers;
        registers["MACvendor"] = MACvendor;
        registers["MACdevice"] = MACdevice;
        registers["FECip"] = strFECip;
        registers["UDPdata"] = UDPdata;
        registers["UDPsc"] = UDPsc;
        registers["UDPdelay"] = UDPdelay;
        registers["DATEflow"] = DATEflow;
        registers["Eth"] = Eth;
        registers["SCmode"] = SCmode;
        registers["DAQip"] = strDACip;
        registers["DTClink"] = DTClink;
        registers["MainClock"] = MainClock;
        registers["MainClockStatus"] = MainClockStatus;
        registers["reserved"] = reserved;
        registers["FirmwareVers"] = FirmwareVers;


        stringstream sx;
        sx.str("");
        sx << "*****************************************************\n"
              //<< " Data from chip # : " << chipNumberStr.toInt(&ok,16) << "\n" //toStdString() << "\n"
           << "  > Firmware version         : " << FirmwareVers.toStdString() << "\n"
           << "  > MAC Vendor part          : " << MACvendor.toStdString() << "\n"
           << "  > MAC device part          : " << MACdevice.toStdString() << "\n"
           << "  > FEC ip                   : " << FECip.toStdString() << " (" << strFECip.toStdString() << ")" <<  "\n"
           << "  > UDP port data transfer   : " << UDPdata.toStdString() << "\n"
           << "  > UDP port slow-control    : " << UDPsc.toStdString() << "\n"
           << "  > Delay between UDP frames : " << UDPdelay.toStdString() << "\n"
           << "  > DATE flow control par.   : " << DATEflow.toStdString() << "\n"
           << "  > Enthernet control reg.   : " << Eth.toStdString() << "\n"
           << "  > Slow control control reg.: " << SCmode.toStdString() << "\n"
           << "  > DAQ destination ip       : " << DAQip.toStdString() << " (" << strDACip.toStdString() << ")" <<  "\n"
           << "  > DTC link control reg.    : " << DTClink.toStdString() << "\n"
           << "  > Main clock selection reg.: " << MainClock.toStdString() << "\n"
           << "  > Main clock status        : " << MainClockStatus.toStdString() << "\n"
           << "  > #reserved                : " << reserved.toStdString() << "\n"
           << "  > Firmware version reg.    : " << FirmwareReg.toStdString() << "\n"
           << "*****************************************************";
        cout << sx.str() << endl;

    } // while loop
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadSystemRegisters");
        GetSocketHandler().ProcessReply("fec", ip);
    } else {
        if(IsDbgEnabled())GetMessageHandler()("Timeout while waiting for replies from VMM",
                                              "FEC_config_module::ReadSystemRegisters",true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::ReadSystemRegisters");
        //            exit(1);
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::readSysReg");
    //writeFECip();
}

// ------------------------------------------------------------------------ //
void FECConfigModule::writeFECip(int FECip)
{
    //    if(dbg())
    GetMessageHandler()("Write new FEC ip...","FEC_config_module::writeFECip");

    bool ok;
    QByteArray datagram;

    // send reset call to FEC port
    int send_to_port = m_fec->GetRegVal("fec_sys_port");
    // headers
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA"; //write
    cmdType = "AA"; // pairs
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32) 0x80000000 //[0,3]
        << (quint16) 0xffff //[4,5]
        << (quint16) 0xffff //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]


    ///////////////////////////
    // word
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0xa0f30000 // FEC ip register
        << (quint32) FECip; // value 167772162=10.0.0.2

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::writeFECip");
    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled()) GetMessageHandler()("Processing replies...","FEC_config_module::writeFECip");
        GetSocketHandler().ProcessReply("fec", ip);
    } else {
        if(IsDbgEnabled()) GetMessageHandler()("Timeout while waiting for replies from VMM",
                                               "FEC_config_module::writeFECip",true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::writeFECip");
        //            exit(1);
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::writeFECip");
}
// ------------------------------------------------------------------------ //


// ------------------------------------------------------------------------ //
void FECConfigModule::writeDAQip(int DAQip)
{
    //    if(dbg())
    GetMessageHandler()("Write new DAQ ip...","FEC_config_module::writeDAQip");

    bool ok;
    QByteArray datagram;

    // send reset call to FEC port
    int send_to_port = m_fec->GetRegVal("fec_sys_port");

    // headers
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA"; //write
    cmdType = "AA"; // pairs
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    QString ip = m_fec->GetIP();


    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32) 0x80000000 //[0,3]
        << (quint16) 0xffff //[4,5]
        << (quint16) 0xffff //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]


    ///////////////////////////
    // word
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0x0a // DAQ ip register
        << (quint32) 0xC0A80003; // value 167772162=10.0.0.2

     std::cout << "WRITING TO  IP " << ip.toStdString() << " " << send_to_port << std::endl;
    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::writeDAQip");
    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    std::cout << "RESULT " << readOK << std::endl;

    if(readOK) {
        if(IsDbgEnabled()) GetMessageHandler()("Processing replies...","FEC_config_module::writeDAQip");
        GetSocketHandler().ProcessReply("fec", ip);
    } else {
        if(IsDbgEnabled()) GetMessageHandler()("Timeout while waiting for replies from VMM",
                                               "FEC_config_module::writeDAQip",true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::writeDAQip");
        //            exit(1);
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::writeDAQip");
}
// ------------------------------------------------------------------------ //



void FECConfigModule::SetReadoutMode()
{
    if(IsDbgEnabled())GetMessageHandler()("Setting trigger mode...","FEC_config_module::setTriggerMode");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = m_fec->GetRegVal("vmmapp_port");

    QString ip = m_fec->GetIP();

    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); // rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////

    QString cmd, cmdType, cmdLength, msbCounter;
    cmd         = "AA";
    cmdType     = "AA";
    cmdLength   = "FFFF";
    msbCounter  = "0x80000000";

    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) m_fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ///////////////////////////
    // trigger mode
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0; //[16,19]
    if(m_fec->GetRegVal("triggermode") == 0){//external trigger
        out << (quint32) 4; //[20,23]
        if(IsDbgEnabled())GetMessageHandler()("External trigger enabled","FEC_config_module::setTriggerMode");
    } // external
    else {
        out << (quint32) 7; //[20,23]
        if(IsDbgEnabled())GetMessageHandler()("Internal trigger enabled","FEC_config_module::setTriggerMode");
    }

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::setTriggerMode");

    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::setTriggerMode");
        GetSocketHandler().ProcessReply("fec", ip);
    }
    else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::setTriggerMode",true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::setTriggerMode");
        //        exit(1);
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::setTriggerMode");

}
// ------------------------------------------------------------------------ //
void FECConfigModule::ACQon(bool broadcast)
{
    if(IsDbgEnabled())GetMessageHandler()("Setting ACQ ON","FEC_config_module::ACQon");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = m_fec->GetRegVal("vmmapp_port");

    QString ip = m_fec->GetIP();
    if(broadcast){
        ip = "10.0.0.255";
    }
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd        = "AA";
    cmdType    = "AA";
    cmdLength  = "FFFF";
    msbCounter = "0x80000000";

    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0  //[4,5]
        << (quint16) m_fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ///////////////////////////
    // ACQ on
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 15 //[16,19]
        << (quint32) 1; //[20,23]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::ACQon");
    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...", "FEC_config_module::ACQon");
        GetSocketHandler().ProcessReply("fec", ip);
    } // readOK
    else {
        GetMessageHandler()("Timeout while waiting for replies from VMM","FEC_config_module::ACQon",true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::ACQon");
        //        exit(1);
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::ACQon");
}
// ------------------------------------------------------------------------ //
void FECConfigModule::ACQoff(bool broadcast)
{
    if(IsDbgEnabled())GetMessageHandler()("Setting ACQ OFF","FEC_config_module::ACQoff");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = m_fec->GetRegVal("vmmapp_port");

    QString ip = m_fec->GetIP();
    if(broadcast){
        ip = "10.0.0.255";
    }
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd         = "AA";
    cmdType     = "AA";
    cmdLength   = "FFFF";
    msbCounter  = "0x80000000";

    //stringstream sx;
    //sx << "AQCOFF command counter = " << socket().commandCounter();
    //msg()(sx);sx.str("");

    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) m_fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ///////////////////////////
    // ACQ off
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 15 //[16,19]
        << (quint32) 0; //[20,23]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::ACQoff [1]");
    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ACQoff");
        QByteArray buffer;
        buffer = GetSocketHandler().GetFECSocket().ProcessReply(ip, 0, GetSocketHandler().GetCommandCounter()); //.processReply("fec", ip);

        //QByteArray buffer = socket().buffer("fec");

        // dantrim May 26 not sure why this second word is sent -- legacy from VMM1/MCgill code?
        //QString bin, hex;
        //QDataStream out (&buffer, QIODevice::WriteOnly);
        //hex = buffer.mid(12,4).toHex();
        //quint32 tmp32 = DataHandler::ValueToReplaceHEX32(hex, 0, false);
        //out.device()->seek(12);
        //out << tmp32;
        //out.device()->seek(6);
        //out << (quint16) 2; // change to write mode ?
        //socket().SendDatagram(buffer, ip, send_to_port, "fec",
        //                                    "FEC_config_module::ACQoff [2]");
    }
    else {
        GetMessageHandler()("Timeout [1] while waiting for replies from VMM",
                            "FEC_config_module::ACQoff", true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::ACQoff");
        //        exit(1);
        return;
    }

    // not doing second loop
    //readOK = socket().waitForReadyRead("fec");
    //if(readOK) {
    //    socket().processReply("fec", ip);
    //}
    //else {
    //   GetMessageHandler()("Timeout [2] while waiting for replies from VMM",
    //            "FEC_config_module::ACQoff", true);
    //    socket().closeAndDisconnect("fec","FEC_config_module::ACQoff");
    //    exit(1);
    //}

    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::ACQoff");
}
// ------------------------------------------------------------------------ //
void FECConfigModule::SetMask()
{
    if(IsDbgEnabled())GetMessageHandler()("Setting HDMI mask and ART...","FEC_config_module::setMask");
    bool ok;
    QByteArray datagram;

    // send call to VMMAPP port
    int send_to_port = m_fec->GetRegVal("vmmapp_port");

    // header
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) m_fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ////////////////////////////
    // command
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 8 //[16,19]
        << (quint32) m_fec->GetChMap();//config().getHDMIChannelMapART(); //[20,23]
    //  << (quint32) config().getHDMIChannelMap(); //[20,23]

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::setMask");

    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::setMask");
        GetSocketHandler().ProcessReply("fec", ip);
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::setMask",true);
        GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::setMask");
        //        exit(1);
        return;
    }

    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::setMask");

}
// ------------------------------------------------------------------------ //
int FECConfigModule::ReadADC(int hdmi_index, int hybrid_index, int vmm_index, int adc_chan)
{
    if(IsDbgEnabled())GetMessageHandler()("Setting/reading i2c on hybrid...","FEC_config_module::ReadADC");

    bool ok;
    QByteArray datagram;

    // send call to i2c port
    int send_to_port = m_fec->GetRegVal("i2c_port");

    //header
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    //I2C address of ADC: 1001000/1001001
    int i2c_addr = 0;
    if (vmm_index % 2 == 0) i2c_addr = 72;
    else i2c_addr = 73;


    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    QString hdmiMapString = "00000000";
    hdmiMapString.replace(7 -  hdmi_index , 1 , QString("1") );
    quint8 hdmiMap = (quint8)hdmiMapString.toInt(&ok,2);

    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint8) hdmiMap//146 //[6] Subaddress: enable MUX channels
        << (quint8) ((i2c_addr << 1) | 0) //i2c_addr*2 + 0 //[7] ADC I2C address, last bit: read/not write (1 = read); ADC Addresses: 1001000/1001001 + read/write
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ///////////////////////////
    // word
    ///////////////////////////
    // calculate channel to read
    // ch 0: tdo        pattern 100 = 4
    // ch 1: pdo        patterm 101 = 5
    // ch 2: Mo         patterm 110 = 6
    // ch 3: not used   pattern 111 = 7
    // => adc_chan + 4

    out << (quint32) 0 //[12,15]
        << (quint32) 0 //[16,19] // goes to sc_address, do not use. can be used e.g. to redefine I2C addresses of in firmware
        << (quint32) (1*65536) +                      (1*32768 + (adc_chan+4)*4096 + 2*512 + 1*256) +                 131; //=116099; //[20,23] //goes to sc_data to ADC, must be 3 bytes to write to configuration register, 0 for setting address pointer to conversion register and anything but 0 to read
    // is 00000001 (point to conversion register) 11000101 (start conversion, channel 1, range +-2V, single shot) 10000011 (LSB of conversion register to be written)

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec", "FEC_config_module::ReadADC");

    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");

    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadADC");
        GetSocketHandler().ProcessReply("fec", ip);
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::ReadADC",true);
        GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::ReadADC");
        //        exit(1);
        return 0;
    }

    //////////////////////////////////////////////////////////////////
    // 2nd i2c command: set address pointer to converstion register //
    //////////////////////////////////////////////////////////////////
    datagram.clear();
    out.device()->seek(0); //rewind


    GetSocketHandler().UpdateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint8) hdmiMap//146 //[6] Subaddress: enable MUX channels
        << (quint8) ((i2c_addr << 1) | 0) //[7] ADC I2C address, last bit: read/not write (1 = read); ADC Addresses: 1001000/1001001 + read/write
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ////////////////////////////
    // word
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0 //[16,19] // goes to sc_address, do not use. can be used e.g. to redefine I2C addresses of in firmware
        << (quint32) 0; //[20,23] //goes to sc_data to ADC, must be 2 bytes to write to configuration register, 0 for setting address pointer to conversion register and anything but 0 to read

    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec", "FEC_config_module::ReadADC");

    readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadADC");
        GetSocketHandler().ProcessReply("fec",ip);
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::ReadADC", true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::ReadADC");
        //        exit(1);
        return 0;
    }
    ///////////////////////////////////////////////////////////
    // 3rd i2c command: read converions result form register //
    ///////////////////////////////////////////////////////////
    datagram.clear();
    out.device()->seek(0); //rewind


    GetSocketHandler().UpdateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint8) hdmiMap//146 //[6] Subaddress: enable MUX channels
        << (quint8) ((i2c_addr << 1) | 1) //[7] ADC I2C address, last bit: read/not write (1 = read); ADC Addresses: 1001000/1001001 + read/write
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ////////////////////////////
    // word
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0 //[16,19] // goes to sc_address, do not use. can be used e.g. to redefine I2C addresses of in firmware
        << (quint32) 11141120;//65535;//11141120; //7121558; //[20,23] //goes to sc_data to ADC, must be 3 bytes
    // can be anytinh here but needs to be 3 byte long
    GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "fec", "FEC_config_module::ReadADC");

    readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead("fec");

    QByteArray read_datagram;
    int ADCresult_int_bare = 0;
    while(GetSocketHandler().GetFECSocket().hasPendingDatagrams()) {

        read_datagram.resize(GetSocketHandler().GetFECSocket().pendingDatagramSize());
        GetSocketHandler().GetFECSocket().readDatagram(read_datagram.data(), read_datagram.size());


        QString ADCresult = read_datagram.mid(22,2).toHex();
        ADCresult_int_bare = ADCresult.toInt(&ok,16) >> 4; //bit shift
    } // while loop

    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadADC");
        GetSocketHandler().ProcessReply("fec",ip);
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::ReadADC", true);
        GetSocketHandler().CloseAndDisconnect("fec","FEC_config_module::ReadADC");
        //        exit(1);
        return 0;
    }
    GetSocketHandler().CloseAndDisconnect("fec", "FEC_config_module::ReadADC");

    //calculate level in mV from bare ADC result: 0-2047: pos (0 mV - 2047 mV), 2048-4094:neg(not possible)
    int ADCresult_int = ADCresult_int_bare; //easy in this case
    return ADCresult_int;
}
// ------------------------------------------------------------------------ //
