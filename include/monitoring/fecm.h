#ifndef FECM_H
#define FECM_H

//std/stl
#include <iostream>
#include <string>
#include "chip.h"

//boost
#include <boost/property_tree/ptree.hpp>

class FecM {
    public :
        FecM();
        bool loadFec(const boost::property_tree::ptree::value_type pt);

        std::string getId() { return m_id; }
        std::string getIp() { return m_ip; }

        void print();

        #warning make vector
        Chip **m_chipArray;
        int m_chipArraySize;

    private :

        std::string m_dcs_file; // obsolete?
        std::string m_name;
        std::string m_ip;
        std::string m_id;

}; // class


#endif
