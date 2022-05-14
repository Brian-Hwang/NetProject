#include "ns3/new-app-helper.h"
#include "ns3/new-app.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3{

NewAppHelper::NewAppHelper(bool mode, Address address){
    m_factory.SetTypeId("ns3::NewApp");
    m_factory.Set("Mode", BooleanValue(mode));
    m_factory.Set("Address", AddressValue(address));
}

void NewAppHelper::SetAttribute(std::string name, const AttributeValue &value){
    m_factory.Set(name, value);
}

ApplicationContainer NewAppHelper::Install(Ptr<Node> node) const{
    return ApplicationContainer(InstallPriv(node));
}

Ptr<Application>
NewAppHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<Application> ();
  node->AddApplication (app);

  return app;
}
};
