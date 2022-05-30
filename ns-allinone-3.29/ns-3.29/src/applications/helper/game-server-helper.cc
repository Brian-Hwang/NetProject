#include "ns3/game-server.h"
#include "ns3/game-server-helper.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3
{

    GameServerHelper::GameServerHelper(Address address, uint16_t port, std::string outfile, uint8_t size)
    {
        m_factory.SetTypeId("ns3::GameServer");
        m_factory.Set("RemoteAddress", AddressValue(address));
        m_factory.Set("RemotePort", UintegerValue(port));
        m_factory.Set("OutFile", StringValue(outfile));
        m_factory.Set("GameSize", UintegerValue(size));
    }

    void GameServerHelper::SetAttribute(std::string name, const AttributeValue &value)
    {
        m_factory.Set(name, value);
    }

    ApplicationContainer GameServerHelper::Install(Ptr<Node> node) const
    {
        return ApplicationContainer(InstallPriv(node));
    }

    Ptr<Application>
    GameServerHelper::InstallPriv(Ptr<Node> node) const
    {
        Ptr<Application> app = m_factory.Create<Application>();
        node->AddApplication(app);

        return app;
    }
};
