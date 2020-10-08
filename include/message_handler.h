#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

// qt
#include <QObject>
#include <QString>

// std/stl
#include <iostream>
#include <sstream>

/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------- //
//  MessageHandler
// ----------------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////////////

class MessageHandler : public QObject
{
    Q_OBJECT;

    public :
        explicit MessageHandler(QObject *parent = 0);
        virtual ~MessageHandler(){};
        void SetGUI(bool set) { m_gui = set; }
        bool GetGUI() { return m_gui; }
        void SetMessageSize(unsigned int size) { m_size = size; }

        std::string GetBuffer() { return m_buffer.str(); }
        void ClearBuffer() { m_buffer.str(""); }

        /// operators
        void operator () (std::stringstream& s, std::string caller,
                                                    bool exit = false);

        void operator () (std::stringstream& s, const char* caller,
                                                        bool exit = false);

        void operator () (std::stringstream& s, bool exit = false);

        void operator () (std::string s, std::string caller,
                                                    bool exit = false);

        void operator () (std::string s, const char* caller,
                                                    bool exit = false);

        void operator () (const char* m, const char* caller, bool exit = false);

        void operator () (std::string s, bool exit = false);

        void operator () (const char* msg, bool exit = false);

    private :
        bool m_gui;
        unsigned int m_size;
        int m_callborder;

        std::stringstream m_buffer;

    signals :
        void on_log_ready();

    public slots :

};

#endif
