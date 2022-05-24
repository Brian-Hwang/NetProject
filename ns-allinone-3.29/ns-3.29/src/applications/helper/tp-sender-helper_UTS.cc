#include "ns3/tp-sender-helper.h"
#include "ns3/tp-sender.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3
{

    TPSenderHelper_UTS::TPSenderHelper_UTS(Address address, std::string filename)
    {
        m_factory.SetTypeId("ns3::TPSender");
        m_factory.Set("Address", AddressValue(address));
        m_factory.Set("FileName", StringValue(filename));
    }

    void TPSenderHelper_UTS::SetAttribute(std::string name, const AttributeValue &value)
    {
        m_factory.Set(name, value);
    }

    ApplicationContainer TPSenderHelper_UTS::Install(Ptr<Node> node) const
    {
        return ApplicationContainer(InstallPriv(node));
    }

    Ptr<Application>
    TPSenderHelper_UTS::InstallPriv(Ptr<Node> node) const
    {
        Ptr<Application> app = m_factory.Create<Application>();
        node->AddApplication(app);

        return app;
    }
};
