
#include "ns3/game-user.h"
#include "ns3/game-user-helper.h"
#include "ns3/game-server.h"
#include "ns3/game-server-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
using namespace ns3;

static void Rxcontent(std::string context, Ptr<const Packet> p){
    uint8_t *buf = new uint8_t [1];
    p->CopyData(buf, 1);
    NS_LOG_UNCOND("Received Packet! Contents: " << std::to_string(buf[0]));
    delete[] buf;
}
/*static void Txcontent(std::string context, Ptr<const Packet> p){
    uint8_t *buf = new uint8_t [1];
    p->CopyData(buf, 1);
    NS_LOG_UNCOND("Sent Packet! Contents: " << std::to_string(buf[0]));
    delete[] buf;
}*/

NS_LOG_COMPONENT_DEFINE("TestTeamProject");

int main(int argc, char *argv[]){

    LogComponentEnable("GameServer", LOG_LEVEL_ALL);
    LogComponentEnable("GameUser", LOG_LEVEL_ALL);
    LogComponentEnable("TestTeamProject", LOG_LEVEL_ALL);
/*    LogComponentEnable("UdpL4Protocol", LOG_LEVEL_ALL);
    LogComponentEnable("UdpSocket", LOG_LEVEL_FUNCTION);
    LogComponentEnable("Socket", LOG_LEVEL_FUNCTION);
    LogComponentEnable("UdpSocketImpl", LOG_LEVEL_LOGIC);
    LogComponentEnable("Ipv4Interface", LOG_LEVEL_LOGIC);
    LogComponentEnable("Ipv4L3Protocol", LOG_LEVEL_LOGIC);
    LogComponentEnable("TrafficControlLayer", LOG_LEVEL_FUNCTION);
    LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_FUNCTION);
    LogComponentEnable("PointToPointChannel", LOG_LEVEL_FUNCTION);
*/
      GlobalValue::Bind ("SimulatorImplementationType", 
                     StringValue ("ns3::RealtimeSimulatorImpl"));
    std::string dr = "100Mbps";
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

    //need to pass full path to sender
    GameUserHelper user (port,  10);
    //user.SetAttribute("NPackets", UintegerValue(1000000000000));
    user.SetAttribute("DataRate", DataRateValue(DataRate("20Mb/s")));
    ApplicationContainer userApp = user.Install(nodes.Get(0));

    userApp.Start(Seconds(1.0));
    //userApp.Stop(Seconds(40.0));

    GameServerHelper server(Address(InetSocketAddress(interfaces.GetAddress(0), port)), port, "/root/NetProject/ns-allinone-3.29/ns-3.29/scratch/output.txt", 10);
    server.SetAttribute("FileIO", BooleanValue(true));
    server.SetAttribute("InFile", StringValue("/root/NetProject/ns-allinone-3.29/ns-3.29/scratch/frames.txt"));
    server.SetAttribute("DisplayFreq", TimeValue(Seconds(0.05)));
    ApplicationContainer serverApp = server.Install(nodes.Get(1));
    serverApp.Start(Seconds(1.0));
    //serverApp.Stop(Seconds(40.0));
    serverApp.Get(0)->TraceConnect("Rx", "Arrived", MakeCallback(&Rxcontent));    
    //userApp.Get(0)->TraceConnect("Tx", "Sent", MakeCallback(&Txcontent));    

    Simulator::Stop(Seconds(55.0));
    Simulator::Run();
    Simulator::Destroy();
}
