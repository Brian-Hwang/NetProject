#ifndef TPSENDER_H_
#define TPSENDER_H_

#include "ns3/application.h"
//#include "ns3/event-id.h"
//#include "ns3/type-id.h"
#include "ns3/traced-callback.h"
#include "ns3/data-rate.h"
//#include "ns3/ptr.h"
//#include "ns3/ipv4-address.h"

namespace ns3 {

class Socket;
class Packet;

class ns3::TPSender : public Application
{
    public:
        static TypeId GetTypeId(void);
        TPSender();
        virtual ~TPSender();

    private:
        virtual void StartApplication(void);
        virtual void StopApplication(void);

        //function for scheduling next packet send
        void ScheduleTx(void);

        //function for actually sending a packet
        void SendPacket(void);

        Address m_address;
        uint32_t m_nPackets;
        DataRate m_dataRate;

        Ptr<Socket> m_socket;
        uint32_t m_packetSize;
        uint32_t m_packetsSent;
        EventId m_sendEvent;
        bool m_running;

        TracedCallback<Ptr<const Packet>> m_txTrace;
        TracedCallback<Ptr<const Packet>> m_rxTrace;
};

};//namespace n3
#endif
