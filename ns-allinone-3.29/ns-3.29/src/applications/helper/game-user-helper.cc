#include "ns3/game-user.h"
#include "ns3/game-user-helper.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3
{

    GameUserHelper::GameUserHelper(Address address)
    {
        m_factory.SetTypeId("ns3::GameUser");
        m_factory.Set("Address", AddressValue(address));
    }

    void GameUserHelper::SetAttribute(std::string name, const AttributeValue &value)
    {
        m_factory.Set(name, value);
    }

    ApplicationContainer GameUserHelper::Install(Ptr<Node> node) const
    {
        return ApplicationContainer(InstallPriv(node));
    }

    Ptr<Application>
    GameUserHelper::InstallPriv(Ptr<Node> node) const
    {
        Ptr<Application> app = m_factory.Create<Application>();
        node->AddApplication(app);

        return app;
    }
};
