
#include "ns3/tp-sender.h"
#include "ns3/tp-sender-helper.h"
#include "ns3/tp-receiver.h"
#include "ns3/tp-receiver-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TestTeamProject");

int main(int argc, char *argv[]){

    LogComponentEnable("TPReceiver", LOG_LEVEL_ALL);
    LogComponentEnable("TPSender", LOG_LEVEL_ALL);

    std::string dr = "1Mbps";
    std::string delay = "1us";

    NS_LOG_UNCOND("Test");
    NodeContainer nodes;
    nodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(dr));
    pointToPoint.SetChannelAttribute("Delay", StringValue(delay));

    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);
    pointToPoint.EnablePcapAll("test");

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    uint16_t port = 8080;
    Address destination(InetSocketAddress(interfaces.GetAddress(1), port));

    TPSenderHelper sender (destination);
    sender.SetAttribute("NPackets", UintegerValue(1000));
    sender.SetAttribute("DataRate", DataRateValue(DataRate("2Mb/s")));
    ApplicationContainer senderApp = sender.Install(nodes.Get(0));

    senderApp.Start(Seconds(1.0));
    senderApp.Stop(Seconds(5.0));

    TPReceiverHelper receiver(Address(InetSocketAddress(interfaces.GetAddress(1), port)));
    ApplicationContainer receiverApp = receiver.Install(nodes.Get(1));
    receiverApp.Start(Seconds(0.5));
    receiverApp.Stop(Seconds(7.0));
    

    Simulator::Run();
    Simulator::Stop(Seconds(8.0));
    Simulator::Destroy();
}
