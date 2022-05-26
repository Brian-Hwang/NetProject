#ifndef GAMEUSER_H_
#define GAMEUSER_H_

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
#include <iostream>
#include <fstream>

namespace ns3
{

    class GameUser : public Application
    {
    public:
        static TypeId GetTypeId(void);
        GameUser();
        virtual ~GameUser();

    private:
        virtual void StartApplication(void);
        virtual void StopApplication(void);

        // function for actually sending a packet
        void SendPacket(char *payload);
        void computeResponse(void);
        Address m_address;
        uint32_t m_nPackets;
        DataRate m_dataRate;

        Ptr<Socket> m_socket;
        uint32_t m_packetSize;
        uint32_t m_packetsSent;
        bool m_running;

        TracedCallback<Ptr<const Packet>> m_rxTrace;
        TracedCallback<Ptr<const Packet>> m_txTrace;
    };
};
#endif
