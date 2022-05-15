#ifndef TPRECVHELPER_H_
#define TPRECVHELPER_H_

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/tp-receiver.h"

namespace ns3{
class TPReceiverHelper{
    public:
        TPReceiverHelper(Address address, std::string infile, std::string outfile);
        void SetAttribute(std::string name, const AttributeValue &value);
        ApplicationContainer Install(Ptr<Node> node) const;
        ApplicationContainer Install(std::string nodeName) const;
        ApplicationContainer Install(NodeContainer c) const;

    private:
        Ptr<Application> InstallPriv(Ptr<Node>node) const;
        ObjectFactory m_factory;
};
};
#endif
