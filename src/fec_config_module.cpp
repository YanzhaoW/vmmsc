#include "fec_config_module.h"

FEC_config_module::FEC_config_module(FEC *top, QObject *parent) :
    fec{top},
    QObject(parent),
    m_dbg(false),
    m_msg(0),
    m_socketHandler(0),
    m_configHandler(0)
{

    //    std::cout<<"FEC -ip4 orig: "<<fec->GetRegVal("ip4") <<std::endl;
//    std::cout<<"FEC -ip4 Set: "<<fec->SetReg("ip4",(unsigned long) 23) <<std::endl;
//    std::cout<<"FEC -ip4 changed: "<<fec->GetRegVal("ip4") <<std::endl;
}

// ------------------------------------------------------------------------ //
void FEC_config_module::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ------------------------------------------------------------------------- //

void FEC_config_module::testing(){
//        qDebug()<<"Polarity : "<<config().globalSettings().polarity;
        qDebug()<<"Polarity new : "<<fec->VMM_Get(0, 0, 0,"sp");
         qDebug()<<"IP address: "<< fec->GetIP();
         msg()("ERROR SocketHandler instance is null", "Configuration::LoadSocket", true);
}
FEC_config_module& FEC_config_module::LoadConfig(ConfigHandler& config)
{
    m_configHandler = &config;
    if(!m_configHandler) {
        msg()("ERROR ConfigHandler instance is null", "Configuration::LoadConfig", true);
//        return;
    }
    else if(dbg()) {
        msg()("ConfigHandler instance loaded", "Configuration::LoadConfig");
    }
    return *this;
}
// ------------------------------------------------------------------------ //
FEC_config_module& FEC_config_module::LoadSocket(SocketHandler& socket)
{
    m_socketHandler = &socket;
    if(!m_socketHandler) {
        msg()("ERROR SocketHandler instance is null", "Configuration::LoadSocket", true);
//        return;
    }
    else if(dbg()) {
        msg()("SocketHandler instance loaded", "Configuration::LoadSocket");
        m_socketHandler->Print();
    }

    return *this;
}
// ------------------------------------------------------------------------ //
void FEC_config_module::SendConfig(int hdmi_index, int hybrid_index, int vmm_index)
{
    stringstream sx;

    // NEED TO ADD SOCKET STATE CHECK

    bool ok;

    // configuration of the VMMs
    int send_to_port = fec->GetRegVal("vmmasic_port");

    ///////////////////////////////////////////////////
    // build the configuration word(s) to be sent
    // to the front end
    ///////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    // Global SPI_1
    /////////////////////////////////////////////////
    std::vector<QString> globalRegisters;
    globalRegisters.clear();
    fillGlobalRegisters(globalRegisters, hdmi_index,  hybrid_index,  vmm_index);
    if(globalRegisters.size()!=3){
        msg()("ERROR Global SPI does not have 3 words", "FEC_config_module::SendConfig", true);
        return;
    }
    ///////////////////////////////////////////////////
    // Channel Registers
    ///////////////////////////////////////////////////
    std::vector<QString> channelRegisters;
    channelRegisters.clear();
    fillChannelRegisters(channelRegisters, hdmi_index,  hybrid_index,  vmm_index);
    if(channelRegisters.size()!=64){
        msg()("ERROR Channel registers do not have 64 values", "FEC_config_module::SendConfig", true);
        return;
    }
    ///////////////////////////////////////////////////
    // Global SPI_2
    ///////////////////////////////////////////////////
    std::vector<QString> globalRegisters2;
    globalRegisters2.clear();
    fillGlobalRegisters2(globalRegisters2, hdmi_index,  hybrid_index,  vmm_index);
    if(globalRegisters2.size()!=3){
        msg()("ERROR Global SPI does not have 3 words", "FEC_config_module::SendConfig", true);
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


    QString ip = fec->GetIP();
    // update global command counter

    socket().updateCommandCounter();
    //debug
    if(dbg()) {
        sx << "sending command SPI at comamnd #: " << socket().commandCounter();
        msg()(sx,"Configuration::SendConfig");sx.str("");
    }
    std::cout << "datagram clear" << std::endl;
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind


    QString chMapString = "0000000000000000";
    chMapString.replace( 15 - (hdmi_index*2+vmm_index) , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
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
    if(dbg()) {
        sx.str("");
        sx << "Send config to port: " << send_to_port;
        msg()(sx);sx.str("");
    }
    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "FEC_config_module::SendConfig");
    //sleep(2);
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        if(dbg()) msg()("Processing replies...", "FEC_config_module::SendConfig");
        socket().processReply("fec", ip);
    }
    else {
        msg()("Timeout while waiting for replies from VMM", "FEC_config_module::SendConfig");
        socket().closeAndDisconnect("fec","Configuration::SendConfig");
        return;
    }

    //send config
   // socket().closeAndDisconnect("fec","Configuration::SendConfig");
}
// ------------------------------------------------------------------------ //
void FEC_config_module::fillGlobalRegisters(std::vector<QString>& global, int hdmi_index, int hybrid_index, int vmm_index)
{
    stringstream sx;

    if(dbg()) msg()("Loading global register configuration...", "Configuration::fillGlobalRegisters");

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
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slvs" ) ) );
    sequence++;

    // skips ch 16-47
    // [5]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"s32" ) ) );
    sequence++;

    //auto-reset at end ramp
    // [6]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"stcr" ) ) );
    sequence++;

    // ART flag synchronization
    // [7]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"ssart" ) ) );
    sequence++;

    // fast recovery from high charge
    // [8]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"srec" ) ) );
    sequence++;

    // mild tail cancellation
    // [9]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"stlc" ) ) );
    sequence++;

    // bipolar shape
    // [10]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sbip" ) ) );
    sequence++;

    // timing ramp at threshold
    // [11]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"srat" ) ) );
    sequence++;

    // fast reset at 6-b completion
    // [12]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sfrst" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckbc
    // [13]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slvsbc" ) ) );
    sequence++;

    // slvs 100 Ohm termination on cktp
    // [14]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slvstp" ) ) );
    sequence++;

    // slvs 100 Ohm termination on cktk
    // [15]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slvstk" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckdt
    // [16]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slvsdt" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckart
    // [17]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slvsart" ) ) );
    sequence++;

    // slvs 100 Ohm termination on cktki
    // [18]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slvstki" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckena
    // [19]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slvsena" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ck6b
    // [20]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slvs6b" ) ) );
    sequence++;

    // mixed signal functions
    // [21]
    spi0.replace(sequence,1,
        QString::number( fec->GetRegVal("sL0enaV") ) );
    sequence++;

    //[22,29] not used

    // reset (1)
    // [30]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"reset1" ) ) );
    sequence++;

    // reset (2)
    // [31]
    spi0.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"reset2" ) ) );
    sequence++;







    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[0]: "<<spi0.toStdString();
        msg()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi0);


    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 1
    //////////////////////////////////////////////////////////////////////

    QString spi1 = "00000000000000000000000000000000";
    sequence = 0;

    //threshold DAC lowest 6 bits
    // [0,5]
    tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sdt" ) ,
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
    tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sdp_2" ) ,
                                                            10,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //10bit ADC
    // [16,17]
    tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "convtime_10" ) ,2,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8bit ADC
    // [18,19]
    tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "convtime_8" ) ,2,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //6bit
    // [20,22]
    tmp = QString("%1").arg(  fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "convtime_6" )  ,3,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8-bit enable
    // [23]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"s8b" ) ) );
    sequence++;

    //6-bit enable
    // [24]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"s6b" ) ) );
    sequence++;

    //ADC enable
    // [25]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"s10b" ) ) );
    sequence++;

    //dual clock serialized
    // [26]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sdcks" ) ) );
    sequence++;

    //dual clock ART
    // [27]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sdcka" ) ) );
    sequence++;

    //dual clock 6-bit
    // [28]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sdck6b" ) ) );
    sequence++;

    //analog tri-states
    // [29]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sdrv" ) ) );
    sequence++;

    //timing out 2
    // [30]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"stpp" ) ) );
    sequence++;

   //[31] reserved


    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[1]: " << spi1.toStdString();
        msg()(sx,"Configuration::fillGlobalRegisters");
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
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sp" ) ) );
    sequence++;

    //disable at peak, sdp
    //[1]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sdp" ) ) );
    sequence++;

    //analog monitor to pdo, sbmx
    //[2]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sbmx" ) ) );
    sequence++;

    //tdo buffer
    //[3]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sbft" ) ) );
    sequence++;

    //pdo buffer
    //[4]
    spi2.replace(sequence,1,
       QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sbfp" ) ) );
    sequence++;

    //mo buffer
    //[5]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sbfm" ) ) );
    sequence++;

    //leakage current
    //[6]
    spi2.replace(sequence,1,
         QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"slg" ) ) );
    sequence++;

    //channel to monitor
    //[7,12]
    tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"monitoring" ) ,
                                                    6,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //multiplexer
    //[13]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"scmx" ) ) );
    sequence++;

    //ART enable
    //[14]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sfa" ) ) );
    sequence++;

    //ART mode
    //[15]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sfam" ) ) );
    sequence++;

    //peak_time
    // [16,17]
    tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"peaktime" ) ,2,2,QChar('0'));
    spi2.replace(sequence, tmp.size(),tmp);
    sequence += tmp.size();

    //double leakage (doubles the leakage current)
    // [18]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sfm" ) ) );
    sequence++;

    //gain
    // [19,21]
    tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"gain" ) ,3,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //neighbor trigger
    // [22]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sng" ) ) );
    sequence++;

    //direct outputs settings
    // [23]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"stot" ) ) );
    sequence++;

    //direct timing
    // [24]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sttt" ) ) );
    sequence++;

    //sub-hysteresis
    // [25]
    spi2.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"ssh" ) ) );
    sequence++;

    //TAC slope adjustment
    // [26,27]
    tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"stc" ) ,
                                                            2,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //threshold DAC highest 4 bits
    // [28,31]
    tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sdt" ) ,
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
        msg()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi2);

}
// ------------------------------------------------------------------------ //
void FEC_config_module::fillChannelRegisters(std::vector<QString>& registers, int hdmi_index, int hybrid_index, int vmm_index)
{
    stringstream sx;
    if(dbg()) msg()("Loading channel configuration...", "Configuration::fillChannelRegisters");

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
            QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "sc", i ) ) );
        sequence++;
        if(do_check) std::cout << " SC : " << reg.toStdString() << std::endl;

        //SL [9]
        reg.replace(sequence,1,
            QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "sl", i ) ) );
        sequence++;
        if(do_check) std::cout << " SL : " << reg.toStdString() << std::endl;

        //ST [10]
        reg.replace(sequence,1,
            QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "st", i ) ) );
        sequence++;
        if(do_check) std::cout << " ST : " << reg.toStdString() << std::endl;

        //STH [11]
        reg.replace(sequence,1,
            QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "sth", i ) ) );
        sequence++;
        if(do_check) std::cout << " STH : " << reg.toStdString() << std::endl;

        //SM [12]
        reg.replace(sequence,1,
            QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "sm", i ) ) );
        sequence++;
        if(do_check) std::cout << " SM : " << reg.toStdString() << std::endl;

        //SMX [13]
        reg.replace(sequence,1,
            QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "smx", i ) ) );
        sequence++;
        if(do_check) std::cout << " SMX : " << reg.toStdString() << std::endl;

        //trim [14,18]
        tmp = "0000";
        tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "sd", i ) ,
                                                    5,2,QChar('0'));
        std::reverse(tmp.begin(),tmp.end()); //bug in VMM2, needs to be reversed
        reg.replace(sequence, tmp.size(), tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " TRIM : " << reg.toStdString() << std::endl;



        //10 bit adc lsb [19,23]
        tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "ADC0_10", i ) ,
                                                    5,2,QChar('0'));
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 10bit : " << reg.toStdString() << std::endl;

        //8 bit adc lsb [24,27]
        tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "ADC0_8", i ) ,
                                                    4,2,QChar('0'));
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 8bit : " << reg.toStdString() << std::endl;

        //6 bit adc lsb [28,30]
        tmp = QString("%1").arg( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index, "ADC0_6", i ) ,
                                                    3,2,QChar('0'));
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 6bit : " << reg.toStdString() << std::endl;

        //[31] not used

        if(m_dbg) {
            using boost::format;
            std::stringstream chan;
            chan.str("");
            chan << " Chan["<< format("%02i") % i <<"]: " << reg.toStdString();
            msg()(chan, "Configuration::fillChannelRegisters");
            //chan << format("%02i") % i;
            //
            //std::cout << "-----------------------------------------------" << std::endl;
            //std::cout << " Channel [" << chan.str() << "] register "
            //     << reg.toStdString() << std::endl;
        }

        registers.push_back(reg);

    } // i


}

void FEC_config_module::fillGlobalRegisters2(std::vector<QString>& global, int hdmi_index, int hybrid_index, int vmm_index)
{
    stringstream sx;

    if(dbg()) msg()("Loading global register configuration...", "Configuration::fillGlobalRegisters");

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
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"nskipm_i" ) ) );
    sequence++;


    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[0]: "<<spi0.toStdString();
        msg()(sx,"Configuration::fillGlobalRegisters");
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
        QString::number( fec->GetRegVal("sL0cktest") ) );
    sequence++;

    //invert DCK
    // [1]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sL0dckinv" ) ) );
    sequence++;

    //invert BCCLK
    // [2]
    spi1.replace(sequence,1,
        QString::number( fec->VMM_Get( hdmi_index,  hybrid_index,  vmm_index,"sL0ckinv" ) ) );
    sequence++;

    //L0 core
    // [3]
    spi1.replace(sequence,1,
        QString::number(  fec->GetRegVal( "sL0ena" ) ) );
    sequence++;

    //Max hits per L0
    // [4,9]
    tmp = QString("%1").arg( fec->GetRegVal( "truncate" ) ,
                                                            6,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    // # L0 triggers to skip on overflow
    // [10,16]
    tmp = QString("%1").arg( fec->GetRegVal( "nskip" ) ,
                                                            7,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    // size trigger window
    // [17,19]
    tmp = QString("%1").arg( fec->GetRegVal( "window" ) ,
                                                            3,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    // Channel tagging BC rollover
    // [20,31]
    tmp = QString("%1").arg( fec->GetRegVal( "rollover" ) ,
                                                            12,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();



//fec->GetRegVal("sL0enaV")


    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[1]: " << spi1.toStdString();
        msg()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi1);

    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 2
    //////////////////////////////////////////////////////////////////////

    QString spi2 = "00000000000000000000000000000000";
    sequence = 0; //was 16 for VMM2 (global reg only 96 - 16 = 80 bit)

    // L0 BC offset
    // [0,11]
    tmp = QString("%1").arg( fec->GetRegVal( "l0offset" ) ,
                                                            12,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    // Channel tagging BC offset
    // [12,23]
    tmp = QString("%1").arg( fec->GetRegVal( "offset" ) ,
                                                            12,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //[24,31] not used

    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[2]: "<<spi2.toStdString();
        msg()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi2);

}


int FEC_config_module::Connect()
{


    QString ip =  fec->GetIP();

    #ifdef __linux__
    int status_code = QProcess::execute("ping", QStringList()<<"-c1"<<ip);
    #elif __APPLE__
    int status_code = QProcess::execute("ping", QStringList()<<"-t1"<<ip);
    #endif
    //////////////////////////////////
    bool pingOK = false;
    if(status_code == 0){
        pingOK = true;
        socket().pinged(true);
        std::cout<<"Ping successful"<<std::endl;

        ///////////////////////////////////////////////////////
        // now that we know we are connected OK, add and bind
        // the sockets
        ///////////////////////////////////////////////////////
        socket().addSocket("FEC", fec->GetRegVal("fec_port"), QUdpSocket::ShareAddress);
//        connect(vmmRunModule, SIGNAL(checkLinks()), this, SLOT(writeFECStatus()));
        return 1;
    }
        else {
            pingOK = false;
            socket().pinged(false);
            std::cout<<"ERROR Unable to successfully ping the IP: "<<ip.toStdString()<<std::endl;
            return 0;

        }
}




void FEC_config_module::configTP( int hdmi_index, int hybrid_index)
{
    if(dbg()) msg()("Configuring the pulser...","FEC_config_module::configTP");

    bool ok;
    QByteArray datagram;

    // send call to s6 port
    int send_to_port = fec->GetRegVal("s6_port");

    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";

    QString ip = fec->GetIP();

    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    socket().updateCommandCounter();

    int tpskew = fec->hdmi[hdmi_index].hybrid[hybrid_index].GetReg("TP_skew");
    int tpwidth = fec->hdmi[hdmi_index].hybrid[hybrid_index].GetReg("TP_width");
    int tppolarity = fec->hdmi[hdmi_index].hybrid[hybrid_index].GetReg("TP_pol");

    ////////////////////////////
    // header
    ////////////////////////////
    QString hdmiMapString = "00000000";
    hdmiMapString.replace( 7 - hdmi_index , 1 , QString("1") );
    quint8 hdmiMap = (quint8)hdmiMapString.toInt(&ok,2);
    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint32) hdmiMap //[4,7]
        << (quint32) cmd.toUInt(&ok,16); //[8,11]

    ////////////////////////////
    // command
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 2 //[16,19]
        << (quint32) (tpskew + (tpwidth*16) + (tppolarity*128)); //[20,23]


    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::configTP");

    bool readOK = true;
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        if(dbg()) msg()("Processing replies...","FEC_config_module::configTP");
        socket().processReply("fec",ip);
    } else {
        msg()("Timeout while waiting for replies from VMM",
                "FEC_config_module::configTP", true);
        socket().closeAndDisconnect("fec","FEC_config_module::configTP");
        return;
    }

    socket().closeAndDisconnect("fec","FEC_config_module::configTP");

}


// ------------------------------------------------------------------------ //
void FEC_config_module::setEventHeaders(int hdmi_index, int hybrid_index, int vmm_index)
{
    if(dbg()) msg()("Setting event headers...","FEC_config_module::setEventHeaders");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = fec->GetRegVal("vmmapp_port");
    //get settings
    const int bld_info = fec->GetRegVal("evbld_infodata");
    const int bld_mode = fec->GetRegVal("evbld_mode");
    bool highRes = fec->GetRegVal("highres");

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

    QString ip = fec->GetIP();
        datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    socket().updateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 15 - (hdmi_index*2+vmm_index) , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
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

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::setEventHeaders");
    bool readOK = true;
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        if(dbg()) msg()("Processing replies...","FEC_config_module::setEventHeaders");
        socket().processReply("fec", ip);
    } else {
        msg()("Timeout while waiting for replies from VMM",
                "FEC_config_module::setEventHeaders",true);
        socket().closeAndDisconnect("fec","FEC_config_module::setEventHeaders");
        return;
    }

    socket().closeAndDisconnect("fec", "FEC_config_module::setEventHeaders");
}


// ------------------------------------------------------------------------ //
void FEC_config_module::setTriggerAcqConstants(int hdmi_index, int hybrid_index, int vmm_index)
{
    if(dbg()) msg()("Sending trigger ACQ constants...","FEC_config_module::setTriggerAcqConstants");

    bool ok;
    QByteArray datagram;

    // send T/DAQ constants to VMMAPP port
    int send_to_port = fec->GetRegVal("vmmapp_port");

    QString ip = fec->GetIP();
    // UPDATE COUNTER, ETC... SHOULD NOW BE DONE
    // SOLELY IN SOCKETHANDLER TO WHICH WE PASS
    // THE DATAGRAMS

    socket().updateCommandCounter();

    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); // rewind

    ///////////////////////////
    // header info
    ///////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 15 - (hdmi_index*2+vmm_index) , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    QString cmd, cmdType, cmdLength, msbCounter;
    cmd         = "AA";
    cmdType     = "AA";
    cmdLength   = "FFFF";
    msbCounter  = "0x80000000";
    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
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
        << (quint32) fec->GetRegVal("trigger_period") //[20,23]
        //pulser delay
        << (quint32) 4 //[24,27]
        << (quint32) fec->GetRegVal("tp_delay") //[28,31]
        //acq. sync
        << (quint32) 5 //[32,35]
        << (quint32) fec->GetRegVal("acq_sync") //[36,39]
        //acq. window
        << (quint32) 6 //[40,43]
        << (quint32) fec->GetRegVal("acq_window") //[44,47]
        //bcid reset
        << (quint32) 9 //[48,51]
        << (quint32) fec->GetRegVal("bcid_reset"); //[52,55]

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                            "FEC_config_module::setTriggerAcqConstants");

    bool readOK = true;
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        if(dbg()) msg()("Processing replies...","FEC_config_module::setTriggerAcqConstants");
        socket().processReply("fec", ip);
    }
    else {
        msg()("Timeout while waiting for replies from VMM",
                    "FEC_config_module::setTriggerAcqConstants", true);
        socket().closeAndDisconnect("fec","FEC_config_module::setTriggerAcqConstants");
        return;
    }

    socket().closeAndDisconnect("fec","FEC_config_module::setTriggerAcqConstants");
}
// ------------------------------------------------------------------------ //
void FEC_config_module::s6clocks(int hdmi_index, int hybrid_index)
{
    if(dbg()) msg()("Setting S6 clocks...","FEC_config_module::s6clocks");

    bool ok;
    QByteArray datagram;

    //get settings
    int cktk = fec->hdmi[hdmi_index].hybrid[hybrid_index].GetReg("CKTK");
    int ckbc = fec->hdmi[hdmi_index].hybrid[hybrid_index].GetReg("CKBC");
    int ckbc_skew = fec->hdmi[hdmi_index].hybrid[hybrid_index].GetReg("CKBC_skew");

    // send call to s6 port
    int send_to_port = fec->GetRegVal("s6_port");

    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";

    QString ip = fec->GetIP();
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ////////////////////////////
        // header
        ////////////////////////////
        QString hdmiMapString = "00000000";
        hdmiMapString.replace(7 -  hdmi_index , 1 , QString("1") );
        quint8 hdmiMap = (quint8)hdmiMapString.toInt(&ok,2);

        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint32) hdmiMap //[4,7]
            << (quint32) cmd.toUInt(&ok,16); //[8,11]

        ////////////////////////////
        // command
        ////////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 6 //[16,19]
            << (quint32) (cktk*16) //[20,23]
            << (quint32) 7 //[24,27]
            << (quint32) ( ckbc + (ckbc_skew*16) ); //[28,31]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::s6clocks");

        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","FEC_config_module::s6clocks");
            socket().processReply("fec",ip);
        } else {
            msg()("Timout while waiting for replies from VMM",
                    "FEC_config_module::s6clocks", true);
            socket().closeAndDisconnect("fec","FEC_config_module::s6clocks");
            return;
        }

    socket().closeAndDisconnect("fec","FEC_config_module::s6clocks");

}
// ------------------------------------------------------------------------ //
void FEC_config_module::setS6Resets(int hdmi_index, int hybrid_index)
{
    if(dbg()) msg()("Setting s6 reset settings...","FEC_config_module::setS6Resets");

    bool ok;
    QByteArray datagram;

    // send call to s6 port
    int send_to_port = fec->GetRegVal("s6_port");
     //get settings
    int s6_tk_pulses = fec->hdmi[hdmi_index].hybrid[hybrid_index].GetReg("TK_Pulses");
    bool set_s6_autoReset = false;
    bool set_s6_fecReset = false;
    int s6_fec_periodRest=fec->hdmi[hdmi_index].hybrid[hybrid_index].GetReg("period");

    // header
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";
    int s6_auto_reset = 0;
    int s6_fec_reset = 0;
    bool fec_reset = false;
    if(set_s6_autoReset) s6_auto_reset = 8;
    if(set_s6_fecReset) { s6_fec_reset = 32; fec_reset = true; }

    QString ip = fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    socket().updateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    QString hdmiMapString = "00000000";
    hdmiMapString.replace( 7 - hdmi_index , 1 , QString("1") );
    quint8 hdmiMap = (quint8)hdmiMapString.toInt(&ok,2);

    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint32) hdmiMap //[4,7]
        << (quint32) cmd.toUInt(&ok,16); //[8,11]

    ////////////////////////////
    // command
    ////////////////////////////
    out << (quint32) 0; //[12,15]

    out << (quint32) 9 //[16,19]
        << (quint32)( s6_tk_pulses + s6_auto_reset + s6_fec_reset); //[20,23]

//    int s6_auto = 0;
//    int s6_fec = 0;
//    if(set_s6_autoReset) s6_auto = 8;
//    if(set_s6_fecReset) s6_fec = 16;

//    out << (quint32) 9 //[16,19]
//        << (quint32)( s6_tk_pulses + s6_auto + s6_fec); //[20,23]

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::setS6Resets");

    bool readOK = true;
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        if(dbg()) msg()("Processing replies...", "FEC_config_module::setS6Resets");
        socket().processReply("fec", ip);
    } else {
        msg()("Timeout while waiting for replies from VMM",
                                "FEC_config_module::setS6Resets", true);
        socket().closeAndDisconnect("fec","FEC_config_module::setS6Resets");
        return;
    }

    ////////////////////////////////
    // set periodic reset
    ////////////////////////////////

    bool resetSeek = out.device()->reset();
    if(resetSeek) {
//        emit s6resetStatus(true);
        socket().updateCommandCounter();
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16))
            << (quint32) hdmiMap
            << (quint32) cmd.toUInt(&ok,16);

        /////////////////////
        // command
        /////////////////////
        out << (quint32) 9
            << (quint32) s6_fec_periodRest;

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::setS6Resets");

        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies [2]...", "FEC_config_module::setS6Resets");
            socket().processReply("fec", ip);
        } else {
            msg()("Timeout while waiting for replies from VMM [2]",
                                "FEC_config_module::setS6Resets", true);
            socket().closeAndDisconnect("fec","FEC_config_module::setS6Resets");
            return;
        } // readok
    } //resetSeek
    else {
        msg()("Error upon resetting datastream seek. Unable to send period reset command for FEC",
                                    "FEC_config_module::setS6Resets");
//        emit s6resetStatus(false);
    }

    socket().closeAndDisconnect("fec","FEC_config_module::setS6Resets");

}
// ------------------------------------------------------------------------ //
void FEC_config_module::checkLinkStatus()
{
    if(dbg()) msg()("Checking link status...","FEC_config_module::checkLinkStatus");

    bool ok;
    QByteArray datagram;

    // send call to vmmapp port
    int send_to_port = fec->GetRegVal("vmmapp_port");

    // header
    QString cmd = "BBAAFFFF";
    QString msbCounter = "0x80000000";

    QString ip = fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    socket().updateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 0 , 1 , QString("1") );
//    chMapString.replace( hdmi_index*2+vmm_index , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
//        << (quint32) chMap //[4,7]
        << (quint32) fec->GetChMap() //[4,7]
        << (quint32) cmd.toUInt(&ok,16); //[8,11]

    ////////////////////////////
    // command
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 16; //[16,19]

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::checkLinkStatus");

    bool readOK = true;
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        emit checkLinks();
        //if(dbg()) msg()("Processing replies...","FEC_config_module::checkLinkStatus");
        //socket().processReply("fec", ip);
    } else {
        msg()("Timeout while waiting for replies from VMM",
                "FEC_config_module::checkLinkStatus", true);
        socket().closeAndDisconnect("fec", "FEC_config_module::checkLinkStatus");
//        return;
        return;
    }

    socket().closeAndDisconnect("fec", "FEC_config_module::checkLinkStatus");

}
// ------------------------------------------------------------------------ //
void FEC_config_module::resetLinks()
{
//    if(dbg())
        msg()("Resetting links...","FEC_config_module::resetLinks");

    bool ok;
    QByteArray datagram;

    // send call to vmmapp port
    int send_to_port = fec->GetRegVal("vmmapp_port");

    QString cmd, cmdType, cmdLength, msbCounter, cmdReset;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";
    cmdReset = "FF";

    QString ip = fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    socket().updateCommandCounter();

    ////////////////////////////
    // header (1)
    ////////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 0 , 1 , QString("1") );
//    chMapString.replace( hdmi_index*2+vmm_index , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ////////////////////////////
    // command (1)
    ////////////////////////////
    out << (quint32) 13 //[12,15]
        << (quint32) cmdReset.toUInt(&ok,16); //[16,19]

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::resetLinks");

    datagram.clear();
    out.device()->seek(0);
    socket().updateCommandCounter();
    ////////////////////////////
    // header (2)
    ////////////////////////////
    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok, 16); //[10,11]

    ////////////////////////////
    // command (2)
    ////////////////////////////
    out << (quint32) 13 //[12,15]
        << (quint32) 0; //[16,19]

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::resetLinks");

    socket().closeAndDisconnect("fec", "FEC_config_module::resetLinks");

}
// ------------------------------------------------------------------------ //
void FEC_config_module::resetFEC(bool do_reset)
{
//    if(dbg())
        msg()("Resetting FEC...","FEC_config_module::resetFEC");

    bool ok;
    QByteArray datagram;

    // send reset call to FEC port
    int send_to_port = fec->GetRegVal("fec_port");
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
        if(dbg()) msg()("Rebooting FEC...","FEC_config_module::resetFEC");
    } else {
        value = "FFFF0001";
        if(dbg()) msg()("WarmInit FEC...","FEC_config_module::resetFEC");
    }

    QString ip = fec->GetIP();
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ///////////////////////////
        // header info
        ///////////////////////////
        QString chMapString = "0000000000000000";
        chMapString.replace( 0 , 1 , QString("1") );
    //    chMapString.replace( hdmi_index*2+vmm_index , 1 , QString("1") );
        quint16 chMap = (quint16)chMapString.toInt(&ok,2);

        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5]
            << (quint16) fec->GetChMap() //[6,7]
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

        ///////////////////////////
        // word
        ///////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) address.toUInt(&ok,16) //[16,19]
            << (quint32) value.toUInt(&ok,16); //[20,23]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "FEC_config_module::resetFEC");
        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","FEC_config_module::resetFEC");
            socket().processReply("fec", ip);
        } else {
            if(dbg()) msg()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::resetFEC",true);
            socket().closeAndDisconnect("fec","FEC_config_module::resetFEC");
//            exit(1);
            return;
        }

    socket().closeAndDisconnect("fec", "FEC_config_module::resetFEC");
}
// ------------------------------------------------------------------------ //
void FEC_config_module::setTriggerMode()
{
    if(dbg()) msg()("Setting trigger mode...","FEC_config_module::setTriggerMode");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = fec->GetRegVal("vmmapp_port");

    QString ip = fec->GetIP();
    // UPDATE COUNTER, ETC... SHOULD NOW BE DONE
    // SOLELY IN SOCKETHANDLER TO WHICH WE PASS
    // THE DATAGRAMS

    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); // rewind

    socket().updateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 0 , 1 , QString("1") );
//    chMapString.replace( hdmi_index*2+vmm_index , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    QString cmd, cmdType, cmdLength, msbCounter;
    cmd         = "AA";
    cmdType     = "AA";
    cmdLength   = "FFFF";
    msbCounter  = "0x80000000";

    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ///////////////////////////
    // trigger mode
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0; //[16,19]
    if(fec->GetRegVal("triggermode") == 0){//external trigger
        out << (quint32) 4; //[20,23]
        if(dbg()) msg()("External trigger enabled","FEC_config_module::setTriggerMode");
    } // external
    else {
        out << (quint32) 7; //[20,23]
        if(dbg()) msg()("Internal trigger enabled","FEC_config_module::setTriggerMode");
    }

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                            "FEC_config_module::setTriggerMode");

    bool readOK = true;
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        if(dbg()) msg()("Processing replies...","FEC_config_module::setTriggerMode");
        socket().processReply("fec", ip);
    }
    else {
        msg()("Timeout while waiting for replies from VMM",
                    "FEC_config_module::setTriggerMode",true);
        socket().closeAndDisconnect("fec","FEC_config_module::setTriggerMode");
//        exit(1);
        return;
    }

    socket().closeAndDisconnect("fec","FEC_config_module::setTriggerMode");
}
// ------------------------------------------------------------------------ //
void FEC_config_module::ACQon()
{
    if(dbg()) msg()("Setting ACQ ON","FEC_config_module::ACQon");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = fec->GetRegVal("vmmapp_port");

    QString ip = fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    socket().updateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 0 , 1 , QString("1") );
//    chMapString.replace( hdmi_index*2+vmm_index , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    QString cmd, cmdType, cmdLength, msbCounter;
    cmd        = "AA";
    cmdType    = "AA";
    cmdLength  = "FFFF";
    msbCounter = "0x80000000";

    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0  //[4,5]
        << (quint16) fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ///////////////////////////
    // ACQ on
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 15 //[16,19]
        << (quint32) 1; //[20,23]

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                "FEC_config_module::ACQon");
    bool readOK = true;
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        if(dbg()) msg()("Processing replies...", "FEC_config_module::ACQon");
        socket().processReply("fec", ip);
    } // readOK
    else {
        msg()("Timeout while waiting for replies from VMM","FEC_config_module::ACQon",true);
        socket().closeAndDisconnect("fec","FEC_config_module::ACQon");
//        exit(1);
        return;
    }

    socket().closeAndDisconnect("fec", "FEC_config_module::ACQon");
}
// ------------------------------------------------------------------------ //
void FEC_config_module::ACQoff()
{
    if(dbg()) msg()("Setting ACQ OFF","FEC_config_module::ACQoff");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = fec->GetRegVal("vmmapp_port");

    QString ip = fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    socket().updateCommandCounter();

    ///////////////////////////
    // header info
    ///////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 0 , 1 , QString("1") );
//    chMapString.replace( hdmi_index*2+vmm_index , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    QString cmd, cmdType, cmdLength, msbCounter;
    cmd         = "AA";
    cmdType     = "AA";
    cmdLength   = "FFFF";
    msbCounter  = "0x80000000";

    //stringstream sx;
    //sx << "AQCOFF command counter = " << socket().commandCounter();
    //msg()(sx);sx.str("");

    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ///////////////////////////
    // ACQ off
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 15 //[16,19]
        << (quint32) 0; //[20,23]

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::ACQoff [1]");
    bool readOK = true;
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        if(dbg()) msg()("Processing replies...","FEC_config_module::ACQoff");
        QByteArray buffer;
        buffer = socket().fecSocket().processReply(ip, 0, socket().commandCounter()); //.processReply("fec", ip);

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
        msg()("Timeout [1] while waiting for replies from VMM",
                "FEC_config_module::ACQoff", true);
        socket().closeAndDisconnect("fec","FEC_config_module::ACQoff");
//        exit(1);
        return;
    }

    // not doing second loop
    //readOK = socket().waitForReadyRead("fec");
    //if(readOK) {
    //    socket().processReply("fec", ip);
    //}
    //else {
    //    msg()("Timeout [2] while waiting for replies from VMM",
    //            "FEC_config_module::ACQoff", true);
    //    socket().closeAndDisconnect("fec","FEC_config_module::ACQoff");
    //    exit(1);
    //}

    socket().closeAndDisconnect("fec", "FEC_config_module::ACQoff");
}
// ------------------------------------------------------------------------ //
void FEC_config_module::setMask()
{
    if(dbg()) msg()("Setting HDMI mask and ART...","FEC_config_module::setMask");
    bool ok;
    QByteArray datagram;

    // send call to vmmapp port
    int send_to_port = fec->GetRegVal("vmmapp_port");

    // header
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

   QString ip = fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    socket().updateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    QString chMapString = "0000000000000000";
    chMapString.replace( 0 , 1 , QString("1") );
//    chMapString.replace( hdmi_index*2+vmm_index , 1 , QString("1") );
    quint16 chMap = (quint16)chMapString.toInt(&ok,2);

    out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint16) fec->GetChMap() //[6,7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

    ////////////////////////////
    // command
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 8 //[16,19]
        << (quint32) fec->GetChMap();//config().getHDMIChannelMapART(); //[20,23]
      //  << (quint32) config().getHDMIChannelMap(); //[20,23]

    socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "FEC_config_module::setMask");

    bool readOK = true;
    readOK = socket().waitForReadyRead("fec");
    if(readOK) {
        if(dbg()) msg()("Processing replies...","FEC_config_module::setMask");
        socket().processReply("fec", ip);
    } else {
        msg()("Timeout while waiting for replies from VMM",
                "FEC_config_module::setMask",true);
        socket().closeAndDisconnect("fec", "FEC_config_module::setMask");
//        exit(1);
        return;
    }

    socket().closeAndDisconnect("fec", "FEC_config_module::setMask");

}
// ------------------------------------------------------------------------ //
