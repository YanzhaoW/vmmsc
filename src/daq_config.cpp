#include "daq_config.h"
#include <QFile>
#include <QObject>

DAQConfig::DAQConfig(DAQWindow *top, QObject *parent)
    : QObject(parent), m_daqWindow{top} {
    // getcwd(m_execPath,sizeof(m_execPath));
}

bool DAQConfig::LoadDAQConf(const char *filename) {
    // add config path before file name
    std::string fname = m_daqWindow->GetApplicationPath().toStdString();
    fname += "/../";
    fname += CONFIG_DIR;
    fname += "/";
    fname += filename;
    std::cout << filename << " " << fname.c_str() << std::endl;
    return LoadDAQConfig(fname);
}

bool DAQConfig::WriteDAQConf(const char *filename) {
    std::string fname = m_daqWindow->GetApplicationPath().toStdString();
    fname += "/../";
    fname += CONFIG_DIR;
    fname += "/";
    fname += filename;
    return WriteDAQConfig(fname);
}

bool DAQConfig::LoadDAQConfig(std::string fname) {
    QFile file;
    file.setFileName(QString::fromStdString(fname));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject globalObject = doc.object();

    QJsonArray fecArray = globalObject["fecs"].toArray();
    for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
        m_daqWindow->m_daq.SetFEC(fec, false);
        m_daqWindow->m_daq.m_fecs[fec].SetInfo("board_id",QString::number(0).toStdString());
        for (unsigned short hyb=0; hyb < HYBRIDS_PER_FEC; hyb++){
            m_daqWindow->m_daq.m_fecs[fec].SetHybrid(hyb, false);
        }
    }

    foreach (const QJsonValue &value, fecArray) {
        const auto &fecObj = value.toObject();
        unsigned long fec = fecObj["fec"].toInt();
        m_daqWindow->m_daq.SetFEC(fec, true);
        QString description = fecObj["description"].toString();
        m_daqWindow->m_daq.m_fecs[fec].SetInfo("description", description.toStdString());


        for (unsigned short j = 0; j < 19;  j++) {
            if (m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) != tr("not_used")) {
                if (m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) == tr("ip_fec") ||
                    m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) == tr("ip_daq")) {
                    QHostAddress ip;
                    QString val =
                        fecObj[m_daqWindow->m_daq.m_fecs[fec].GetRegName(j)].toString();
                    ip.setAddress(val);
                    unsigned long theIP = ip.toIPv4Address();
                    m_daqWindow->m_daq.m_fecs[fec].SetReg(m_daqWindow->m_daq.m_fecs[fec].GetRegName(j),theIP);
                }
                else {
                    unsigned long val =
                        fecObj[m_daqWindow->m_daq.m_fecs[fec].GetRegName(j)].toInt();
                    m_daqWindow->m_daq.m_fecs[fec].SetReg(
                        m_daqWindow->m_daq.m_fecs[fec].GetRegName(j), val);
                }
            }
        }
        m_daqWindow->m_daq.m_fecs[fec].SetId();
        QJsonArray hybridArray = fecObj["hybrids"].toArray();
        foreach (const QJsonValue &value, hybridArray) {
            const auto &hybridObj = value.toObject();
            unsigned long hyb = hybridObj["hybrid"].toInt();
            m_daqWindow->m_daq.m_fecs[fec].SetHybrid(hyb, true);
            unsigned long val = hybridObj["TP_skew"].toInt();
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].SetReg("TP_skew", val);
            val = hybridObj["TP_width"].toInt();
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].SetReg("TP_width", val);
            val = hybridObj["TP_pol"].toInt();
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].SetReg("TP_pol", val);
            val = hybridObj["TP_disable"].toInt();
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].SetReg("TP_disable", val);
            QString description = hybridObj["description"].toString();
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].SetInfo("description", description.toStdString());

            for (int vmm = 0; vmm < 2; vmm++) {
                QString name = "vmm" + QString::number(vmm);
                const auto &vmmObj = hybridObj[name].toObject();
                QString description = vmmObj["description"].toString();
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].SetInfo("description", description.toStdString());

                for (const auto &vmmSetting : (*m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].m_vmmSettings->m_globalRegs)) {
                    unsigned int val =  vmmObj[QString::fromStdString(vmmSetting.first)].toInt();
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].SetRegister(vmmSetting.first, val);
                }



                for (unsigned short k = 0; k < VMM_CHANNELS; k++) {
                    QString name = "channel" + QString::number(k).rightJustified(2, '0');
                    const auto &channelObj = vmmObj[name].toObject();
                    for (auto const &channelSetting : m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].m_vmmSettings->m_channels[k].m_channel) {
                        unsigned long val = channelObj[QString::fromStdString(channelSetting.first)].toInt();
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].SetRegister(channelSetting.first, val, k);
                    }
                }
            }
        }
    }
    return true;
}

bool DAQConfig::WriteDAQConfig(std::string fname) {
    QJsonObject globalObject;
    QJsonArray fecArray;
    QFile jsonFile(QString::fromStdString(fname));
    jsonFile.open(QFile::WriteOnly);

    for (unsigned short fec = 0; fec < FECS_PER_DAQ; fec++) {
        if (m_daqWindow->m_daq.GetFEC(fec)) {
            QJsonObject fecObject;
            fecObject.insert("fec", fec);
            QString description = QString::fromStdString(
                m_daqWindow->m_daq.m_fecs[fec].GetInfo("description"));
            fecObject.insert("description", description);
            int ring = 0;
            int fen = 0;
            if (g_card_name == "FEN") {
                ring = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("ring");
                fen = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("fen");
            }
            fecObject.insert("ring", ring);
            fecObject.insert("fen", fen);

            for (unsigned short j = 0; j <19; j++) {
                if (m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) != tr("not_used")) {
                    if (m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) == tr("ip_fec") || m_daqWindow->m_daq.m_fecs[fec].GetRegName(j) ==
                            tr("ip_daq")) {
                        QHostAddress ip;
                        ip.setAddress(m_daqWindow->m_daq.m_fecs[fec].GetReg(j));
                        fecObject.insert(
                            m_daqWindow->m_daq.m_fecs[fec].GetRegName(j),
                            QString::fromStdString(ip.toString().toStdString()));
                    }
                    else {
                        int val = m_daqWindow->m_daq.m_fecs[fec].GetRegVal(j);
                        fecObject.insert(m_daqWindow->m_daq.m_fecs[fec].GetRegName(j),val);
                    }
                }
            }
            QJsonArray hybridArray;
            for (unsigned short hyb = 0; hyb < HYBRIDS_PER_FEC; hyb++) {
                if (m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hyb)) {
                    QJsonObject hybridObject;
                    hybridObject.insert("hybrid", hyb);
                    hybridObject.insert(
                        "description",
                        QString::fromStdString(
                            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].GetInfo(
                                "description")));
                    hybridObject.insert(
                        "TP_skew",
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].GetReg("TP_skew"));
                    hybridObject.insert(
                        "TP_width",
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].GetReg("TP_width"));
                    hybridObject.insert(
                        "TP_pol",
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].GetReg("TP_pol"));
                    hybridObject.insert(
                        "TP_disable",
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].GetReg(
                            "TP_disable"));
                    for (unsigned short vmm = 0; vmm < VMMS_PER_HYBRID; vmm++) {
                        QJsonObject vmmObject;
                        vmmObject.insert("vmm", vmm);
                        vmmObject.insert(
                            "description",
                            QString::fromStdString(
                                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].GetInfo(
                                    "description")));
                        for (const auto &entr : (*m_daqWindow->m_daq.m_fecs[fec]
                                                      .m_hybrids[hyb]
                                                      .m_vmms[vmm]
                                                      .m_vmmSettings->m_globalRegs)) {
                            if (entr.first != "scmx") {
                                if (entr.first == "sm5_sm0") {
                                    if (m_daqWindow->m_daq.m_fecs[fec]
                                            .m_hybrids[hyb]
                                            .m_vmms[vmm]
                                            .m_vmmSettings->m_globalRegs->at("scmx") == 0) {
                                        vmmObject.insert(QString::fromStdString(entr.first),
                                                         entr.second + 63);
                                    } else {
                                        vmmObject.insert(QString::fromStdString(entr.first),
                                                         entr.second);
                                    }

                                } else {
                                    vmmObject.insert(QString::fromStdString(entr.first),
                                                     entr.second);
                                }
                            }
                        }
                        for (unsigned short k = 0; k < VMM_CHANNELS; k++) {
                            QJsonObject channelObject;
                            for (auto const &entr : m_daqWindow->m_daq.m_fecs[fec]
                                                        .m_hybrids[hyb]
                                                        .m_vmms[vmm]
                                                        .m_vmmSettings->m_channels[k]
                                                        .m_channel) {
                                channelObject.insert(QString::fromStdString(entr.first),
                                                     entr.second);
                            }
                            vmmObject.insert("channel" +
                                                 QString::number(k).rightJustified(2, '0'),
                                             channelObject);
                        }
                        if (vmm == 0) {
                            hybridObject.insert("vmm0", vmmObject);
                        } else {
                            hybridObject.insert("vmm1", vmmObject);
                        }
                    }
                    hybridArray.push_back(hybridObject);
                }
                fecObject.insert("hybrids", hybridArray);
            }
            fecArray.push_back(fecObject);
        }
    }
    globalObject.insert("fecs", fecArray);
    QJsonDocument document(globalObject);
    jsonFile.write(document.toJson(QJsonDocument::JsonFormat::Indented));
    jsonFile.close();
    return true;
}

DAQConfig::~DAQConfig() {}
