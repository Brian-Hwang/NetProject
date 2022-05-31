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
#include "ns3/game-server.h"
#include <algorithm>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("GameServer");

    NS_OBJECT_ENSURE_REGISTERED(GameServer);

    TypeId GameServer::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::GameServer")
                                .SetParent<Application>()
                                .AddConstructor<GameServer>()
                                .AddAttribute("Local", "The Address on which to Bind the RX socket.",
                                              AddressValue(),
                                              MakeAddressAccessor(&GameServer::m_address),
                                              MakeAddressChecker())
                                .AddAttribute("RemoteAddress", "The destination Address of the outbound packet.",
                                              AddressValue(),
                                              MakeAddressAccessor(&GameServer::m_peerAddress),
                                              MakeAddressChecker())
                                .AddAttribute("RemotePort", "The destination Port of the outbound packets.",
                                              UintegerValue(0),
                                              MakeUintegerAccessor(&GameServer::m_peerPort),
                                              MakeUintegerChecker<uint16_t>())
                                .AddAttribute("IntervalBrick", "The time interval to update the bricks",
                                              TimeValue (Seconds(0.5)),
                                              MakeTimeAccessor (&GameServer::m_interval),
                                              MakeTimeChecker())
                                .AddAttribute("InFile", "The name of the input file to get data for calculation from.",
                                              StringValue(),
                                              MakeStringAccessor(&GameServer::m_inFilename),
                                              MakeStringChecker())
                                .AddAttribute("OutFile", "The name of the output file to write received data to.",
                                              StringValue(),
                                              MakeStringAccessor(&GameServer::m_outFilename),
                                              MakeStringChecker())
                                .AddAttribute("GameSize", "Size of the displayed field in game.",
                                              UintegerValue(),
                                              MakeUintegerAccessor(&GameServer::m_fieldSize),
                                              MakeUintegerChecker<uint8_t>())
                                .AddAttribute("DisplayFreq", "Time between displaying different frames. (FPS maybe?)",
                                              TimeValue(Seconds(0.1)),
                                              MakeTimeAccessor(&GameServer::m_dispFreq),
                                              MakeTimeChecker())
                                .AddAttribute("FileIO", "Determines whether the application runs its own frame generation, or if it gets frames over File IO",
                                              BooleanValue(false),
                                              MakeBooleanAccessor(&GameServer::m_fileIO),
                                              MakeBooleanChecker())
                                .AddTraceSource("Rx", "A packet has been received",
                                                MakeTraceSourceAccessor(&GameServer::m_rxTrace), "ns3::Packet::TracedCallback")
                                .AddTraceSource("Tx", "A packet has been sent",
                                                MakeTraceSourceAccessor(&GameServer::m_txTrace), "ns3::Packet::TracedCallback");
        return tid;
    }

    GameServer::GameServer()
        : m_socket(0),
          m_totalRx(0),
          m_running(false),
          m_currPos(0),
          m_nextFrame(NULL),
          m_inFile(NULL),
          m_outFile(NULL)
    {
        m_lastFrame = "";
        m_sendEvent = EventId();
        NS_LOG_FUNCTION(this);
    }

    GameServer::~GameServer()
    {
        NS_LOG_FUNCTION(this);

        delete[] m_nextFrame;
    }

    void GameServer::StartApplication(void)
    {
        NS_LOG_FUNCTION(this);

        if (!m_socket)
        {
            // this is udp for now [TODO] decide UDP or TCP
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            m_socket->Bind(m_address);
            m_socket->Connect(m_peerAddress);
            //m_socket->Listen();
            //m_socket->ShutdownSend();
        }

        m_currPos = (int)(m_fieldSize / 2);
        //m_nextFrame = new char[m_fieldSize * m_fieldSize];

        if (!m_inFile && m_fileIO)
        {
            m_inFile.open(m_inFilename);

            NS_ASSERT_MSG(m_inFile, "Unable to open Input File " << m_inFilename<<". ");

            m_entireMapStream << m_inFile.rdbuf();
            m_entireMapStream >> m_entireMap;

            m_inFile.close();
        }

        if (!m_outFile)
        {
            m_outFile.open(m_outFilename);
            NS_ASSERT_MSG(m_outFile, "Unable to open Output File.");
        }

        m_running = true;

        //send first frame
        //SendFrame();

        //Schedule Display first frame
        //ScheduleDisplay();
        //Display(true);
        m_socket->SetRecvCallback(MakeCallback(&GameServer::HandleRead, this));
        //ScheduleTransmit(Seconds(0.));
        UpdateBricks();
        ScheduleTransmit(Seconds(0.));
        ScheduleDisplay();
    }

    void GameServer::SendFrame(void){
        NS_LOG_FUNCTION(this);

        char buf[31];
        size_t nread = m_lastFrame.copy(buf, 30, 60);
        while (nread < 30) {
            buf[nread++] = '0'; 
        }
        buf[nread] = '\0';
        m_send_start += 10;

        //Create a packet sending the frame here
        uint8_t* p = reinterpret_cast<uint8_t*> (&buf);
        Ptr<Packet> packet = Create<Packet>(p, 31);        
        //Send the actual packet here
        m_txTrace(packet);
        m_socket->Send(packet);

        //Display(true);
    }

    void GameServer::ScheduleDisplay(void)
    {
        NS_LOG_FUNCTION(this);
        if (m_running)
        {
            m_displayEvent = Simulator::Schedule(m_dispFreq, &GameServer::Display, this);
        }
    }
    void GameServer::ScheduleUpdate(void)
    {
        NS_LOG_FUNCTION(this);
        if (m_running)
        {
            m_displayEvent = Simulator::Schedule(m_interval, &GameServer::UpdateBricks, this);
        }
    }
    

    void 
    GameServer::ScheduleTransmit (Time dt)
    {
      NS_LOG_FUNCTION (this << dt);
      if (m_running)
          m_sendEvent = Simulator::Schedule (dt, &GameServer::SendFrame, this);
    }


    void GameServer::Display(void)
    {
        NS_LOG_FUNCTION(this);

        uint16_t dim = m_fieldSize * m_fieldSize;

        //NS_LOG_DEBUG("Next Frame is " << m_display_start << "|" << m_entireMap.size() << std::endl);

        std::string display_frame {};
        /*if (falldown) {
            char buf[101];
            size_t nread = m_entireMap.copy(buf, 100, m_display_start);
            buf[nread] = '\0';
            m_lastFrame = buf;
            if (nread < 100) {
                std::string fill_str (100-nread, '0');
                m_lastFrame += fill_str;
            }
            display_frame = m_lastFrame;
            m_display_start += 100;
        }
        else {
            display_frame = m_lastFrame;
            display_frame[dim - m_fieldSize + 9] = '9';
        }*/
        display_frame = m_lastFrame;
        //std::cout << "DISPLAY FRAME: " << display_frame.substr(90, 99) << std::endl;
        
        // write to outFile
        //std::reverse(display_frame.begin(), display_frame.end());
        display_frame[dim - m_fieldSize + static_cast<int>(m_currPos)] = '2';
        m_outFile << display_frame.c_str();

        if (m_display_start + 200 < m_entireMap.size())
        {
            ScheduleTransmit(Seconds(0.));
            ScheduleDisplay();
        }
        else {
            //std::cout << "WTF " << m_entireMap.size() << std::endl;
            StopApplication();
        }
        //std::cout << "hummm\n";
    }

    void GameServer::UpdateBricks(void){
        NS_LOG_FUNCTION(this);
        //Call this function every X seconds
        //Update the value m_lastFrame (bad name)
        char buf[101];
        size_t nread = m_entireMap.copy(buf, 100, m_display_start);
        buf[nread] = '\0';
        m_lastFrame = buf;
        if (nread < 100) {
            std::string fill_str (100-nread, '0');
            m_lastFrame += fill_str;
        }
        m_display_start += 100;

        ScheduleUpdate();
    }

    void GameServer::HandleRead(Ptr<Socket> socket)
    {
        NS_LOG_FUNCTION(this);
        Ptr<Packet> packet;
        Address from;
        while ((packet = m_socket->RecvFrom(from)))
        {
            if (packet->GetSize() > 0)
            {
                m_totalRx++;
                m_rxTrace(packet);
                uint8_t *payload = new uint8_t[packet->GetSize()];
                packet->CopyData(payload, packet->GetSize());
                //NS_LOG_DEBUG("Received " << static_cast<int>(payload[0]) << " from user.");
                //std::cout << "Received " << static_cast<int>(payload[0]) << " from user." << std::endl;

                //m_currPos = static_cast<int>(payload[0]);

                if (static_cast<int>(payload[0]) == 1)
                {
                    m_currPos = (m_currPos + 1 > m_fieldSize - 1) ? m_fieldSize - 1
                                                                  : m_currPos + 1;
                }
                else if (static_cast<int>(payload[0]) == 2)
                {
                    m_currPos = (m_currPos - 1 < 0) ? 0
                                                    : m_currPos - 1;
                }
                //std::cout << "im handleread, current pos is: " << static_cast<int>(m_currPos) << std::endl;

                delete[] payload;
            }
        }
        //Display(false); 
    }

    void GameServer::StopApplication()
    {
        NS_LOG_FUNCTION(this);
        m_running = false;
        if (m_displayEvent.IsRunning())
        {
            Simulator::Cancel (m_sendEvent);
            Simulator::Cancel(m_displayEvent);
        }
        if (m_socket)
        {
            m_socket->Close();
        }
        if (m_inFile.is_open())
        {
            m_inFile.close();
        }
        if (m_outFile.is_open())
        {
            m_outFile.close();
        }
    }

};
