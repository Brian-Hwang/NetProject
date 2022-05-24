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
#include "ns3/tp-sender_STU.h"
#include "ns3/string.h"

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("TPSender_STU");

    NS_OBJECT_ENSURE_REGISTERED(TPSender_STU);

    TypeId TPSender_STU::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::TPSender_STU")
                                .SetParent<Application>()
                                .AddConstructor<TPSender_STU>()
                                .AddAttribute("Address", "The address",
                                              AddressValue(),
                                              MakeAddressAccessor(&TPSender_STU::m_address),
                                              MakeAddressChecker())
                                .AddAttribute("NPackets", "The total number of packets to send",
                                              UintegerValue(10000),
                                              MakeUintegerAccessor(&TPSender_STU::m_nPackets),
                                              MakeUintegerChecker<uint32_t>())
                                .AddAttribute("FileName", "The name of the file to be transmitted.",
                                              StringValue(),
                                              MakeStringAccessor(&TPSender_STU::m_filename),
                                              MakeStringChecker())
                                .AddAttribute("DataRate", "The data rate",
                                              DataRateValue(DataRate("500kb/s")),
                                              MakeDataRateAccessor(&TPSender_STU::m_dataRate),
                                              MakeDataRateChecker())
                                .AddTraceSource("Tx", "A new packet is created and is sent",
                                                MakeTraceSourceAccessor(&TPSender_STU::m_txTrace), "ns3::Packet::TracedCallback");
        return tid;
    }

    TPSender_STU::TPSender_STU()
        : m_socket(0),
          m_packetSize(1000),
          m_packetsSent(0),
          m_running(false),
          m_file(NULL)
    {
        NS_LOG_FUNCTION(this);
    }

    TPSender_STU::~TPSender_STU()
    {
        NS_LOG_FUNCTION(this);
    }

    void TPSender_STU::StartApplication(void)
    {
        NS_LOG_FUNCTION(this);

        // if there is no socket yet, create one
        if (!m_socket)
        {
            // this is udp for now [TODO] decide UDP or TCP
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            m_socket->Bind();
            m_socket->Connect(m_address);
        }

        if (!m_file)
        {
            m_file.open(m_filename);
            NS_LOG_INFO("Opening File...");
            if (m_file.is_open())
            {
                NS_LOG_INFO("Opened File!");
            }
        }

        // indicate that application is running now
        m_running = true;

        // Send first packet
        SendPacket();
    }

    void TPSender_STU::SendPacket(void)
    {
        NS_LOG_FUNCTION(this);

        char *content = new char[10];
        m_file.read(content, 1);
        // content[1] = '\0';

        NS_LOG_INFO("Read " << content << " from file");

        uint8_t *buf = new uint8_t[10];
        for (uint8_t i = 0; i < 10; i++)
        {
            buf[i] = (uint8_t)content[i - 1];
        }

        NS_LOG_INFO("Converted char to " << buf);

        Ptr<Packet> packet = Create<Packet>(buf, 2);

        m_txTrace(packet);

        // send packet over socket (this is fully abstracted, no need to change based on TCP/UDP)
        m_socket->Send(packet);

        // continue sending until all number of packets have been sent
        if (++m_packetsSent < m_nPackets && !m_file.eof())
        {
            ScheduleTx();
        }
        delete[] content;
        delete[] buf;
    }

    void TPSender_STU::ScheduleTx(void)
    {
        NS_LOG_FUNCTION(this);
        if (m_running)
        {
            // getting next transmission tiome based on transmission rate
            Time tNext(
                Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));

            // creating new send event with the scheduler using the calculated time and the function call being scheduled, here: SendPacket()
            m_sendEvent = Simulator::Schedule(tNext, &TPSender_STU::SendPacket, this);
        }
    }

    void TPSender_STU::StopApplication()
    {
        NS_LOG_FUNCTION(this);
        m_running = false;
        if (m_sendEvent.IsRunning())
        {

            Simulator::Cancel(m_sendEvent);
        }

        if (m_socket)
        {
            m_socket->Close();
        }
        if (m_file.is_open())
        {
            m_file.close();
        }
    }

};