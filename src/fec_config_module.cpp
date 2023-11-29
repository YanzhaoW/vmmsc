#include <QThread>
#include <QMessageBox>
#include "fec_config_module.h"
#include "vmm_sys_regs_map.h"

FECConfigModule::FECConfigModule(FEC *top, QObject *parent) :
    QObject(parent),
    m_fec{top},
    m_dbg(false),
    m_socketHandler(0),
    m_messageHandler(0)
{
    g_hybrid_i2c.clear();
    if(g_clock_source <= 1) {
        g_hybrid_i2c.push_back(0);
        g_hybrid_i2c.push_back(1);
        g_hybrid_i2c.push_back(2);
        g_hybrid_i2c.push_back(3);
        g_hybrid_i2c.push_back(4);
        g_hybrid_i2c.push_back(5);
        g_hybrid_i2c.push_back(6);
        g_hybrid_i2c.push_back(7);
    }
    else {
        g_hybrid_i2c.push_back(3);
        g_hybrid_i2c.push_back(2);
        g_hybrid_i2c.push_back(1);
        g_hybrid_i2c.push_back(0);
        g_hybrid_i2c.push_back(7);
        g_hybrid_i2c.push_back(6);
        g_hybrid_i2c.push_back(5);
        g_hybrid_i2c.push_back(4);

    }
    m_registers.clear();
    setRegisterValues(m_registers);
}

// ------------------------------------------------------------------------ //
void FECConfigModule::LoadMessageHandler(MessageHandler& m)
{
    m_messageHandler = &m;
}

// ------------------------------------------------------------------------ //
FECConfigModule& FECConfigModule::LoadSocket(SocketHandler& socket)
{
    m_socketHandler = &socket;
    return *this;
}

// ------------------------------------------------------------------------ //
bool FECConfigModule::ConfigVMM(int hybrid_index, int vmm_index, bool enableConfigCheck)
{
    bool result = true;
    bool ok;

    ///////////////////////////////////////////////////
    // build the configuration word(s) to be sent
    // to the front end
    ///////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    // Global SPI_1
    /////////////////////////////////////////////////
    std::vector<QString> globalRegisters;
    globalRegisters.clear();
    FillGlobalRegisters(globalRegisters, hybrid_index,  vmm_index);
    if(globalRegisters.size()!=3){
        GetMessageHandler()("ERROR Global SPI does not have 3 words", "FEC_config_module::SendConfig", true);
        return -1;
    }
    ///////////////////////////////////////////////////
    // Channel Registers
    ///////////////////////////////////////////////////
    std::vector<QString> channelRegisters;
    channelRegisters.clear();
    FillChannelRegisters(channelRegisters, hybrid_index,  vmm_index);
    if(channelRegisters.size()!=64){
        GetMessageHandler()("ERROR Channel registers do not have 64 values", "FEC_config_module::SendConfig", true);
        return -1;
    }
    ///////////////////////////////////////////////////
    // Global SPI_2
    ///////////////////////////////////////////////////
    std::vector<QString> globalRegisters2;
    globalRegisters2.clear();
    FillGlobalRegisters2(globalRegisters2, hybrid_index,  vmm_index);
    if(globalRegisters2.size()!=3){
        GetMessageHandler()("ERROR Global SPI does not have 3 words", "FEC_config_module::SendConfig", true);
        return -1;
    }


    if(g_slow_control != 2) {
        int idx = hybrid_index*2+vmm_index;
        if(g_use_config_check) {
            //reset I2C address 65 register 0
            if(enableConfigCheck) {
                uint32_t value = (1 << hybrid_index);
                ESS_WriteSc("sc_i2c_reset_config_check", 0x00000000, ok);
                ESS_WriteSc("sc_i2c_reset_config_check", value, ok);
                ESS_WriteSc("sc_i2c_reset_config_check", 0x00000000, ok);
            }
        }

        unsigned int firstGlobalRegSPI_2  = 0;
        unsigned int lastGlobalRegSPI_2   = 2;
        unsigned int firstChRegSPI = 3;
        unsigned int lastChRegSPI  = 66;
        unsigned int firstGlobalRegSPI_1  = 67;
        unsigned int lastGlobalRegSPI_1   = 69;

        // global SPI / VMM3: global bank 0
        for(unsigned int i = firstGlobalRegSPI_2; i <= lastGlobalRegSPI_2; ++i) {
            ESS_WriteSc("vmm_global_bank2_sp"+QString::number(i-firstGlobalRegSPI_2)+"0"+QString::number(idx), globalRegisters2[i-firstGlobalRegSPI_2].toUInt(&ok,2), ok);
        }

        //channel SPI
        for(unsigned int i = firstChRegSPI; i <= lastChRegSPI; ++i) {
            ESS_WriteSc("vmm_ch"+QString::number(i-firstChRegSPI).rightJustified(2, '0') + "0"+QString::number(idx), channelRegisters[i-firstChRegSPI].toUInt(&ok,2), ok);
        } // i
        //[12,523]

        // global SPI / VMM3: global bank 1
        for(unsigned int i = firstGlobalRegSPI_1; i <= lastGlobalRegSPI_1; ++i) {
            ESS_WriteSc("vmm_global_bank1_sp"+QString::number(i-firstGlobalRegSPI_1)+"0"+QString::number(idx), globalRegisters[i-firstGlobalRegSPI_1].toUInt(&ok,2), ok);
        } // i

        unsigned int value = (1 << idx);
        ESS_WriteSc("sc_cfg_vmm", 0x00000000, ok);
        ESS_WriteSc("sc_cfg_vmm", value, ok);
        ESS_WriteSc("sc_cfg_vmm", 0x00000000, ok);

    }
    else {
        if(g_use_config_check) {
            //reset I2C address 65 register 0
            if(enableConfigCheck) {
                CommunicateWithHybridI2C(65, hybrid_index, 0, 0, 2);
            }
        }
        stringstream sx;
        // configuration of the VMMs
        int send_to_port = m_fec->GetRegVal("vmmasic_port");

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
        //std::cout << "Configure IP " << ip.toStdString() << std::endl;

        // update global command counter
        GetSocketHandler().UpdateCommandCounter();
        //debug
        if(IsDbgEnabled()) {
            sx << "sending command SPI at comamnd #: " << GetSocketHandler().GetCommandCounter();
            GetMessageHandler()(sx,"Configuration::SendConfig");sx.str("");
        }

        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind


        QString chMapString = "0000000000000000";
        chMapString.replace( 15 - (hybrid_index*2+1-vmm_index) , 1 , QString("1") );
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
            sx << "Send config to port: " << send_to_port << " on IP " << ip.toStdString();
            GetMessageHandler()(sx);sx.str("");
        }
        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::SendConfig")) {
            return -1;
        }
        //sleep(2);
        readOK = GetSocketHandler().WaitForReadyRead();
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies...", "FEC_config_module::SendConfig");
            GetSocketHandler().ProcessReply();
        }
        else {
            GetMessageHandler()("Timeout while waiting for replies from VMM", "FEC_config_module::SendConfig");
            GetSocketHandler().CloseAndDisconnect("Configuration::SendConfig");
            return -1;
        }
    }
    GetSocketHandler().CloseAndDisconnect("Configuration::SendConfig");
    if(g_use_config_check) {
        //poll I2C address 65 register 0
        if(enableConfigCheck) {
            result = CheckConfigurationOfVMMs(hybrid_index, vmm_index);
        }
    }
    return result;
}


// ------------------------------------------------------------------------ //
void FECConfigModule::FillGlobalRegisters(std::vector<QString>& global, int hybrid_index, int vmm_index)
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
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slvs" ) ) );
    sequence++;

    // skips ch 16-47
    // [5]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"s32" ) ) );
    sequence++;

    //auto-reset at end ramp
    // [6]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index, vmm_index,"stcr" ) ) );
    sequence++;

    // ART flag synchronization
    // [7]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"ssart" ) ) );
    sequence++;

    // fast recovery from high charge
    // [8]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"srec" ) ) );
    sequence++;

    // mild tail cancellation
    // [9]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"stlc" ) ) );
    sequence++;

    // bipolar shape
    // [10]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sbip" ) ) );
    sequence++;

    // timing ramp at threshold
    // [11]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"srat" ) ) );
    sequence++;

    // fast reset at 6-b completion
    // [12]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sfrst" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckbc
    // [13]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slvsbc" ) ) );
    sequence++;

    // slvs 100 Ohm termination on cktp
    // [14]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slvstp" ) ) );
    sequence++;

    // slvs 100 Ohm termination on cktk
    // [15]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slvstk" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckdt
    // [16]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slvsdt" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckart
    // [17]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slvsart" ) ) );
    sequence++;

    // slvs 100 Ohm termination on cktki
    // [18]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slvstki" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ckena
    // [19]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slvsena" ) ) );
    sequence++;

    // slvs 100 Ohm termination on ck6b
    // [20]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slvs6b" ) ) );
    sequence++;

    // mixed signal functions
    // [21]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetRegVal("sL0enaV") ) );
    sequence++;

    // congurable feedback currents (factor 10x)
    // [22]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slh") ) );
    sequence++;

    // congurable feedback currents (factor 100x)
    // [23]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slxh") ) );
    sequence++;

    // extreme charge handling compensation
    // [24]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"stgc") ) );
    sequence++;

    //[25,29] not used
    sequence+=5;

    // reset (1)
    // [30]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"reset1" ) ) );
    sequence++;

    // reset (2)
    // [31]
    spi0.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"reset2" ) ) );
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
    tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index,"sdt" ) ,
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
    tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index,"sdp10" ) ,
                             10,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //10bit ADC
    // [16,17]
    tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index, "sc10b" ) ,2,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8bit ADC
    // [18,19]
    tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index, "sc8b" ) ,2,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //6bit
    // [20,22]
    tmp = QString("%1").arg(  m_fec->GetVMM( hybrid_index,  vmm_index, "sc6b" )  ,3,2,QChar('0'));
    spi1.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8-bit enable
    // [23]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"s8b" ) ) );
    sequence++;

    //6-bit enable
    // [24]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"s6b" ) ) );
    sequence++;

    //ADC enable
    // [25]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"s10b" ) ) );
    sequence++;

    //dual clock serialized
    // [26]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sdcks" ) ) );
    sequence++;

    //dual clock ART
    // [27]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sdcka" ) ) );
    sequence++;

    //dual clock 6-bit
    // [28]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sdck6b" ) ) );
    sequence++;

    //analog tri-states
    // [29]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sdrv" ) ) );
    sequence++;

    //timing out 2
    // [30]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"stpp" ) ) );
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
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sp" ) ) );
    sequence++;

    //disable at peak, sdp
    //[1]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sdp" ) ) );
    sequence++;

    //analog monitor to pdo, sbmx
    //[2]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sbmx" ) ) );
    sequence++;

    //tdo buffer
    //[3]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sbft" ) ) );
    sequence++;

    //pdo buffer
    //[4]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sbfp" ) ) );
    sequence++;

    //mo buffer
    //[5]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sbfm" ) ) );
    sequence++;

    //leakage current
    //[6]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"slg" ) ) );
    sequence++;

    //channel to monitor
    //[7,12]
    tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index,"sm5_sm0" ) ,
                             6,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //multiplexer
    //[13]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"scmx" ) ) );
    sequence++;

    //ART enable
    //[14]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sfa" ) ) );
    sequence++;

    //ART mode
    //[15]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sfam" ) ) );
    sequence++;

    //peak_time
    // [16,17]
    tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index,"st" ) ,2,2,QChar('0'));
    spi2.replace(sequence, tmp.size(),tmp);
    sequence += tmp.size();

    //double leakage (doubles the leakage current)
    // [18]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sfm" ) ) );
    sequence++;

    //gain
    // [19,21]
    tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index,"sg" ) ,3,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //neighbor trigger
    // [22]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sng" ) ) );
    sequence++;

    //direct outputs settings
    // [23]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"stot" ) ) );
    sequence++;

    //direct timing
    // [24]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sttt" ) ) );
    sequence++;

    //sub-hysteresis
    // [25]
    spi2.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"ssh" ) ) );
    sequence++;

    //TAC slope adjustment
    // [26,27]
    tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index,"stc" ) ,
                             2,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //threshold DAC highest 4 bits
    // [28,31]
    tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index,"sdt" ) ,
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
void FECConfigModule::FillChannelRegisters(std::vector<QString>& registers, int hybrid_index, int vmm_index)
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
                    QString::number( m_fec->GetVMM( hybrid_index,  vmm_index, "sc", i ) ) );
        sequence++;
        if(do_check) std::cout << " SC : " << reg.toStdString() << std::endl;

        //SL [9]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hybrid_index,  vmm_index, "sl", i ) ) );
        sequence++;
        if(do_check) std::cout << " SL : " << reg.toStdString() << std::endl;

        //ST [10]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hybrid_index,  vmm_index, "st", i ) ) );
        sequence++;
        if(do_check) std::cout << " ST : " << reg.toStdString() << std::endl;

        //STH [11]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hybrid_index,  vmm_index, "sth", i ) ) );
        sequence++;
        if(do_check) std::cout << " STH : " << reg.toStdString() << std::endl;

        //SM [12]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hybrid_index,  vmm_index, "sm", i ) ) );
        sequence++;
        if(do_check) std::cout << " SM : " << reg.toStdString() << std::endl;

        //SMX [13]
        reg.replace(sequence,1,
                    QString::number( m_fec->GetVMM( hybrid_index,  vmm_index, "smx", i ) ) );
        sequence++;
        if(do_check) std::cout << " SMX : " << reg.toStdString() << std::endl;

        //trim [14,18]
        tmp = "0000";
        tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index, "sd", i ) ,
                                 5,2,QChar('0'));
        //According to ATLAS software, still needed
        std::reverse(tmp.begin(),tmp.end()); //bug in VMM2, needs to be reversed
        reg.replace(sequence, tmp.size(), tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " TRIM : " << reg.toStdString() << std::endl;



        //10 bit adc lsb [19,23]
        tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index, "sz10b", i ) ,
                                 5,2,QChar('0'));
        //According to ATLAS software, has to be reversed
        std::reverse(tmp.begin(),tmp.end());
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 10bit : " << reg.toStdString() << std::endl;

        //8 bit adc lsb [24,27]
        tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index, "sz08b", i ) ,
                                 4,2,QChar('0'));
        //According to ATLAS software, has to be reversed
        std::reverse(tmp.begin(),tmp.end());
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 8bit : " << reg.toStdString() << std::endl;

        //6 bit adc lsb [28,30]
        tmp = QString("%1").arg( m_fec->GetVMM( hybrid_index,  vmm_index, "sz06b", i ) ,
                                 3,2,QChar('0'));
        //According to ATLAS software, has to be reversed
        std::reverse(tmp.begin(),tmp.end());
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 6bit : " << reg.toStdString() << std::endl;

        //[31] not used

        if(m_dbg) {
            std::stringstream chan;
            chan.str("");
            GetMessageHandler()(chan, "Configuration::fillChannelRegisters");
        }

        registers.push_back(reg);

    } // i


}



void FECConfigModule::FillGlobalRegisters2(std::vector<QString>& global, int hybrid_index, int vmm_index)
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
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"nskipm_i" ) ) );
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
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sL0dckinv" ) ) );
    sequence++;

    //invert BCCLK
    // [2]
    spi1.replace(sequence,1,
                 QString::number( m_fec->GetVMM( hybrid_index,  vmm_index,"sL0ckinv" ) ) );
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


bool FECConfigModule::Connect()
{
    QString ip =  m_fec->GetIP();

    if(GetSocketHandler().IsFECSocketOK() && m_isPinged) {
        return true;
    }
    else {
#ifdef __linux__
        int status_code = QProcess::execute("ping", QStringList()<<"-c1"<<ip);
#elif __APPLE__
        int status_code = QProcess::execute("ping", QStringList()<<"-t1"<<ip);
#elif _WIN32
        QString command = "ping -n 1 " + ip;
        int status_code = QProcess::execute(command);
#endif
        //////////////////////////////////
        if(status_code == 0){
            m_isPinged = true;
            std::cout<<"Ping successful"<<std::endl;

            ///////////////////////////////////////////////////////
            // now that we know we are connected OK, add and bind
            // the sockets
            ///////////////////////////////////////////////////////
            GetSocketHandler().AddSocket(m_fec->GetRegVal("fec_port"), QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
            return true;
        }
        else {
            m_isPinged = false;
            std::cout<<"ERROR Unable to successfully ping the IP: "<<ip.toStdString()<<std::endl;
            return false;
        }
    }
}




void FECConfigModule::ConfigHybrid( int hybrid_index)
{
    if(IsDbgEnabled())GetMessageHandler()("Configuring the pulser...","FEC_config_module::ConfigHybrid");

    bool ok;
    if(g_slow_control != 2) {
        ESS_WriteSc("hyb_tp_skew0"+QString::number(hybrid_index), m_fec->m_hybrids[hybrid_index].GetReg("TP_skew"), ok);
        ESS_WriteSc("hyb_tp_width0"+QString::number(hybrid_index), m_fec->m_hybrids[hybrid_index].GetReg("TP_width"), ok);
        ESS_WriteSc("hyb_tp_polarity0"+QString::number(hybrid_index), m_fec->m_hybrids[hybrid_index].GetReg("TP_pol"), ok);

        unsigned int value = (1 << hybrid_index);
        ESS_WriteSc("sc_cfg_hyb", 0x00000000, ok);
        ESS_WriteSc("sc_cfg_hyb", value, ok);
        ESS_WriteSc("sc_cfg_hyb", 0x00000000, ok);
    }
    else {
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

        int tpskew = m_fec->m_hybrids[hybrid_index].GetReg("TP_skew");
        int tpwidth = m_fec->m_hybrids[hybrid_index].GetReg("TP_width");
        int tppolarity = m_fec->m_hybrids[hybrid_index].GetReg("TP_pol");
        int tpdisable = m_fec->m_hybrids[hybrid_index].GetReg("TP_disable");
        ////////////////////////////
        // header
        ////////////////////////////
        QString hybridMapString = "00000000";
        hybridMapString.replace( 7 - hybrid_index , 1 , QString("1") );
        quint8 hybridMap = (quint8)hybridMapString.toInt(&ok,2);
        out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint32) hybridMap //[4,7]
            << (quint32) cmd.toUInt(&ok,16); //[8,11]

        //RD51 VMM3a hybrid firmware (40 MHz)
        if(g_clock_source == 3) {
            //0=22.5 MHz, 1=45 MHz, 2=90 MHz, 3=180 MHz
            int ckdt = 3;
            //0=80 MHz, 1= 80 MHz inv, 2 = 40 MHz, 3 = 20 MHz, 4 = 10 MHz, 5 = 5 MHz, 6 = 2.5 MHz
            int ckbc = 2;
            //0=0 ns, 1 = -12.5 ns, 2=-25 ns, 3= -37.5 ns
            int ckbc_skew = 0;
            ////////////////////////////
            // command
            ////////////////////////////
            out << (quint32) 0 //[12,15]
                << (quint32) 2 //[16,19]
                << (quint32) (tpskew + (tpwidth*16) + (tppolarity*128))
                << (quint32) 7 //[24,27]
                << (quint32)  ckbc + (ckbc_skew*16) //[32,35]
                << (quint32) 5 //[36,39], CKDT control register
                << (quint32) 0 + (ckdt*2); //[40,43] lowest bit: dynamic reconfiguration of clock, bits [1,2,3] CKDT selection (readout clock)
        }
        else {
            ////////////////////////////
            // command
            ////////////////////////////
            out << (quint32) 0 //[12,15]
                << (quint32) 2 //[16,19]
                << (quint32) tpskew //[20,23]
                << (quint32) 3
                << (quint32) tpwidth
                << (quint32) 4
                << (quint32) tppolarity
                << (quint32) 5
                << (quint32) tpdisable;
        }

        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ConfigHybrid")) {
            return;
        }
        bool readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies from IP " + ip.toStdString() + "...","FEC_config_module::configTP");
            GetSocketHandler().ProcessReply();
        } else {
            GetMessageHandler()("Timeout while waiting for replies from VMM",
                                "FEC_config_module::configTP", true);
            GetSocketHandler().CloseAndDisconnect("FEC_config_module::configTP");
            return;
        }
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::configTP");

}



// ------------------------------------------------------------------------ //
void FECConfigModule::ConfigFEC()
{
    if(IsDbgEnabled())GetMessageHandler()("Sending trigger ACQ constants...","FEC_config_module::setTriggerAcqConstants");

    bool ok;
    if(g_slow_control != 2) {
        ESS_WriteSc("app_debug_data_format", m_fec->GetRegVal("debug_data_format"), ok);
        ESS_WriteSc("app_latency_reset", m_fec->GetRegVal("latency_reset") , ok);
        ESS_WriteSc("app_latency_data_max", m_fec->GetRegVal("latency_data_max"), ok);
        ESS_WriteSc("app_latency_data_jitter", m_fec->GetRegVal("latency_data_error") , ok);
        ESS_WriteSc("app_tp_offset_first", m_fec->GetRegVal("tp_offset_first") , ok);
        ESS_WriteSc("app_tp_offset", m_fec->GetRegVal("tp_offset"), ok);
        ESS_WriteSc("app_tp_latency", m_fec->GetRegVal("tp_latency"), ok);
        ESS_WriteSc("app_tp_number", m_fec->GetRegVal("tp_number"), ok);
        ESS_WriteSc("app_chmask", m_fec->GetChMap(), ok);
        ESS_WriteSc("sc_cfg_app", 0x00000000, ok);
        ESS_WriteSc("sc_cfg_app", 0x00000001, ok);
        ESS_WriteSc("sc_cfg_app", 0x00000000, ok);
    }
    else {
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

        QString cmd, cmdType, cmdLength, msbCounter;
        cmd         = "AA";
        cmdType     = "AA";
        cmdLength   = "FFFF";
        msbCounter  = "0x80000000";
        out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5]
            << (quint16) m_fec->GetChMap() //[6,7]
            << (quint8)  cmd.toUInt(&ok,16) //[8]
            << (quint8)  cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok, 16); //[10,11]

        uint32_t trgout_time =  m_fec->GetRegVal("trgout_time");
        uint32_t trgout_sel = 0;
        // index = 0: input, index 1-4096: generate trigger
        if(trgout_time > 0) {
           trgout_time =  trgout_time - 1;
        }

        uint32_t trgin_on = 0;
        uint32_t trgin_invert = m_fec->GetRegVal("trgin_invert");
        if(trgin_invert > 0) {
            // trigger input on, register timestamp
            trgin_on = 1;
            // polarity setting, 0 positive, 1 negative
            trgin_invert = trgin_invert - 1;
        }

        uint32_t trgout_on = 0;
        uint32_t trgout_invert = m_fec->GetRegVal("trgout_invert");
        if(trgout_invert > 0) {
            // polarity setting, 0 positive, 1 negative
            trgout_invert = trgout_invert - 1;
            //create a trigger output
            trgout_on = 1;
        }
        if(trgout_on == 1) {
             trgout_sel = 1;
        }
        uint32_t trg_on_off =  trgin_on + 2*trgout_on;
        uint32_t trg_invert =  trgin_invert +2*trgout_invert +4*trgout_sel;



        ///////////////////////////
        // trigger constants
        ///////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 1 //[16,19]
            << (quint32) m_fec->GetRegVal("debug_data_format") //[20,23]
            << (quint32) 2 //[16,19]
            << (quint32) m_fec->GetRegVal("latency_reset") //[20,23]
            << (quint32) 3 //[16,19]
            << (quint32) m_fec->GetRegVal("latency_data_max") //[20,23]
            << (quint32) 4 //[16,19]
            << (quint32) m_fec->GetRegVal("latency_data_error") //[20,23]
            << (quint32) 5 //[16,19]
            << (quint32) trg_on_off //[20,23]
            << (quint32) 7 //[16,19]
            << (quint32) 0x00000000 //[20,23]
            << (quint32) 8 //[16,19]
            << (quint32) m_fec->GetChMap()
            << (quint32) 9 //[16,19]
            << (quint32) m_fec->GetRegVal("tp_offset_first") //[20,23]
            << (quint32) 10 //[16,19]
            << (quint32) m_fec->GetRegVal("tp_offset") //[20,23]
            << (quint32) 11 //[16,19]
            << (quint32) m_fec->GetRegVal("tp_latency") //[20,23]
            << (quint32) 12 //[16,19]
            << (quint32) m_fec->GetRegVal("tp_number") //[20,23] //[20,23]
            << (quint32) 13 //[16,19]
            << (quint32) trg_invert //[20,23] //[20,23]
            << (quint32) 14 //[16,19]
            << (quint32) trgout_time; //[20,23] //[20,23]

        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::SetFECConfig")) {
            return;
        }
        bool readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies from IP " + ip.toStdString() + "...","FEC_config_module::SetFECConfig");
            GetSocketHandler().ProcessReply();
        }
        else {
            GetMessageHandler()("Timeout while waiting for replies from VMM",
                                "FEC_config_module::setTriggerAcqConstants", true);
            GetSocketHandler().CloseAndDisconnect("FEC_config_module::SetFECConfig");
            return;
        }
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::SetFECConfig");
}

void FECConfigModule::PowerCycleHybrids()
{
    if(IsDbgEnabled())GetMessageHandler()("Setting DVM settings...","FEC_config_module::PowerCycleHybrids");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = m_fec->GetRegVal("dvm_i2c_port");

    QString ip = m_fec->GetIP();
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    // headers
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";


    /*PCA9534
    P0: De-emphasis Fast-Or (ART Trigger input ) from RJ45 (Powerbox only), default = off
    P1: Pre-emphasis 40MHz clock and Configuration download to all HDMI ports, default =off
    P2: De-emphasis of incoming VMM data links D0 and D1
    P3: 1 = RESET CRATE !!! If cable to ATX adapter is set and Crate in Remote mode. Set to 0 to allow crate reboot (ecperimental)
    P4: 1 = (Disable P1 and P2 power for VMMs on top 4 HDMI ports, experimental)
    P5: 1 = (Disable P1 and P2 power for VMMs on bottom 4 HDMI ports , experimental)
    P6: 1 = P1 Power enabled when P2a is disabled ( tests with Spartan and I2C only)
    P7: Power to VMM via HDMI default power = 0, 1= Off, 0/1/0 = Reset VMMs ( 0.5 sec waitloop between 1-> 0 )

    Device address : 0100
        Hardware address (a2, a1, a0) : 001
        Total address : 0100001 0x21


        Write config register
        0x42 (adress+write)
            0x03 command byte (write to config register)
                0x7F (0111 1111), set P7 to output (1=input, 0 =output, default is 1)

        Write output port
        0x42 (adress+write)
            0x01 command byte (write to output port)
                0x80 (0000 0000), set P7 to high

            Write output port
        0x42 (adress+write)
            0x01 command byte (write to output port)
                0x00 (0000 0000), set P7 to low

            */

    quint8 deviceAdress = 0x21;
    quint8 i2cAddress = 0x42; //0x21 << 1 | 0
    ///////////////////////////
    // First command: set activated pins as output
    ///////////////////////////
    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32) 0x80000000 //[0,3]
        << (quint16) 0x0000 //[4,5]
        << (quint8) 0x00 //[6]
        << (quint8) i2cAddress //[7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]


    ///////////////////////////
    // word
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0x00 //slow control address, not used
        << (quint32) 0x037F; //write to config register (0x03), second byte set P7 to output (1=input, 0 =output, default is 1)


    if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::PowerCycleHybrids")) {
        return;
    }
    bool readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead();
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::PowerCycleHybrids");
        GetSocketHandler().ProcessReply();
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::PowerCycleHybrids",true);
        GetSocketHandler().CloseAndDisconnect("FEC_config_module::PowerCycleHybrids");
        return;
    }

    GetSocketHandler().CloseAndDisconnect("FEC_config_module::PowerCycleHybrids");
    //QThread::sleep(2);

    datagram.clear();
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // Second command: write for activated pins high to output port
    ///////////////////////////
    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32) 0x80000000 //[0,3]
        << (quint16) 0x0000 //[4,5]
        << (quint8) 0x00 //[6]
        << (quint8) i2cAddress //[7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]


    ///////////////////////////
    // word
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0x00 //slow control address, not used
        << (quint32) 0x0180; //first byte command byte (write to output port), second byte set P7 to high


    if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::PowerCycleHybrids")) {
        return;
    }
    readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead();
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::PowerCycleHybrids");
        GetSocketHandler().ProcessReply();
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::PowerCycleHybrids",true);
        GetSocketHandler().CloseAndDisconnect("FEC_config_module::PowerCycleHybrids");
        return;
    }

    GetSocketHandler().CloseAndDisconnect("FEC_config_module::PowerCycleHybrids");

    QThread::sleep(4);

    datagram.clear();
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    ///////////////////////////
    // Last command: write for activated pins high to output port
    ///////////////////////////
    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32) 0x80000000 //[0,3]
        << (quint16) 0x0000 //[4,5]
        << (quint8) 0x00 //[6]
        << (quint8) i2cAddress //[7]
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]


    ///////////////////////////
    // word
    ///////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) 0x00 //slow control address, not used
        << (quint32) 0x0100; //first byte write to output port, second byte set P7 to low

    if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::PowerCycleHybrids")) {
        return;
    }
    readOK = true;
    readOK = GetSocketHandler().WaitForReadyRead();
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::PowerCycleHybrids");
        GetSocketHandler().ProcessReply();
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::PowerCycleHybrids",true);
        GetSocketHandler().CloseAndDisconnect("FEC_config_module::PowerCycleHybrids");
        return;
    }

    GetSocketHandler().CloseAndDisconnect("FEC_config_module::PowerCycleHybrids");
}

// ------------------------------------------------------------------------ //
bool FECConfigModule::CheckLinkStatus(QString & message, QVector<QString> & linkstatus)
{
    linkstatus.clear();
    for(int n=0; n<HYBRIDS_PER_FEC;n++) {
        linkstatus.push_back("0");
    }
    if(IsDbgEnabled())GetMessageHandler()("Checking link status...","FEC_config_module::checkLinkStatus");

    bool ok;
    bool status_checked =false;
    if(g_slow_control != 2) {
        QString result = ESS_ReadSc("sc_app_link_status", 0, "app_link_status", ok);
        if(!ok) {
            return false;
        }
        stringstream ss;
        QString bin, hex;

        quint32 tmp32 = result.toUInt(&ok,16);
        QString str = bin.number(tmp32,16);
        for(int n=0; n<str.size(); n++) {
            linkstatus[n] = str.mid(str.size()-1-n,1);
            ss << "hybr. " + QString::number(n).toStdString() + ": " + linkstatus[n].toStdString() << std::endl;
        }
        message = QString::fromStdString(ss.str());
        status_checked = true;
    }
    else {
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

        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::checkLinkStatus")) {
            return false;
        }
        bool readOK = GetSocketHandler().WaitForReadyRead();

        QByteArray read_datagram;
        read_datagram.clear();
        QHostAddress address;
        quint16 port;

        while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
            read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
            GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
            if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {
                stringstream ss;
                QString bin, hex;
                hex = read_datagram.mid(0, 4).toHex();
                quint32 tmp32 = hex.toUInt(&ok,16);
                ss << " *** STATUS-ID " <<  bin.number(tmp32,10).toStdString() << " ***" << endl;

                hex = read_datagram.mid(20, 4).toHex();
                tmp32 = hex.toUInt(&ok,16);
                QString str = bin.number(tmp32,16);
                for(int n=0; n<str.size(); n++) {
                    linkstatus[n] = str.mid(str.size()-1-n,1);
                    ss << "hybr. " + QString::number(n).toStdString() + ": " + linkstatus[n].toStdString() << std::endl;
                }
                message = QString::fromStdString(ss.str());

                status_checked = true;
                break;
            }
        }
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::checkLinkStatus");
            GetSocketHandler().ProcessReply();
        } else {
            GetMessageHandler()("Timeout while waiting for replies from VMM",
                                "FEC_config_module::checkLinkStatus", true);
        }

    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::checkLinkStatus");
    return status_checked;
}

// ------------------------------------------------------------------------ //
void FECConfigModule::ResetFEC()
{
    GetMessageHandler()("Resetting FEC...","FEC_config_module::resetFEC");
    bool ok;
    if(g_slow_control != 2) {
        ESS_WriteSc("sc_app_reset_assister", 0x00000000, ok);
        ESS_WriteSc("sc_app_reset_assister", 0x00000001, ok);
        ESS_WriteSc("sc_app_reset_assister", 0x00000000, ok);
    }
    else {
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
        QString value = "FFFF0001";
        if(IsDbgEnabled())GetMessageHandler()("WarmInit FEC...","FEC_config_module::resetFEC");


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

        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::resetFEC")) {
            return;
        }

        bool readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::resetFEC");
            GetSocketHandler().ProcessReply();
        } else {
            if(IsDbgEnabled())GetMessageHandler()("Timeout while waiting for replies from VMM",
                                                  "FEC_config_module::resetFEC",true);
        }
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::resetFEC");
    return;
}

unsigned int FECConfigModule::ESS_GetAddress(QString reg) {
    //For slow control via the assister, the ring and fen does not matter,
    //since each assister has it's own IP and the board 1GB ethernet is used
    int fenID = 0;
    int ringID = 0;
    if(g_slow_control==0) {
        ringID = m_fec->GetRegVal("ring");
        fenID = m_fec->GetRegVal("fen");
    }
    uint32_t addr = m_registers[reg.toStdString()];
    return (ringID*0x10000000)+(fenID*0x00800000)+addr;
}

bool FECConfigModule::ESS_ResetSc() {
    bool ok;
    ESS_WriteSc("sc_cfg_app", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_cfg_hyb", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_cfg_vmm", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_app_link_status", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_app_reset_assister", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_app_firmware_version", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_app_mac_address_high", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_app_mac_address_low", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_app_assister_ip", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_app_daq_ip", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_i2c_geo_id", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_i2c_firmware_version", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_i2c_hybrid_id", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_i2c_config_check", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_i2c_reset_config_check", 0x00000000, ok);
    if(!ok) {
        return ok;
    }
    ESS_WriteSc("sc_i2c_vmm_adc", 0x00000000, ok);
    return ok;
}

QString FECConfigModule::ESS_ReadSc(QString command_reg, int index, QString read_reg, bool &readOk) {
    QString ret = "";
    QString retStatus = "";
    QString retData = "";
    QString retAddress = "";
    QString retCommand = "";
    QByteArray datagram;
    QByteArray read_datagram;
    QHostAddress address;
    quint16 port;


    QString ip = m_fec->GetIP();
    //std::cout << "IP " << ip.toStdString() << std::endl;
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    int send_to_port = m_fec->GetRegVal("ess_sc_port");

    unsigned int command = 0xE55C0003;
    unsigned int reg = ESS_GetAddress(command_reg);
    unsigned int data = 0x00000000;
    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32)command //[0,3]
        << (quint32) reg //[4,5]
        << (quint32) data; //[10,11]

    if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ESS_ReadSc")) {
        readOk = false;
        return ret;
    }

    readOk = GetSocketHandler().WaitForReadyRead();
    read_datagram.clear();

    while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
        read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
        GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
        if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {
            retCommand = read_datagram.mid(0,4).toHex();
            retAddress = read_datagram.mid(4,4).toHex();
            retData = read_datagram.mid(8,4).toHex();
            retStatus = read_datagram.mid(12,4).toHex();
            if(retStatus!= "00000000") {
                std::cout << "\nError ESS_ReadSc Command: " << retCommand.toStdString() << ",  address:" << retAddress.toStdString() << ",  data:" << retData.toStdString() << ",  status:" << retStatus.toStdString() << std::endl;
            }
            readOk = true;
            break;
        }
    } // while loop


    datagram.clear();
    out.device()->seek(0); //rewind

    command = 0xE55C0003;
    reg = ESS_GetAddress(command_reg);
    data = (1 << index);
    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32)command //[0,3]
        << (quint32) reg //[4,5]
        << (quint32) data; //[10,11]


    if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ESS_ReadSc")) {
        readOk = false;
        return ret;
    }
    readOk = GetSocketHandler().WaitForReadyRead();
    read_datagram.clear();

    while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
        read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
        GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
        if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {
            retCommand = read_datagram.mid(0,4).toHex();
            retAddress = read_datagram.mid(4,4).toHex();
            retData = read_datagram.mid(8,4).toHex();
            retStatus = read_datagram.mid(12,4).toHex();
            if(retStatus!= "00000000") {
                std::cout << "\nError ESS_ReadSc Command: " << retCommand.toStdString() << ",  address:" << retAddress.toStdString() << ",  data:" << retData.toStdString() << ",  status:" << retStatus.toStdString() << std::endl;
            }
            readOk = true;
            break;
        }
    } // while loop

    datagram.clear();
    out.device()->seek(0); //rewind

    //Do not read registers back too soon, otherwise old ADC value is read
    QThread::usleep(7000);

    command = 0xE55C0001;
    reg = ESS_GetAddress(read_reg);
    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32)command //[0,3]
        << (quint32) reg; //[4,5]
    if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ESS_ReadSc")) {
        readOk = false;
        return ret;
    }
    readOk = GetSocketHandler().WaitForReadyRead();
    read_datagram.clear();

    while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
        read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
        GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
        if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {
            retCommand = read_datagram.mid(0,4).toHex();
            retAddress = read_datagram.mid(4,4).toHex();
            retData = read_datagram.mid(8,4).toHex();
            retStatus = read_datagram.mid(12,4).toHex();
            if(retStatus!= "00000000") {
                std::cout << "\nError ESS_ReadSc Command: " << retCommand.toStdString() << ",  address:" << retAddress.toStdString() << ",  data:" << retData.toStdString() << ",  status:" << retStatus.toStdString() << std::endl;
            }
            readOk = true;
            ret = retData;
            break;
        }
    } // while loop



    datagram.clear();
    out.device()->seek(0); //rewind

    command = 0xE55C0003;
    reg =  ESS_GetAddress(command_reg);;
    data = 0x00000000;
    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32)command //[0,3]
        << (quint32) reg //[4,5]
        << (quint32) data; //[10,11]

    if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ESS_ReadSc")) {
        readOk = false;
        return ret;
    }

    readOk = GetSocketHandler().WaitForReadyRead();
    read_datagram.clear();

    while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
        read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
        GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
        if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {
            retCommand = read_datagram.mid(0,4).toHex();
            retAddress = read_datagram.mid(4,4).toHex();
            retData = read_datagram.mid(8,4).toHex();
            retStatus = read_datagram.mid(12,4).toHex();
            if(retStatus!= "00000000") {
                std::cout << "\nError ESS_ReadSc Command: " << retCommand.toStdString() << ",  address:" << retAddress.toStdString() << ",  data:" << retData.toStdString() << ",  status:" << retStatus.toStdString() << std::endl;
            }
            readOk = true;
            break;
        }
    } // while loop
    return ret;
}

void FECConfigModule::ESS_WriteSc(QString the_reg, unsigned int val, bool &readOk) {
    QString ret = "";
    QString retStatus = "";
    QString retData = "";
    QString retAddress = "";
    QString retCommand = "";
    QByteArray datagram;
    QByteArray read_datagram;
    QHostAddress address;
    quint16 port;

    QString ip = m_fec->GetIP();
    //std::cout << "IP " << ip.toStdString() << std::endl;
    datagram.clear();
    QDataStream out (&datagram, QIODevice::WriteOnly);
    out.device()->seek(0); //rewind

    GetSocketHandler().UpdateCommandCounter();

    int send_to_port = m_fec->GetRegVal("ess_sc_port");

    unsigned int command = 0xE55C0003;
    unsigned int reg = ESS_GetAddress(the_reg);
    unsigned int data = val;

    ///////////////////////////
    // header info
    ///////////////////////////
    out << (quint32)command //[0,3]
        << (quint32) reg //[4,5]
        << (quint32) data; //[10,11]

    if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ESS_WriteSc")) {
        readOk = false;
        return;
    }

    readOk = GetSocketHandler().WaitForReadyRead();
    read_datagram.clear();

    while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
        read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
        GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
        if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {
            retCommand = read_datagram.mid(0,4).toHex();
            retAddress = read_datagram.mid(4,4).toHex();
            retData = read_datagram.mid(8,4).toHex();
            retStatus = read_datagram.mid(12,4).toHex();
            if(retStatus!= "00000000") {
                std::cout << "\nError ESS_WriteSc - Command: " << retCommand.toStdString() << ",  address:" << retAddress.toStdString() << ",  data:" << retData.toStdString() << ",  status:" << retStatus.toStdString() << std::endl;
            }
            readOk = true;
            break;
        }
    } // while loop

}

// ------------------------------------------------------------------------ //
bool FECConfigModule::ReadSystemRegisters(QMap<QString, QString>& registers) {
    if(IsDbgEnabled())
        GetMessageHandler()("Reading FEC system registers...","FEC_config_module::ReadSystemRegisters");

    bool ok = true;
    bool read_registers = false;
    if(g_slow_control != 2) {
        if(!ESS_ResetSc()) {
            return false;
        }
        QString result = ESS_ReadSc("sc_app_board_id", 0, "app_board_id", ok);
        if(!ok) {
            return false;
        }
        registers["board_id"] = result;
        read_registers = true;

    }
    else {
        QByteArray datagram;

        QString ip = m_fec->GetIP();
        //std::cout << "IP " << ip.toStdString() << std::endl;
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        GetSocketHandler().UpdateCommandCounter();

        // send reset call to FEC port
        int send_to_port = m_fec->GetRegVal("fec_port");
        // headers
        QString cmd, cmdType, cmdLength, msbCounter;
        cmd = "BB"; //read
        cmdType = "AA"; // list
        cmdLength = "FFFF";
        msbCounter = "0x80000000";

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
        out << (quint32) 0
            << (quint32) 0
            << (quint32) 1 // FEC MAC part 1: Vendor identifier
            << (quint32) 2 // FEC MAC part 2: Device identifier
            << (quint32) 3 // FEC ip
            << (quint32) 10;// DAQ IP


        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ReadSystemRegisters")) {
            return false;
        }

        bool readOK = GetSocketHandler().WaitForReadyRead();

        QByteArray read_datagram;
        read_datagram.clear();
        QHostAddress address;
        quint16 port;

        while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
            read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
            GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
            if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {

                QString FirmwareVers = read_datagram.mid(20,4).toHex();
                QString MACvendor = read_datagram.mid(29,3).toHex();
                QString MACdevice = read_datagram.mid(37,3).toHex();
                QString FECip = read_datagram.mid(44,4).toHex();
                QHostAddress theFECIP;
                bool ok;
                long long lFECIP = FECip.toLongLong(&ok,16);
                theFECIP.setAddress(lFECIP);

                QString strFECip = theFECIP.toString();

                QString DAQip = read_datagram.mid(52,4).toHex();
                QHostAddress theDACIP;

                long long  lDACIP = DAQip.toLongLong(&ok,16);
                theDACIP.setAddress(lDACIP);
                QString strDACip = theDACIP.toString();

                registers["FirmwareVers"] = FirmwareVers;
                registers["MACvendor"] = MACvendor;
                registers["MACdevice"] = MACdevice;
                registers["FECip"] = strFECip;
                registers["DAQip"] = strDACip;


                stringstream sx;
                sx.str("");
                sx << "*****************************************************\n"
                   << "  > Firmware version: " << FirmwareVers.toStdString() << "\n"
                   << "  > MAC Vendor part: " << MACvendor.toStdString() << "\n"
                   << "  > MAC device part: " << MACdevice.toStdString() << "\n"
                   << "  > FEC IP : " << FECip.toStdString() << " (" << strFECip.toStdString() << ")" <<  "\n"
                   << "  > DAQ PC IP : " << DAQip.toStdString() << " (" << strDACip.toStdString() << ")" <<  "\n"
                   << "*****************************************************";
                cout << sx.str() << endl;
                read_registers = true;
                break;
            }
        } // while loop
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadSystemRegisters");
            GetSocketHandler().ProcessReply();
        } else {
            if(IsDbgEnabled())GetMessageHandler()("Timeout while waiting for replies from VMM",
                                                  "FEC_config_module::ReadSystemRegisters",true);
        }
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::ReadSystemRegisters");

    return read_registers;

}


// ------------------------------------------------------------------------ //
void FECConfigModule::writeBoardId(int boardId) {
    GetMessageHandler()("Write new board id...","FEC_config_module::writeBoardId");
    std::cout << boardId << std::endl;
    bool ok;
    if(g_slow_control == 0) {
        ESS_WriteSc("eeprom_board_id", boardId, ok);
        ESS_WriteSc("sc_eeprom_board_id", 0x00000000, ok);
        ESS_WriteSc("sc_eeprom_board_id", 0x00000001, ok);
        ESS_WriteSc("sc_eeprom_board_id", 0x00000000, ok);
    }
}


// ------------------------------------------------------------------------ //
void FECConfigModule::writeFECip(int FECip)
{
    GetMessageHandler()("Write new FEC ip...","FEC_config_module::writeFECip");

    bool ok;
    if(g_slow_control == 1) {
        ESS_WriteSc("eeprom_assister_ip", FECip, ok);
        ESS_WriteSc("sc_eeprom_assister_ip", 0x00000000, ok);
        ESS_WriteSc("sc_eeprom_assister_ip", 0x00000001, ok);
        ESS_WriteSc("sc_eeprom_assister_ip", 0x00000000, ok);
    }
    else if(g_slow_control == 2) {
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
            << (quint32) 0xa8f30000 // FEC ip register on FEC EEPROM (via I2C)
            << (quint32) FECip; // value 167772162=10.0.0.2

        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::writeFECip")) {
            return;
        }

        bool readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();
        if(readOK) {
            if(IsDbgEnabled()) GetMessageHandler()("Processing replies...","FEC_config_module::writeFECip");
            GetSocketHandler().ProcessReply();
        } else {
            if(IsDbgEnabled()) GetMessageHandler()("Timeout while waiting for replies from VMM",
                                                   "FEC_config_module::writeFECip",true);
        }
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::writeFECip");
}
// ------------------------------------------------------------------------ //


// ------------------------------------------------------------------------ //
void FECConfigModule::writeDAQip(int DAQip)
{
    GetMessageHandler()("Write new DAQ ip...","FEC_config_module::writeDAQip");

    bool ok;
    if(g_slow_control != 2) {
        ESS_WriteSc("eeprom_daq_ip", DAQip, ok);
        ESS_WriteSc("sc_eeprom_daq_ip", 0x00000000, ok);
        ESS_WriteSc("sc_eeprom_daq_ip", 0x00000001, ok);
        ESS_WriteSc("sc_eeprom_daq_ip", 0x00000000, ok);
    }
    else {
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
        out << (quint32) 0x0 //[12,15]
            << (quint32) 0xa8f30012// DAQ ip register on FEC EEPROM (via I2C)
            << (quint32) DAQip; // value 167772162=10.0.0.2

        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::writeDAQip")) {
            return;
        }

        bool readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();
        if(readOK) {
            if(IsDbgEnabled()) GetMessageHandler()("Processing replies...","FEC_config_module::writeDAQip");
            GetSocketHandler().ProcessReply();
        } else {
            if(IsDbgEnabled()) GetMessageHandler()("Timeout while waiting for replies from VMM",
                                                   "FEC_config_module::writeDAQip",true);
        }
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::writeDAQip");
}

// ------------------------------------------------------------------------ //
void FECConfigModule::ACQon(bool broadcast)
{
    if(IsDbgEnabled())GetMessageHandler()("Setting ACQ ON","FEC_config_module::ACQon");

    bool ok;
    if(g_slow_control != 2) {
        ESS_WriteSc("app_acq_enable", 0x00000001, ok);
        ESS_WriteSc("sc_acq_on_off", 0x00000000, ok);
        ESS_WriteSc("sc_acq_on_off", 0x00000001, ok);
        ESS_WriteSc("sc_acq_on_off", 0x00000000, ok);
    }
    else {
        QByteArray datagram;

        // send trigger mode to VMMAPP port
        int send_to_port = m_fec->GetRegVal("vmmapp_port");

        QString ip = m_fec->GetIP();
        QStringList list = ip.split(".");
        if(broadcast){
            ip = list[0] + "." + list[1] + "." + list[2] + ".255";
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


        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ACQon")) {
            return;
        }

        bool readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies...", "FEC_config_module::ACQon");
            GetSocketHandler().ProcessReply();
        } // readOK
        else {
            GetMessageHandler()("Timeout while waiting for replies from VMM","FEC_config_module::ACQon",true);
        }
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::ACQon");
}
// ------------------------------------------------------------------------ //
void FECConfigModule::ACQoff(bool broadcast)
{
    if(IsDbgEnabled())GetMessageHandler()("Setting ACQ OFF","FEC_config_module::ACQoff");

    bool ok;
    if(g_slow_control != 2) {
        ESS_WriteSc("app_acq_enable", 0x00000000, ok);
        ESS_WriteSc("sc_acq_on_off", 0x00000000, ok);
        ESS_WriteSc("sc_acq_on_off", 0x00000001, ok);
        ESS_WriteSc("sc_acq_on_off", 0x00000000, ok);
    }
    else {
        QByteArray datagram;

        // send trigger mode to VMMAPP port
        int send_to_port = m_fec->GetRegVal("vmmapp_port");

        QString ip = m_fec->GetIP();
        QStringList list = ip.split(".");
        if(broadcast){
            ip = list[0] + "." + list[1] + "." + list[2] + ".255";
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

        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ACQoff [1]")) {
            return;
        }
        bool readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ACQoff");
            GetSocketHandler().ProcessReply();
        }
        else {
            GetMessageHandler()("Timeout [1] while waiting for replies from VMM",
                                "FEC_config_module::ACQoff", true);
        }
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::ACQoff");
}


// ------------------------------------------------------------------------ //
int FECConfigModule::ReadADC(int hybrid_index, int vmm_index, int adc_chan)
{
    if(IsDbgEnabled())GetMessageHandler()("Reading ADC on hybrid...","FEC_config_module::ReadADC");

    int ADCresult_int = 0;
    bool ok;
    if(g_slow_control != 2) {
        int vmm = hybrid_index*2+vmm_index;
        QString result = ESS_ReadSc("sc_i2c_vmm_adc", vmm, "vmm_i2c_adc0" + QString::number(vmm), ok);
        if(!ok) {
            return false;
        }
        ADCresult_int = (result.toInt(&ok,16) & 0xFFFF) >> 4; //bit shift
    }
    else {
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

        QString hybridMapString = "00000000";
        hybridMapString.replace(7 -  g_hybrid_i2c[hybrid_index] , 1 , QString("1") ); // remaping of HDMI ports on DVM card: not performed for I2C MUX -> this hack needed
        quint8 hybridMap = (quint8)hybridMapString.toInt(&ok,2);

        ///////////////////////////
        // header info
        ///////////////////////////
        out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5]
            << (quint8) hybridMap//146 //[6] Subaddress: enable MUX channels
            << (quint8) ((i2c_addr << 1) | 0) //i2c_addr*2 + 0 //[7] ADC I2C address, last bit: read/not write (1 = read); ADC Addresses: 1001000/1001001 + read/write
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

        ///////////////////////////
        // word
        ///////////////////////////
        // calculate channel to read
        // ch 0: tdo        pattern 100 = 4
        // ch 1: pdo        pattern 101 = 5
        // ch 2: Mo         pattern 110 = 6
        // ch 3: not used   pattern 111 = 7
        // => adc_chan + 4

        out << (quint32) 0 //[12,15]
            << (quint32) 0 //[16,19] // goes to sc_address, do not use. can be used e.g. to redefine I2C addresses of in firmware
            << (quint32) (1*65536) +                      (1*32768 + (adc_chan+4)*4096 + 2*512 + 1*256) +                 131;
        //=116099; //[20,23] //goes to sc_data to ADC, must be 3 bytes to write to configuration register, 0 for setting address pointer to conversion register and anything but 0 to read
        // is 00000001 (point to conversion register) 11000101 (start conversion, channel 1, range +-2V, single shot) 10000011 (LSB of conversion register to be written)

        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ReadADC")) {
            return 0;
        }

        bool readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();

        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadADC");
            GetSocketHandler().ProcessReply();
        } else {
            GetMessageHandler()("Timeout while waiting for replies from VMM",
                                "FEC_config_module::ReadADC",true);
            GetSocketHandler().CloseAndDisconnect("FEC_config_module::ReadADC");
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
            << (quint8) hybridMap//146 //[6] Subaddress: enable MUX channels
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


        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ReadADC")) {
            return 0;
        }

        readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();
        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadADC");
            GetSocketHandler().ProcessReply();
        } else {
            GetMessageHandler()("Timeout while waiting for replies from VMM",
                                "FEC_config_module::ReadADC", true);
            GetSocketHandler().CloseAndDisconnect("FEC_config_module::ReadADC");
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
            << (quint8) hybridMap//146 //[6] Subaddress: enable MUX channels
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

        if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ReadADC")) {
            return 0;
        }
        readOK = true;
        readOK = GetSocketHandler().WaitForReadyRead();


        QByteArray read_datagram;
        read_datagram.clear();
        QHostAddress address;
        quint16 port;

        while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
            read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
            GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
            if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {

                QString ADCresult = read_datagram.mid(22,2).toHex();
                //std::cout << ADCresult.toStdString() << std::endl;
                //calculate level in mV from bare ADC result: 0-2047: pos (0 mV - 2047 mV), 2048-4094:neg(not possible)
                ADCresult_int = ADCresult.toInt(&ok,16) >> 4; //bit shift
                break;
            }
        }

        if(readOK) {
            if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadADC");
            GetSocketHandler().ProcessReply();
        } else {
            GetMessageHandler()("Timeout while waiting for replies from VMM",
                                "FEC_config_module::ReadADC", true);
        }
        GetSocketHandler().CloseAndDisconnect("FEC_config_module::ReadADC");
    }
    if(m_fec->m_hybrids[hybrid_index].m_vmms[vmm_index].GetRegister("sp") != 0 && m_fec->m_hybrids[hybrid_index].m_vmms[vmm_index].GetRegister("sm5_sm0") == 1) {
        ADCresult_int = 1200 - ADCresult_int;
    }
    return ADCresult_int;
}
// ------------------------------------------------------------------------ //

QString FECConfigModule::ReadI2C(int hybrid_index, int choice) {
    QString result;
    if(choice == 2) {
        result = ReadGeoPos(hybrid_index);
    }
    else if(choice == 1){
        result = ReadFirmwareVersion(hybrid_index);
    }
    else {
        result = ReadIDChip(hybrid_index);
    }

    return result;
}

QString FECConfigModule::ReadFirmwareVersion(int hybrid_index) {
    if(IsDbgEnabled()) GetMessageHandler()("Reading hybrid firmware version...","FEC_config_module::ReadFirmwareVersion");
    bool ok;
    QString result;
    if(g_slow_control != 2) {
        result = ESS_ReadSc("sc_i2c_firmware_version", hybrid_index, "hyb_i2c_firmware_version0" + QString::number(hybrid_index), ok);
    }
    else {
        result = CommunicateWithHybridI2C(65,hybrid_index, 0, 6, 1);
        result = CommunicateWithHybridI2C(65, hybrid_index, 1, 0, 4);
    }
    return result;
}



// ------------------------------------------------------------------------ //
QString FECConfigModule::CommunicateWithHybridI2C(int i2c_addr, int hybrid_index, int rw, int reg_value, int bytes)
{
    if(IsDbgEnabled())GetMessageHandler()("Setting/reading i2c on hybrid...","FEC_config_module::CommunicateWithHybridI2C");

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
    QString ip = m_fec->GetIP();

    QString hybridMapString = "00000000";
    hybridMapString.replace(7 -  g_hybrid_i2c[hybrid_index] , 1 , QString("1") );
    quint8 hybridMap = (quint8)hybridMapString.toInt(&ok,2);
    bool readOK = true;
    QString result ="-1";

    QDataStream out (&datagram, QIODevice::WriteOnly);


    datagram.clear();
    out.device()->seek(0); //rewind
    GetSocketHandler().UpdateCommandCounter();

    ////////////////////////////
    // header
    ////////////////////////////
    out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
        << (quint16) 0 //[4,5]
        << (quint8) hybridMap//146 //[6] Subaddress: enable MUX channels
        << (quint8) ((i2c_addr << 1) | rw) //[7]  I2C address, last bit: read/not write (1 = read);
        << (quint8) cmd.toUInt(&ok,16) //[8]
        << (quint8) cmdType.toUInt(&ok,16) //[9]
        << (quint16) cmdLength.toUInt(&ok,16); //[10,11]




    ////////////////////////////
    // word
    ////////////////////////////
    out << (quint32) 0 //[12,15]
        << (quint32) bytes //[16,19] //sc_address: write two bytes
        << (quint32) reg_value; //[20,23] //sc_value: first byte 0x0 register, second byte 0x0 value



    if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::")) {
        return result;
    }

    readOK = GetSocketHandler().WaitForReadyRead();

    if(rw == 1) {
        QByteArray read_datagram;
        read_datagram.clear();
        QHostAddress address;
        quint16 port;
        while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
            read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
            GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
            if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {
                result = read_datagram.mid(24-bytes,bytes).toHex();
                break;
            }
        }
    }
    if(readOK) {
        if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::CommunicateWithHybridI2C");
        GetSocketHandler().ProcessReply();
    } else {
        GetMessageHandler()("Timeout while waiting for replies from VMM",
                            "FEC_config_module::CommunicateWithHybridI2C", true);
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::CommunicateWithHybridI2C");
    if(readOK) {
        if(rw == 0) {
            result = "1";
        }
    }
    return result;
}



// ------------------------------------------------------------------------ //
bool FECConfigModule::CheckConfigurationOfVMMs(int hybrid_index, int vmm_index)
{
    //different phases of check
    //phase 0: BEFORE sending the VMM config, reset the register 0 (see send config method)
    //phase 1: read 1 byte from register 0 (poll bits 3 and 4)
    //phase 2: read 1 byte from register 0 (check bits 5 and 6)

    //Phase 1
    int counter = 0;
    int bitToPoll=0;
    int bitToCheck=0;
    QString result="0";
    bool ok;

    if(vmm_index == 0) {
        bitToPoll = 0x08;
        bitToCheck = 0x20;
    }
    else {
        bitToPoll = 0x10;
        bitToCheck = 0x40;
    }
    while(counter < 10) {
        counter++;
        if(g_slow_control != 2) {
            result = ESS_ReadSc("sc_i2c_config_check", hybrid_index, "hyb_i2c_config_check0" + QString::number(hybrid_index), ok);
            if(!ok) {
                result = -1;
            }
        }
        else {
            //CommunicateWithHybridI2C(int hybrid_index, int rw, int reg, int bytes)
            //send 1 byte of 0x00 to choose register 0
            result = CommunicateWithHybridI2C(65, hybrid_index, 0, 0, 1);
            //If problem with I2C, avoid pop-up message
            if(result == "-1") {
                return true;
            }
            //read 1 byte from register 0x00
            result = CommunicateWithHybridI2C(65, hybrid_index, 1, 0, 1);
        }
        //If problem with I2C, avoid pop-up message
        if(result == "-1") {
            return true;
        }

        uint32_t res = result.toUInt(&ok, 16);
        //std::cout << "Config check: reading cyle nr " << counter << ",  " << (res & bitToPoll) << std::endl;
        if((res & bitToPoll) ==  bitToPoll) {
            QThread::msleep(10);
            counter = 0;
            break;
        }
    }

    result = "0";
    if(counter == 0) {
        if(g_slow_control != 2) {
            result = ESS_ReadSc("sc_i2c_config_check", hybrid_index, "hyb_i2c_config_check0" + QString::number(hybrid_index), ok);
            if(!ok) {
                result = -1;
            }
        }
        else {
            //Phase 2
            //send 1 byte of 0x00 to choose register 0
            result = CommunicateWithHybridI2C(65, hybrid_index, 0, 0, 1);
            if(result == "-1") {
                return true;
            }
            //read 1 byte from register 0x00
            result = CommunicateWithHybridI2C(65, hybrid_index, 1, 0, 1);
        }
        if(result == "-1") {
            return true;
        }
        bool ok;
        int res = result.toUInt(&ok, 16);
        //std::cout << counter << " " << res << " " << bitToCheck << " " << (res & bitToCheck) << std::endl;
        if((res & bitToCheck) ==  0x00) {
            QThread::msleep(10);
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------------------------ //
QString FECConfigModule::ReadGeoPos(int hybrid_index)
{
    if(IsDbgEnabled())GetMessageHandler()("Reading Geo-Pos on hybrid...","FEC_config_module::ReadGeoPos");

    bool ok;
    QString result="0";
    if(g_slow_control != 2) {
        result = ESS_ReadSc("sc_i2c_geo_id", hybrid_index, "hyb_i2c_geoid0" + QString::number(hybrid_index), ok);
    }
    else {
        //send 1 byte of 0 to choose register 0
        result = CommunicateWithHybridI2C(66, hybrid_index, 0, 2, 1);

        //read 1 byte from register 0
        result = CommunicateWithHybridI2C(66, hybrid_index, 1, 0, 1);
    }
    return result.mid(6);

}
// ------------------------------------------------------------------------ //



QString FECConfigModule::ReadIDChip(int hybrid_index)
{
    if(IsDbgEnabled())GetMessageHandler()("Reading ID chip on hybrid via i2c...","FEC_config_module::ReadIDChip");
    QString result;
    bool ok;
    if(g_slow_control != 2) {
        QString id_part = ESS_ReadSc("sc_i2c_hybrid_id", hybrid_index, "hyb_i2c_hybrid_id00" + QString::number(hybrid_index), ok);
        result.append(id_part);
        id_part = ESS_ReadSc("sc_i2c_hybrid_id", hybrid_index, "hyb_i2c_hybrid_id10" + QString::number(hybrid_index), ok);
        result.append(id_part);
        id_part = ESS_ReadSc("sc_i2c_hybrid_id", hybrid_index, "hyb_i2c_hybrid_id20" + QString::number(hybrid_index), ok);
        result.append(id_part);
        id_part = ESS_ReadSc("sc_i2c_hybrid_id", hybrid_index, "hyb_i2c_hybrid_id30" + QString::number(hybrid_index), ok);
        result.append(id_part);
    }
    else {
        QByteArray datagram;

        // send call to i2c port
        int send_to_port = m_fec->GetRegVal("i2c_port");

        //header
        QString cmd, cmdType, cmdLength, msbCounter;
        cmd = "AA";
        cmdType = "AA";
        cmdLength = "FFFF";
        msbCounter = "0x80000000";
        QString ip = m_fec->GetIP();
        QString hybridMapString = "00000000";
        hybridMapString.replace(7 -  g_hybrid_i2c[hybrid_index] , 1 , QString("1") );
        quint8 hybridMap = (quint8)hybridMapString.toInt(&ok,2);
        bool readOK = true;
        int i2c_addr = 88;
        int reg = 0x80;
        for(uint32_t n=1; n<=4; n++) {

            datagram.clear();
            QDataStream out (&datagram, QIODevice::WriteOnly);
            out.device()->seek(0); //rewind
            GetSocketHandler().UpdateCommandCounter();

            uint32_t byte_position_id = n*4;
            //////////////////////////////////////////////////////////////////
            // 1st i2c command: set address pointer to chip register (chip) //
            //////////////////////////////////////////////////////////////////


            ////////////////////////////
            // header
            ////////////////////////////
            out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
                << (quint16) 0 //[4,5]
                << (quint8) hybridMap//146 //[6] Subaddress: enable MUX channels
                << (quint8) ((i2c_addr << 1) | 0) //[7]  I2C address, last bit: read/not write (1 = read);
                << (quint8) cmd.toUInt(&ok,16) //[8]
                << (quint8) cmdType.toUInt(&ok,16) //[9]
                << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

            ////////////////////////////
            // word
            ////////////////////////////
            out << (quint32) 0 //[12,15]
                << (quint32) 0 //[16,19] // goes to sc_address, for the ID chip determines at which position for bytes of the 16 byte ID are read
                << (quint32) reg; //[20,23] //defines which register of Geocode chip or ID chip is going to be read


            if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ReadIDChip")) {
                return 0;
            }

            readOK = true;
            readOK = GetSocketHandler().WaitForReadyRead();
            if(readOK) {
                if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadIDChip");
                GetSocketHandler().ProcessReply();
            } else {
                GetMessageHandler()("Timeout while waiting for replies from VMM",
                                    "FEC_config_module::ReadIDChip", true);
                GetSocketHandler().CloseAndDisconnect("FEC_config_module::ReadIDChip");
                return 0;
            }

            ///////////////////////////////////////////////////////////
            // 2nd i2c command: read result form register //
            ///////////////////////////////////////////////////////////
            datagram.clear();
            out.device()->seek(0); //rewind


            GetSocketHandler().UpdateCommandCounter();

            ////////////////////////////
            // header
            ////////////////////////////
            out << (quint32)(GetSocketHandler().GetCommandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
                << (quint16) 0 //[4,5]
                << (quint8) hybridMap//146 //[6] Subaddress: enable MUX channels
                << (quint8) ((i2c_addr << 1) | 1) //[7] I2C address, last bit: read/not write (1 = read)
                << (quint8) cmd.toUInt(&ok,16) //[8]
                << (quint8) cmdType.toUInt(&ok,16) //[9]
                << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

            ////////////////////////////
            // word
            ////////////////////////////
            out << (quint32) 0 //[12,15]
                << (quint32) byte_position_id //[16,19] // defines which part of the chip id is read
                << (quint32) 0x00000000; //[20,23] //goes to sc_data to ADC, must be 3 bytes
            // can be anything here but needs to be 3 byte long

            if(!GetSocketHandler().SendDatagram(datagram, ip, send_to_port, "FEC_config_module::ReadIDChip")) {
                return 0;
            }
            readOK = true;
            readOK = GetSocketHandler().WaitForReadyRead();


            QByteArray read_datagram;
            read_datagram.clear();
            QHostAddress address;
            quint16 port;
            while(GetSocketHandler().GetSocket().hasPendingDatagrams()) {
                read_datagram.resize(GetSocketHandler().GetSocket().pendingDatagramSize() );
                GetSocketHandler().GetSocket().readDatagram(read_datagram.data(), read_datagram.size(), &address, &port);
                if(CheckIpPortSize(address, ip,port,send_to_port,read_datagram.size())) {
                    QString id_part = read_datagram.mid(20,4).toHex();
                    result.append(id_part);
                    break;
                }
            }

            if(readOK) {
                if(IsDbgEnabled())GetMessageHandler()("Processing replies...","FEC_config_module::ReadIDChip");
                GetSocketHandler().ProcessReply();
            } else {
                GetMessageHandler()("Timeout while waiting for replies from VMM",
                                    "FEC_config_module::ReadIDChip", true);
                result = "0";
            }

        }
    }
    GetSocketHandler().CloseAndDisconnect("FEC_config_module::ReadIDChip");
    return result;
}
// ------------------------------------------------------------------------ //


bool FECConfigModule::CheckIpPortSize(QHostAddress ipRead, QString ip,int portRead, int port, int size) {
    bool conversionOK = false;
    QHostAddress ip4Address(ipRead.toIPv4Address(&conversionOK));
    QString ip4String;
    if (conversionOK)
    {
        ip4String = ip4Address.toString();
    }
    if(ip4String != ip) {
        return false;
    }
    if(portRead != port) {
        return false;
    }
    if(size < 16) {
        return false;
    }
    return true;
}
