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
#include "ns3/tp-receiver_STU.h"

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("TPReceiverSTU");

    NS_OBJECT_ENSURE_REGISTERED(TPReceiverSTU);

    TypeId TPReceiverSTU::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::TPReceiverSTU")
                                .SetParent<Application>()
                                .AddConstructor<TPReceiverSTU>()
                                .AddAttribute("Local", "The Address on which to Bind the RX socket.",
                                              AddressValue(),
                                              MakeAddressAccessor(&TPReceiverSTU::m_address),
                                              MakeAddressChecker())
                                .AddAttribute("InFile", "The name of the input file to get data for calculation from.",
                                              StringValue(),
                                              MakeStringAccessor(&TPReceiverSTU::m_inFilename),
                                              MakeStringChecker())
                                .AddAttribute("OutFile", "The name of the output file to write received data to.",
                                              StringValue(),
                                              MakeStringAccessor(&TPReceiverSTU::m_outFilename),
                                              MakeStringChecker())
                                .AddAttribute("GameSize", "Size of the displayed field in game.",
                                              UintegerValue(),
                                              MakeUintegerAccessor(&TPReceiverSTU::m_fieldSize),
                                              MakeUintegerChecker<uint8_t>())
                                .AddAttribute("FileIO", "Determines whether the application runs its own frame generation, or if it gets frames over File IO",
                                              BooleanValue(false),
                                              MakeBooleanAccessor(&TPReceiverSTU::m_fileIO),
                                              MakeBooleanChecker())
                                .AddTraceSource("Rx", "A packet has been received",
                                                MakeTraceSourceAccessor(&TPReceiverSTU::m_rxTrace), "ns3::Packet::TracedCallback")
                                .AddTraceSource("Tx", "A packet has been sent",
                                                MakeTraceSourceAccessor(&TPReceiverSTU::m_txTrace), "ns3::Packet::TracedCallback");
        return tid;
    }

    TPReceiverSTU::TPReceiverSTU()
        : m_socket(0),
          m_totalRx(0),
          m_running(false),
          m_inFile(NULL),
          m_outFile(NULL)
    {
        NS_LOG_FUNCTION(this);
    }

    TPReceiverSTU::~TPReceiverSTU()
    {
        NS_LOG_FUNCTION(this);
    }

    void TPReceiverSTU::StartApplication(void)
    {
        NS_LOG_FUNCTION(this);

        if (!m_socket)
        {
            // this is udp for now [TODO] decide UDP or TCP
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            m_socket->Bind(m_address);
            m_socket->Listen();
            m_socket->ShutdownSend();
            m_socket->SetRecvCallback(MakeCallback(&TPReceiverSTU::HandleRead, this));
        }
        m_obstacle = (uint8_t *)malloc(sizeof(uint8_t) * m_fieldSize * m_fieldSize);

        if (!m_inFile && m_fileIO)
        {
            m_inFile.open(m_inFilename);

            NS_ASSERT_MSG(m_inFile, "Unable to open Input File.");
        }

        if (!m_outFile)
        {
            m_outFile.open(m_outFilename);
            NS_ASSERT_MSG(m_outFile, "Unable to open Output File.");
        }

        m_running = true;
        MakeAIDecision();
        Display();
    }

    void TPReceiverSTU::MakeAIDecision(void)
    {
        NS_LOG_FUNCTION(this);
        uint16_t dim = m_fieldSize * m_fieldSize;

        char *frame = NextFrame(dim);

        NS_LOG_DEBUG("Next Frame is " << frame);

        DoAI(); // do AI HERE
        // write to outFile
        m_outFile << frame;
        delete[] frame;
    }
    void TPReceiverSTU::DoAI() {}

    char *TPReceiverSTU::NextFrame(uint16_t dim)
    {
        NS_LOG_FUNCTION(this);

        NS_LOG_DEBUG("Frame Dimension: " << dim);
        char *frame = new char[dim + 1];

        if (m_fileIO)
        {

            // read frame from input file
            m_inFile.read(frame, dim);
            frame[dim] = '\0';
        }
        else
        {
            /* Insert game's frame generation logic here*/
        }

        return frame;
    }

    void TPReceiverSTU::HandleRead(Ptr<Socket> socket)
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
                NS_LOG_DEBUG("Received " << payload << " from sender.");

                for (uint8_t i = 0; i < packet->GetSize(); i++)
                {
                    if (payload[i] == '1')
                    {
                        m_obstacle[i] = 1;
                    }
                    else
                        m_obstacle[i] = 0;
                }
                delete[] payload;
            }
        }
    }

    void TPReceiverSTU::StopApplication()
    {
        NS_LOG_FUNCTION(this);
        m_running = false;
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
