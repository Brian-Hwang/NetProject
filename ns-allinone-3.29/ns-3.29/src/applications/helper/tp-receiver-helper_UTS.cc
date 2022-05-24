#include "ns3/tp-receiver-helper_UTS.h"
#include "ns3/tp-receiver.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3
{

    TPReceiverHelper_UTS::TPReceiverHelper_UTS(Address address, std::string outfile, uint8_t size)
    {
        m_factory.SetTypeId("ns3::TPReceiver");
        m_factory.Set("Local", AddressValue(address));
        m_factory.Set("OutFile", StringValue(outfile));
        m_factory.Set("GameSize", UintegerValue(size));
    }

    void TPReceiverHelper_UTS::SetAttribute(std::string name, const AttributeValue &value)
    {
        m_factory.Set(name, value);
    }

    ApplicationContainer TPReceiverHelper_UTS::Install(Ptr<Node> node) const
    {
        return ApplicationContainer(InstallPriv(node));
    }

    Ptr<Application>
    TPReceiverHelper_UTS::InstallPriv(Ptr<Node> node) const
    {
        Ptr<Application> app = m_factory.Create<Application>();
        node->AddApplication(app);

        return app;
    }
};
