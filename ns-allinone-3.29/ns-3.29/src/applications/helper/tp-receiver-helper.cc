#include "ns3/tp-receiver-helper.h"
#include "ns3/tp-receiver.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3{

TPReceiverHelper::TPReceiverHelper(Address address){
    m_factory.SetTypeId("ns3::TPReceiver");
    m_factory.Set("Local", AddressValue(address));
}

void TPReceiverHelper::SetAttribute(std::string name, const AttributeValue &value){
    m_factory.Set(name, value);
}

ApplicationContainer TPReceiverHelper::Install(Ptr<Node> node) const{
    return ApplicationContainer(InstallPriv(node));
}

Ptr<Application>
TPReceiverHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<Application> ();
  node->AddApplication (app);

  return app;
}
};
