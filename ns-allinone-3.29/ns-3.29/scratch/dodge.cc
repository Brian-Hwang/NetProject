
#include "ns3/game-user.h"
#include "ns3/game-user-helper.h"
#include "ns3/game-server.h"
#include "ns3/game-server-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

static void Rxcontent(std::string context, Ptr<const Packet> p)
{
    uint8_t *buf = new uint8_t[1];
    p->CopyData(buf, 1);
    Ptr<Packet> copyP = p->Copy();
    SeqTsHeader hdr;
    copyP->RemoveHeader(hdr);
    NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "\t" <<  Simulator::Now().GetSeconds() - hdr.GetTs().GetSeconds() );
    delete[] buf;
}

NS_LOG_COMPONENT_DEFINE("TestTeamProject");

int main(int argc, char *argv[])
{

    //LogComponentEnable("GameServer", LOG_LEVEL_FUNCTION);
    //LogComponentEnable("GameUser", LOG_LEVEL_ALL);
    LogComponentEnable("TestTeamProject", LOG_LEVEL_ALL);
    GlobalValue::Bind("SimulatorImplementationType",
                      StringValue("ns3::RealtimeSimulatorImpl"));
    
    std::string dr = "100Mbps";
    std::string delay = "1ms";
    std::string proto = "Udp";
    Time fps = Seconds(0.05);
    Time speed = Seconds(0.5);
    Time speedInc = Seconds(0.01);
    Time speedInt = Seconds(1.);
    bool io = false;

    CommandLine cmd;
    cmd.AddValue("Delay", "Link Delay", delay);
    cmd.AddValue("DataRate", "Data Rate", dr);
    cmd.AddValue("Protocol", "UDP or Wifi", proto);
    cmd.AddValue("FromInput", "Set true if instead of random frame generation, frames should be read from a file", io);
    cmd.AddValue("FrameRate", "After what time should a new frame be written to output", fps);
    cmd.AddValue("Speed", "Beginning Speed of the Game", speed);
    cmd.AddValue("SpeedInc", "In what intervals does speed increase?", speedInc);
    cmd.AddValue("SpeedInt", "After how much time is speed increased?", speedInt);

    cmd.Parse(argc, argv);

    NS_LOG_UNCOND(proto);
    NodeContainer nodes;
    nodes.Create(2);
    NetDeviceContainer devices;
    NetDeviceContainer clientStaDevices;
    NetDeviceContainer serverApDevice;

    if (!proto.compare("wifi"))
    {
        // Create PHY layer
        YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
        YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
        phy.SetChannel(channel.Create());

        // Create MAC layer
        WifiMacHelper mac;
        Ssid ssid = Ssid("Dodge");
        mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));

        // Create WLAN setting
        WifiHelper wifi;
        wifi.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);
        wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("HtMcs7"), "ControlMode", StringValue("HtMcs0"));

        // create NetDevices
        // NetDeviceContainer clientStaDevices;
        clientStaDevices = wifi.Install(phy, mac, nodes.Get(0));

        mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid), "BeaconInterval", TimeValue(MicroSeconds(102400)), "BeaconGeneration", BooleanValue(true));

        // NetDeviceContainer serverApDevice;
        serverApDevice = wifi.Install(phy, mac, nodes.Get(1));
    }
    else if (!proto.compare("Udp"))
    {

        PointToPointHelper pointToPoint;
        pointToPoint.SetDeviceAttribute("DataRate", StringValue(dr));
        pointToPoint.SetChannelAttribute("Delay", StringValue(delay));

        // create NetDevices
        devices = pointToPoint.Install(nodes);
        pointToPoint.EnablePcapAll("test");
    }

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaceServer, interfaceClient;
    if (!proto.compare("wifi"))
    {
        interfaceClient = address.Assign(clientStaDevices);
        interfaceServer = address.Assign(serverApDevice);

        MobilityHelper mobility;
        Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
        positionAlloc->Add(Vector(0.0, 0.0, 0.0));
        positionAlloc->Add(Vector(1.0, 0.0, 0.0));
        mobility.SetPositionAllocator(positionAlloc);

        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        mobility.Install(nodes.Get(1));
        mobility.Install(nodes.Get(0));
    }
    else if (!proto.compare("Udp"))
    {
        interfaceClient = address.Assign(devices.Get(0));
        interfaceServer = address.Assign(devices.Get(1));
    }


    uint16_t port = 8080;
    Address destination(InetSocketAddress(interfaceServer.GetAddress(0), port));

    // need to pass full path to sender
    GameUserHelper user(port, 10);
    user.SetAttribute("DataRate", DataRateValue(DataRate("20Mb/s")));
    ApplicationContainer userApp = user.Install(nodes.Get(0));

    userApp.Start(Seconds(1.0));
    userApp.Stop(Seconds(50.0));

    GameServerHelper server(Address(InetSocketAddress(interfaceClient.GetAddress(0), port)), port, "scratch/output.txt", 10);
    
    server.SetAttribute("FileIO", BooleanValue(io));
    server.SetAttribute("InFile", StringValue("scratch/frames1.txt"));
    server.SetAttribute("DisplayFreq", TimeValue(fps));
    server.SetAttribute("SpeedIncrease", TimeValue(speedInc));
    server.SetAttribute("SpeedIncreaseInterval", TimeValue(speedInt));
    server.SetAttribute("IntervalBrick", TimeValue(speed));
    
    ApplicationContainer serverApp = server.Install(nodes.Get(1));
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(Seconds(50.0));
    serverApp.Get(0)->TraceConnect("Rx", "Arrived", MakeCallback(&Rxcontent));

    Simulator::Stop(Seconds(50.0));
    Simulator::Run();
    Simulator::Destroy();
}
