#include "ns3/address.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/tp-sender.h"
#include "ns3/string.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("TPSender");

NS_OBJECT_ENSURE_REGISTERED (TPSender);

TypeId TPSender::GetTypeId(void){
    static TypeId tid = TypeId ("ns3::TPSender")
        .SetParent<Application>()
        .AddConstructor<TPSender>()
        .AddAttribute("Address", "The address",
                AddressValue(),
                MakeAddressAccessor(&TPSender::m_address),
                MakeAddressChecker())
        .AddAttribute("NPackets", "The total number of packets to send",
                UintegerValue(10000),
                MakeUintegerAccessor(&TPSender::m_nPackets),
                MakeUintegerChecker<uint32_t>())
        .AddAttribute("DataRate", "The data rate",
                DataRateValue(DataRate("500kb/s")),
                MakeDataRateAccessor(&TPSender::m_dataRate),
                MakeDataRateChecker())
        .AddTraceSource("Rx", "A new packet is created and is sent",
                MakeTraceSourceAccessor(&TPSender::m_rxTrace), "ns3::Packet::TracedCallback")
        .AddTraceSource("Tx", "A new packet is created and is sent",
                MakeTraceSourceAccessor(&TPSender::m_txTrace), "ns3::Packet::TracedCallback")
        ;
    return tid;
}

TPSender::TPSender()
    :m_socket(0),
    m_packetSize(1000),
    m_packetsSent(0),
    m_running(false)
{
    NS_LOG_FUNCTION(this);
}

TPSender::~TPSender ()
{
  NS_LOG_FUNCTION (this);
}

void TPSender::StartApplication(void){
    NS_LOG_FUNCTION(this);

    //if there is no socket yet, create one
    if(!m_socket){
        //this is udp for now [TODO] decide UDP or TCP
        TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        m_socket = Socket::CreateSocket(GetNode(), tid);
        m_socket->Bind();
        m_socket->Connect(m_address);
    }

    //indicate that application is running now
    m_running = true;

}

void TPSender::computeResponse(void){
    NS_LOG_FUNCTION(this);
    uint8_t move = 0;
    /*Receive Packet with frame here*/

    /*Insert AI for movement here*/
    
    //assuming that a variable 'move' is set, which is 0 when no movement is required
    if(move){
        char *buf = new char[2];
        buf[0] = (char)move;
        buf[1] = '\0';
        SendPacket(buf);
    }
}

void TPSender::SendPacket(char *payload){
    NS_LOG_FUNCTION(this);

    uint8_t *buf = new uint8_t[strlen(payload)];
    for (uint8_t i = 0; i < strlen(payload); i++)
        buf[i] = (uint8_t)payload[0];
    
    NS_LOG_DEBUG("Converted char to " << buf);

    Ptr<Packet> packet = Create<Packet> (buf, 2);
    
    m_txTrace(packet);

    //send packet over socket (this is fully abstracted, no need to change based on TCP/UDP)
    m_socket->Send(packet);

    m_packetsSent++;

    delete[] payload;
    delete[] buf;
}

void TPSender::StopApplication(){
    NS_LOG_FUNCTION(this);
    m_running = false;
    if(m_sendEvent.IsRunning()){
    
        Simulator::Cancel(m_sendEvent);
    }

    if(m_socket){
        m_socket->Close();
    }
}
    
};
