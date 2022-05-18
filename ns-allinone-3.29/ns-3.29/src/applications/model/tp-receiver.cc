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
        .AddAttribute("GameSize", "Size of the displayed field in game.",
                UintegerValue(),
                MakeUintegerAccessor(&TPReceiver::m_fieldSize),
                MakeUintegerChecker<uint8_t>())
        .AddAttribute("DisplayFreq", "Time between displaying different frames. (FPS maybe?)",
                TimeValue(Seconds(0.1)),
                MakeTimeAccessor(&TPReceiver::m_dispFreq),
                MakeTimeChecker())
        .AddTraceSource("Rx", "A packet has been received",
                MakeTraceSourceAccessor(&TPReceiver::m_rxTrace), "ns3::Packet::TracedCallback")
        ;
    return tid;
}

TPReceiver::TPReceiver()
    :m_socket(0),
    m_totalRx(0),
    m_running(false),
    m_currPos((int)(m_fieldSize/2)),
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

    m_running = true;

    Display();
}

void TPReceiver::ScheduleDisplay(void){
    NS_LOG_FUNCTION(this);
    if(m_running){
        m_displayEvent = Simulator::Schedule(m_dispFreq, &TPReceiver::Display, this);
    }
}


void TPReceiver::Display(void){
    NS_LOG_FUNCTION(this);
    uint8_t dim = m_fieldSize * m_fieldSize; 
    NS_LOG_INFO("Dimension: " << dim);
    //read frame from inFile
    //char *frame = new char[m_fieldSize * m_fieldSize + 1];
    char *frame = new char[dim+1];
    //m_inFile.read(frame, m_fieldSize * m_fieldSize);
    m_inFile.read(frame, dim);
    //frame[m_fieldSize * m_fieldSize] = '\0';
    frame[dim] = '\0';
    NS_LOG_INFO("Next Frame is " << frame);
    //place player based on m_currPos
    frame[dim - m_fieldSize + m_currPos] = '2';
    
    //write to outFile
    m_outFile << frame;

    delete[] frame;
    if(!m_inFile.eof()){
        ScheduleDisplay();
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
            //m_outFile << reinterpret_cast<char*>(payload);
            
            if (payload[0] == '1'){
                m_currPos = (m_currPos + 1 > m_fieldSize - 1)? m_fieldSize - 1
                                                             : m_currPos + 1;
            }else if(payload[0] == '2'){
                m_currPos = (m_currPos - 1 < 0)? 0
                                               : m_currPos - 1;
            }

            delete[] payload;
        }
    }
}

void TPReceiver::StopApplication(){
    NS_LOG_FUNCTION(this);
    m_running = false;
    if(m_displayEvent.IsRunning()){
    
        Simulator::Cancel(m_displayEvent);
    }
    if(m_socket){
        m_socket->Close();
    }
    if(m_inFile.is_open()){
        m_inFile.close();
    }
    if(m_outFile.is_open()){
        m_outFile.close();
    }
}
    
};