
// vmm
#include "configuration_module.h"

// std/stl
#include <iostream>
using namespace std;

// Qt
#include <QString>
#include <QDataStream>
#include <QByteArray>

// boost
#include <boost/format.hpp>

//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//  Configuration
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////
Configuration::Configuration(QObject *parent) :
    QObject(parent),
    m_dbg(false),
    m_socketHandler(0),
    m_configHandler(0),
    m_msg(0)
{
}
// ------------------------------------------------------------------------ //
void Configuration::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ------------------------------------------------------------------------ //
Configuration& Configuration::LoadConfig(ConfigHandler& config)
{
    m_configHandler = &config;
    if(!m_configHandler) {
        msg()("ERROR ConfigHandler instance is null", "Configuration::LoadConfig", true);
        exit(1);
    }
    else if(dbg()) {
        msg()("ConfigHandler instance loaded", "Configuration::LoadConfig");
    }
    return *this;
}
// ------------------------------------------------------------------------ //
Configuration& Configuration::LoadSocket(SocketHandler& socket)
{
    m_socketHandler = &socket;
    if(!m_socketHandler) {
        msg()("ERROR SocketHandler instance is null", "Configuration::LoadSocket", true);
        exit(1);
    }
    else if(dbg()) {
        msg()("SocketHandler instance loaded", "Configuration::LoadSocket");
        m_socketHandler->Print();
    }
        
    return *this;
}
// ------------------------------------------------------------------------ //
void Configuration::SendConfig()
{
    stringstream sx;

    // NEED TO ADD SOCKET STATE CHECK

    bool ok;

    // configuration of the VMMs
    int send_to_port = config().commSettings().vmmasic_port;

    ///////////////////////////////////////////////////
    // build the configuration word(s) to be sent
    // to the front end
    ///////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    // Global SPI
    ///////////////////////////////////////////////////
    std::vector<QString> globalRegisters;
    globalRegisters.clear();
    fillGlobalRegisters(globalRegisters);
    if(globalRegisters.size()!=3){
        msg()("ERROR Global SPI does not have 3 words", "Configuration::SendConfig", true);
        exit(1);
    }
    ///////////////////////////////////////////////////
    // Channel Registers
    ///////////////////////////////////////////////////
    std::vector<QString> channelRegisters;
    channelRegisters.clear();
    fillChannelRegisters(channelRegisters);
    if(channelRegisters.size()!=64){
        msg()("ERROR Channel registers do not have 64 values", "Configuration::SendConfig", true);
        exit(1);
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

    for(const auto& ip : socket().ipList()) {
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
    
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint32)config().getHDMIChannelMap() //[4,7]
            << (quint32)cmd.toUInt(&ok,16) //[8,11]
            << (quint32) 0; //[12,15]
        // global SPI / VMM3: global bank 0
        for(unsigned int i = firstGlobalRegSPI_2; i <= lastGlobalRegSPI_2; ++i) {
            out << (quint32)(i)
                << (quint32)0;
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
                                        "Configuration::SendConfig"); 
        //sleep(2);
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...", "Configuration::SendConfig");
            socket().processReply("fec", ip);
        }
        else {
            msg()("Timeout while waiting for replies from VMM", "Configuration::SendConfig");
            socket().closeAndDisconnect("fec","Configuration::SendConfig");
            exit(1);
        }
    } // ip

    //send config
   // socket().closeAndDisconnect("fec","Configuration::SendConfig");
}
// ------------------------------------------------------------------------ //
void Configuration::fillGlobalRegisters(std::vector<QString>& global)
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
    tmp = QString("%1").arg(config().globalSettings().threshold_dac,
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
    tmp = QString("%1").arg(config().globalSettings().test_pulse_dac,
                                                            10,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //10bit ADC
    // [16,17]
    tmp = QString("%1").arg(config().globalSettings().adc_10bit,2,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8bit ADC
    // [18,19]
    tmp = QString("%1").arg(config().globalSettings().adc_8bit,2,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //6bit
    // [20,21]
    tmp = QString("%1").arg(config().globalSettings().adc_6bit,3,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8-bit enable
    // [22]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().conv_mode_8bit));
    sequence++;

    //6-bit enable
    // [23]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().enable_6bit));
    sequence++;

    //ADC enable
    // [24]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().adcs));
    sequence++;

    //dual clock serialized
    // [25]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().dual_clock_data));
    sequence++;

    //dual clock ART
    // [26]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().dual_clock_art));
    sequence++;

    //dual clock 6-bit
    // [27]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().dual_clock_6bit));
    sequence++;

    //analog tri-states
    // [28]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().analog_tristates));
    sequence++;

    //timing out 2
    // [29]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().direct_time_mode0));

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

    //polarity
    //[0]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().polarity));
    sequence++;

    //disable at peak
    //[1]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().disable_at_peak));
    sequence++;

    //analog monitor to pdo
    //[2]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().monitor_pdo_out));
    sequence++;

    //tdo buffer
    //[3]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().out_buffer_tdo));
    sequence++;

    //pdo buffer
    //[4]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().out_buffer_pdo));
    sequence++;

    //mo buffer
    //[5]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().out_buffer_mo));
    sequence++;

    //leakage current
    //[6]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().leakage_current));
    sequence++;

    //channel to monitor
    //[7,12]
    tmp = QString("%1").arg(config().globalSettings().channel_monitor,
                                                    6,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //multiplexer
    //[13]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().monitoring_control));
    sequence++;

    //ART enable
    //[14]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().art));
    sequence++;

    //ART mode
    //[15]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().art_mode));
    sequence++;

    //peak_time
    // [16,17]
    tmp = QString("%1").arg(config().globalSettings().peak_time,2,2,QChar('0'));
    spi2.replace(sequence, tmp.size(),tmp);
    sequence += tmp.size();

    //double leakage (doubles the leakage current)
    // [18]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().double_leakage));
    sequence++;

    //gain
    // [19,21]
    tmp = QString("%1").arg(config().globalSettings().gain,3,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //neighbor trigger
    // [22]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().neighbor_trigger));
    sequence++;

    //direct outputs settings
    // [23]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().direct_time_mode1));
    sequence++;

    //direct timing
    // [24]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().direct_time));
    sequence++;

    //sub-hysteresis
    // [25]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().sub_hysteresis));
    sequence++;

    //TAC slope adjustment
    // [26,27]
    tmp = QString("%1").arg(config().globalSettings().tac_slope,
                                                            2,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //threshold DAC highest 4 bits
    // [28,21]
    tmp = QString("%1").arg(config().globalSettings().threshold_dac,
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
void Configuration::fillChannelRegisters(std::vector<QString>& registers)
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

        //SP [8]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).polarity));
        sequence++; 
        if(do_check) std::cout << " SP : " << reg.toStdString() << std::endl;

        //SC [9]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).capacitance));
        sequence++;
        if(do_check) std::cout << " SC : " << reg.toStdString() << std::endl;

        //SL [10]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).leakage_current));
        sequence++;
        if(do_check) std::cout << " SL : " << reg.toStdString() << std::endl;

        //ST [11]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).test_pulse));
        sequence++;
        if(do_check) std::cout << " ST : " << reg.toStdString() << std::endl;

        //SM [12]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).hidden_mode));
        sequence++;
        if(do_check) std::cout << " SM : " << reg.toStdString() << std::endl;

        //trim [13,16]
        tmp = "0000";
        tmp = QString("%1").arg(config().channelSettings(i).trim,
                                                    4,2,QChar('0'));
        std::reverse(tmp.begin(),tmp.end()); //bug in VMM2, needs to be reversed
        reg.replace(sequence, tmp.size(), tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " TRIM : " << reg.toStdString() << std::endl;

        //SMX [17]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).monitor));
        sequence++;
        if(do_check) std::cout << " SMX : " << reg.toStdString() << std::endl;

        //10 bit adc lsb [18,22]
        tmp = QString("%1").arg(config().channelSettings(i).s10bitADC,
                                                    5,2,QChar('0'));
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 10bit : " << reg.toStdString() << std::endl;

        //8 bit adc lsb [23,26]
        tmp = QString("%1").arg(config().channelSettings(i).s8bitADC,
                                                    4,2,QChar('0'));
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 8bit : " << reg.toStdString() << std::endl;

        //6 bit adc lsb [27,29]
        tmp = QString("%1").arg(config().channelSettings(i).s6bitADC,
                                                    3,2,QChar('0'));
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 6bit : " << reg.toStdString() << std::endl;
        
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
