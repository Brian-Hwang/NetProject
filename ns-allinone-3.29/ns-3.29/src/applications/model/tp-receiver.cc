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
        .AddAttribute("InFile", "The name of the input file to get data for calculation from.",
                StringValue(),
                MakeStringAccessor(&TPReceiver::m_inFilename),
                MakeStringChecker())
        .AddAttribute("OutFile", "The name of the output file to write received data to.",
                StringValue(),
                MakeStringAccessor(&TPReceiver::m_outFilename),
                MakeStringChecker())
        .AddTraceSource("Rx", "A packet has been received",
                MakeTraceSourceAccessor(&TPReceiver::m_rxTrace), "ns3::Packet::TracedCallback")
        ;
    return tid;
}

TPReceiver::TPReceiver()
    :m_socket(0),
    m_totalRx(0),
    m_running(false),
    m_inFile(NULL),
    m_outFile(NULL)
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

    if(!m_inFile){
        m_inFile.open(m_inFilename);
        NS_LOG_INFO("Opening Input File...");
        if(m_inFile.is_open()){
            NS_LOG_INFO("Opened Input File!");
        }
    }
    
    if(!m_outFile){
        m_outFile.open(m_outFilename);
        NS_LOG_INFO("Opening Output File...");
        if(m_outFile.is_open()){
            NS_LOG_INFO("Opened Output File!");
        }
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
            uint8_t *payload = new uint8_t [packet->GetSize()];
            packet->CopyData(payload, packet->GetSize());
            NS_LOG_INFO("Received " << payload << " from sender.");
            m_outFile << reinterpret_cast<char*>(payload);
            delete[] payload;
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
