#include "../include/calibration_module.h"
using namespace std;

calibration_module::calibration_module(QObject *parent) :
    QObject(parent),
     m_DAQSocket(0),
     m_daqSocket(0),
     m_calibRun(false),
     m_msg(new MessageHandler),
     m_ignore16(false)
{

}

// ------------------------------------------------------------------------ //
void calibration_module::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
    // remove monitoring m_daqMonitor->LoadMessageHandler(msg());
}
// ------------------------------------------------------------------------ //

void calibration_module::connectDAQSocket()
{
    stringstream sx;

    int daqport = 6006;

    if(!m_DAQSocket) {
        msg()("Initializing DAQ socket...","DataHandler::connectDAQSocket");
        m_DAQSocket = new QUdpSocket();
        connect(m_DAQSocket, SIGNAL(readyRead()), this, SLOT(readEvent()));
    }

    if(m_DAQSocket->state() == QAbstractSocket::UnconnectedState) {
        if(dbg()){
            sx << "About to re-bind DAQ socket";
            msg()(sx,"DataHandler::connectDAQSocket"); sx.str("");
        }
        bool bnd = m_DAQSocket->bind(daqport, QUdpSocket::ShareAddress);
        if(!bnd) {
            sx << "ERROR Unable to re-bind DAQ socket to port " << daqport;
            msg()(sx, "DataHandler::connectDAQSocket"); sx.str("");
            if(dbg()) {
                sx << "Closing and disconnecting DAQ socket";
                msg()(sx,"DataHandler::connectDAQSocket"); sx.str("");
            }
            m_DAQSocket->close();
            m_DAQSocket->disconnectFromHost();
        } // not bnd correctly
        else {

            if(dbg()) {
                sx << "DAQ socket successfully bound to port " << daqport;
                msg()(sx,"DataHandler::connectDAQSocket"); sx.str("");
            }
        } // bnd ok
    }
}
// ------------------------------------------------------------------------ //
void calibration_module::closeDAQSocket()
{
    // close the socket
    if(dbg()) msg()("Closing DAQ socket", "DataHandler::closeDAQSocket");
    m_DAQSocket->close();
    m_DAQSocket->disconnectFromHost();
}
void calibration_module::readEvent()
{
    stringstream sx;

    bool ok_to_read = true;

//    if(!m_fileOK) ok_to_read = false;
//    if(m_write && !m_rootFileOK) ok_to_read = false;
//    if(!ok_to_read) return;

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

           //if(dbg() && verbose) {
           if(true){
               sx.str("");
               headerStr        = datagram.mid(4,4).toHex();
               fullEventDataStr = datagram.mid(12, datagram.size()).toHex();
               sx << "*****************************************************\n"
                  << " Data from chip # : " << chipNumberStr.toInt(&ok,16) << "\n" //toStdString() << "\n"
                  << "  > Header        : " << headerStr.toStdString() << "\n"
                  << "  > Data          : " << fullEventDataStr.toStdString() << "\n"
                  << "*****************************************************";
               cout << sx.str() << endl;
               //msg()(sx,"DataHandler::decodeAndWriteData");
           } //dbg

           if(datagram.size()==12 && dbg()) {
               sx.str("");
               sx << "Empty event from chip #: " << chipNumberStr.toInt(&ok,16);
               cout << sx.str() << endl;
               msg()(sx,"DataHandler::decodeAndWriteData"); sx.str("");
           }

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
                          << " DataHandler::setCalibrationChannel";
                       msg()(sx, "DataHandler::decodeAndWriteData"); sx.str("");
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
                      << "charge           : " << outCharge_ << "\n"
                      << "q_1              : " << q_1.toStdString() << "\n"
                      << "q_2              : " << q_2.toStdString() << "\n"
                      << "q_final          : " << q_final.toStdString() << "\n"
                      << "tac              : " << outTac_ << "\n"
                      << "bcid-gray        : " << gray << "\n"
                      << "bcid             : " << outBCID_ << "\n";
                   cout << sx.str() << endl;
                   //msg()(sx,"DataHandler::decodeAndWriteData");
                   //msg()(" "," ");
               } // dbg

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
        cout << "DataHandler::reverse32    Input datagram is larger than 32 bits!" << endl;
        cout << "DataHandler::reverse32    >>> Exiting." << endl;
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


