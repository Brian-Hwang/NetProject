#include "ns3/tp-sender-helper.h"
#include "ns3/tp-sender.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3{

TPSenderHelper::TPSenderHelper(Address address){
    m_factory.SetTypeId("ns3::TPSender");
    m_factory.Set("Address", AddressValue(address));
}

void TPSenderHelper::SetAttribute(std::string name, const AttributeValue &value){
    m_factory.Set(name, value);
}

ApplicationContainer TPSenderHelper::Install(Ptr<Node> node) const{
    return ApplicationContainer(InstallPriv(node));
}

Ptr<Application>
TPSenderHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<Application> ();
  node->AddApplication (app);

  return app;
}
};
