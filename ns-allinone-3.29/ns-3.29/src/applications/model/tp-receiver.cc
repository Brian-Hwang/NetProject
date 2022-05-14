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
#include "ns3/tp-receiver.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("TPReceiver");

NS_OBJECT_ENSURE_REGISTERED (TPReceiver);

TypeId TPReceiver::GetTypeId(void){
    static TypeId tid = TypeId ("ns3::TPReceiver")
        .SetParent<Application>()
        .AddConstructor<TPReceiver>()
        .AddAttribute("Local", "The Address on which to Bind the RX socket.",
                AddressValue(),
                MakeAddressAccessor(&TPReceiver::m_address),
                MakeAddressChecker())
        .AddTraceSource("Rx", "A packet has been received",
                MakeTraceSourceAccessor(&TPReceiver::m_rxTrace), "ns3::Packet::TracedCallback")
        ;
    return tid;
}

TPReceiver::TPReceiver()
    :m_socket(0),
    m_totalRx(0),
    m_running(false)
{
    NS_LOG_FUNCTION(this);
}

TPReceiver::~TPReceiver ()
{
  NS_LOG_FUNCTION (this);
}

void TPReceiver::StartApplication(void){
    NS_LOG_FUNCTION(this);

    if(!m_socket){
        //this is udp for now [TODO] decide UDP or TCP
        TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        m_socket = Socket::CreateSocket(GetNode(), tid);
        m_socket->Bind(m_address);
        m_socket->Listen();
        m_socket->ShutdownSend();
        m_socket->SetRecvCallback(MakeCallback(&TPReceiver::HandleRead, this));
    }
}

void TPReceiver::HandleRead(Ptr<Socket> socket){
    NS_LOG_FUNCTION(this);
    Ptr<Packet> packet;
    Address from;
    while((packet = m_socket->RecvFrom(from))){
        if(packet->GetSize() > 0){
            m_totalRx++;
            m_rxTrace(packet);
        }
    }
}

void TPReceiver::StopApplication(){
    NS_LOG_FUNCTION(this);
    m_running = false;
    if(m_socket){
        m_socket->Close();
    }
}
    
};
