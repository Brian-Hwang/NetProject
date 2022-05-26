#ifndef GAMESERVERHELPER_H_
#define GAMESERVERHELPER_H_

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/game-server.h"

namespace ns3
{
    class GameServerHelper
    {
    public:
        GameServerHelper(Address address, std::string outfile, uint8_t size);
        void SetAttribute(std::string name, const AttributeValue &value);
        ApplicationContainer Install(Ptr<Node> node) const;
        ApplicationContainer Install(std::string nodeName) const;
        ApplicationContainer Install(NodeContainer c) const;

    private:
        Ptr<Application> InstallPriv(Ptr<Node> node) const;
        ObjectFactory m_factory;
    };
};
#endif
