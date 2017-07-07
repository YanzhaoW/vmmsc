#ifndef SRSCONFIG_H
#define SRSCONFIG_H

#include <iostream>
#include <string>

//mmdaq
#include "fecm.h"


class SRSConfig {
    public :
        SRSConfig();
        void loadXml(std::string filename);
        std::string getFecContainsChipName(std::string chipname);

        void print();

        std::string getConfigXml() { return m_configXml; }


    private :
        bool readXMLfile(const std::string filename);

        std::string m_configXml;

        #warning why not make a vector of pointers?
        int m_fecArraySize;
        FecM **m_fecArray;

}; // class

#endif
