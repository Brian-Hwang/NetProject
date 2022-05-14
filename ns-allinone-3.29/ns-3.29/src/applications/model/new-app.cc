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
#include "ns3/new-app.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("NewApp");

NS_OBJECT_ENSURE_REGISTERED (NewApp);

TypeId NewApp::GetTypeId(void){
    static TypeId tid = TypeId ("ns3::NewApp")
        .SetParent<Application>()
        .AddConstructor<NewApp>()
        .AddAttribute("Mode", "Sender(true), Receiver(false)",
                BooleanValue(false),
                MakeBooleanAccessor(&NewApp::m_mode),
                MakeBooleanChecker())
        .AddAttribute("Address", "The address",
                AddressValue(),
                MakeAddressAccessor(&NewApp::m_address),
                MakeAddressChecker())
        .AddAttribute("NPackets", "The total number of packets to send",
                UintegerValue(10000),
                MakeUintegerAccessor(&NewApp::m_nPackets),
                MakeUintegerChecker<uint32_t>())
        .AddAttribute("DataRate", "The data rate",
                DataRateValue(DataRate("500kb/s")),
                MakeDataRateAccessor(&NewApp::m_dataRate),
                MakeDataRateChecker())
        .AddTraceSource("Tx", "A new packet is created and is sent",
                MakeTraceSourceAccessor(&NewApp::m_txTrace), "ns3::Packet::TracedCallback")
        .AddTraceSource("Rx", "A packet has been received",
                MakeTraceSourceAccessor(&NewApp::m_rxTrace), "ns3::Packet::TracedCallback")
        ;
    return tid;
}

NewApp::NewApp()
    :m_socket(0),
    m_packetSize(1000),
    m_packetsSent(0),
    m_running(false)
{
    NS_LOG_FUNCTION(this);
}

NewApp::~NewApp ()
{
  NS_LOG_FUNCTION (this);
}

void NewApp::StartApplication(void){
    NS_LOG_FUNCTION(this);

    if(m_mode == true){
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

        //Send first packet
        SendPacket();
    }else{
        if(!m_socket){
            //this is udp for now [TODO] decide UDP or TCP
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            m_socket->Bind(m_address);
            m_socket->Listen();
            m_socket->ShutdownSend();
            m_socket->SetRecvCallback(MakeCallback(&NewApp::HandleRead, this));
        }
        
    }
}

void NewApp::SendPacket(void){
    NS_LOG_FUNCTION(this);

    Ptr<Packet> packet = Create<Packet> (m_packetSize);
    m_txTrace(packet);

    //send packet over socket (this is fully abstracted, no need to change based on TCP/UDP)
    m_socket->Send(packet);

    //continue sending until all number of packets have been sent
    if(++m_packetsSent < m_nPackets){
        ScheduleTx();
    }
}

void NewApp::ScheduleTx(void){
    NS_LOG_FUNCTION(this);
    if(m_running){
        //getting next transmission tiome based on transmission rate
        Time tNext(
                Seconds (m_packetSize*8/static_cast<double>(m_dataRate.GetBitRate())));
        
        //creating new send event with the scheduler using the calculated time and the function call being scheduled, here: SendPacket()
        m_sendEvent = Simulator::Schedule(tNext, &NewApp::SendPacket, this);
    }
}

void NewApp::HandleRead(Ptr<Socket> socket){
    NS_LOG_FUNCTION(this);
    Ptr<Packet> packet;
    Address from;
    while((packet = m_socket->RecvFrom(from))){
        if(packet->GetSize() > 0){
            m_rxTrace(packet);
        }
    }
}

void NewApp::StopApplication(){
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
