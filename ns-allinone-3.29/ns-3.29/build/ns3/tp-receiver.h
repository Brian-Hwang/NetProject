#ifndef TPRECV_H_
#define TPRECV_H_

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include "ns3/data-rate.h"
#include "ns3/socket.h" 
#include "ns3/boolean.h" 
#include "ns3/log.h" 
#include "ns3/uinteger.h" 
#include "ns3/string.h"
#include "ns3/nstime.h"
#include <iostream>
#include <fstream>

namespace ns3{

//class Socket;
//class Packet;

class TPReceiver : public Application
{
    public:
        static TypeId GetTypeId(void);
        TPReceiver();
        virtual ~TPReceiver();

    private:
        virtual void StartApplication(void);
        virtual void StopApplication(void);

        void HandleRead(Ptr<Socket> socket);
        void ScheduleDisplay(void);
        void Display(void);
        char* NextFrame(uint16_t);

        Address m_address;

        Ptr<Socket> m_socket;
        uint32_t m_totalRx;
        bool m_running;
        uint8_t m_fieldSize;
        bool m_fileIO;
        uint8_t m_currPos;
        Time m_dispFreq;
        EventId m_displayEvent;
        std::string m_inFilename;
        std::string m_outFilename;
        std::ifstream m_inFile;
        std::ofstream m_outFile;
        TracedCallback<Ptr<const Packet>> m_rxTrace;
        TracedCallback<Ptr<const Packet>> m_txTrace;
};
};
#endif

