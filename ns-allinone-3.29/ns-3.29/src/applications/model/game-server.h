#ifndef GAMESERVER_H_
#define GAMESERVER_H_

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
#include "ns3/nstime.h"
#include "ns3/traced-value.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace ns3
{

    // class Socket;
    // class Packet;

    class GameServer : public Application
    {
    public:
        static TypeId GetTypeId(void);
        GameServer();
        virtual ~GameServer();

    private:
        virtual void StartApplication(void);
        virtual void StopApplication(void);

        void HandleRead(Ptr<Socket> socket);
        void ScheduleDisplay(void);
        void ScheduleUpdate(void);
        void ScheduleSpeedIncrease(void);
        void UpdateBricks(void);
        void UpdateSpeed(void);
        void ScheduleTransmit(Time dt);
        void Display(void);
        char *NextFrame(uint16_t);
        void SendFrame(void);

        Address m_address;

        Ptr<Socket> m_socket;
        uint32_t m_totalRx;
        bool m_running;
        uint8_t m_fieldSize;
        bool m_fileIO;
        TracedValue<uint8_t> m_currPos;
        std::stringstream m_entireMapStream;
        std::string m_entireMap;
        std::string m_lastFrame;
        size_t m_send_start = 10;
        size_t m_display_start = 0;
        char *m_nextFrame;
        Time m_dispFreq;
        Time m_interval;
        Time m_speedIncrease;
        Time m_speedInterval;
        
        EventId m_displayEvent;
        EventId m_sendEvent;
        EventId m_speedEvent;
        std::string m_inFilename;
        std::string m_outFilename;
        std::ifstream m_inFile;
        std::ofstream m_outFile;

        std::string m_proto;

        bool m_generateBricks;
        bool m_gameOver;

        Address m_peerAddress;
        uint16_t m_peerPort;
        TracedCallback<Ptr<const Packet>> m_rxTrace;
        TracedCallback<Ptr<const Packet>> m_txTrace;
    };
};
#endif
