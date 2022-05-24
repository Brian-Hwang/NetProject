#ifndef TPRECVHELPER_H_
#define TPRECVHELPER_H_

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/tp-receiver_UTS.h"

namespace ns3
{
    class TPReceiverHelper_UTS
    {
    public:
        TPReceiverHelper_UTS(Address address, std::string outfile, uint8_t size);
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
