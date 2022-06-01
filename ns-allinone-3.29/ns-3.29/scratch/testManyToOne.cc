
#include "ns3/game-user.h"
#include "ns3/game-user-helper.h"
#include "ns3/game-server.h"
#include "ns3/game-server-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/bridge-module.h"
using namespace ns3;

static void Rxcontent(std::string context, Ptr<const Packet> p){
    uint8_t *buf = new uint8_t [1];
    p->CopyData(buf, 1);
    Ptr<Packet> copyP = p->Copy();
    SeqTsHeader hdr;
    copyP->RemoveHeader(hdr);
    NS_LOG_UNCOND(context << "\t" << Simulator::Now().GetSeconds() << "\t" <<  Simulator::Now().GetSeconds() - hdr.GetTs().GetSeconds() );
    delete[] buf;
}

NS_LOG_COMPONENT_DEFINE("Test2TeamProject");

int main(int argc, char *argv[]){

    //LogComponentEnable("GameServer", LOG_LEVEL_ALL);
    //LogComponentEnable("GameUser", LOG_LEVEL_ALL);
    LogComponentEnable("Test2TeamProject", LOG_LEVEL_ALL);
    
    GlobalValue::Bind ("SimulatorImplementationType", 
                     StringValue ("ns3::RealtimeSimulatorImpl"));
    
    std::string dr = "100Mbps";
    std::string delay = "1ms";
    int numNodes = 20;
    std::string incast = "Client";

    CommandLine cmd;
    cmd.AddValue("Incast", "Decide whether to have incast traffic on server or client.", incast);
    cmd.AddValue("Nodes", "The number of nodes in the simulation.", numNodes);
    cmd.Parse(argc, argv);

    int mode;

    //mode 0 for incast traffic on server
    //mode 1 for incast traffic on client
    if(!incast.compare("Server")){
        NS_LOG_INFO("Incast on Server");
        mode = 1;
    }else{
        NS_LOG_INFO("Incast on Client");
        mode = 0;
    }
    NodeContainer terminals;
    terminals.Create(numNodes);

    NodeContainer csmaSwitch;
    csmaSwitch.Create(1);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(5000000));
    csma.SetChannelAttribute("Delay", TimeValue(MicroSeconds(10)));
    csma.EnablePcapAll("secondTest");
    NetDeviceContainer terminalDevices;
    NetDeviceContainer switchDevices;

    for(int i = 0; i < numNodes; i++){
        NetDeviceContainer link = csma.Install(NodeContainer(terminals.Get(i), csmaSwitch));
        terminalDevices.Add(link.Get(0));
        switchDevices.Add(link.Get(1));
    }

    Ptr<Node> switchNode = csmaSwitch.Get(0);
    BridgeHelper bridge;
    bridge.Install(switchNode, switchDevices);

    InternetStackHelper internet;
    internet.Install(terminals);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(terminalDevices);


    uint16_t *ports = (uint16_t*)malloc(sizeof(uint16_t) * (numNodes - 1));
    ports[0] = 8081;
    for(int i = 1; i < numNodes - 1; i++)
        ports[i] = ports[i-1] + 1;

    NS_LOG_UNCOND(std::to_string(ports[0]) + " " + std::to_string(ports[1]) + " " + std::to_string(ports[2]));


    ApplicationContainer userApps;
    ApplicationContainer serverApps;
    GameUserHelper user (ports[0],  10);
    user.SetAttribute("DataRate", DataRateValue(DataRate("20Mb/s")));
    
    GameServerHelper server(Address(InetSocketAddress(interfaces.GetAddress(0), ports[0])), ports[0], "./scratch/output0.txt", 10);
//    server.SetAttribute("FileIO", BooleanValue(true));
    server.SetAttribute("InFile", StringValue("./scratch/frames.txt"));
    server.SetAttribute("DisplayFreq", TimeValue(Seconds(0.05)));
    server.SetAttribute("GameOver", BooleanValue(true));
    
    for(int i = 0; i < numNodes -1; i++){
        //Create User Application
        //Address destination(InetSocketAddress(interfaces.GetAddress(i), ports[i]));
        user.SetAttribute("Port", UintegerValue(ports[i]));

        //Create Server Application
        //server.SetAttribute("Local", AddressValue(destination));
        server.SetAttribute("OutFile",  StringValue("./scratch/output" + std::to_string(i) + ".txt"));
        server.SetAttribute("RemotePort", UintegerValue(ports[i]));
        //server.SetAttribute("InFile",  StringValue("/root/NetProject/ns-allinone-3.29/ns-3.29/scratch/frames" + std::to_string(i) + ".txt"));
        
        if(mode == 0){
            server.SetAttribute("RemoteAddress", AddressValue(InetSocketAddress(interfaces.GetAddress(numNodes-1), ports[i])));
            
            userApps.Add(user.Install(terminals.Get(numNodes - 1)));
            serverApps.Add(server.Install(terminals.Get(i)));
        
        }else{
            server.SetAttribute("RemoteAddress", AddressValue(InetSocketAddress(interfaces.GetAddress(i), ports[i])));
            userApps.Add(user.Install(terminals.Get(i)));
            serverApps.Add(server.Install(terminals.Get(numNodes-1)));
        }
        
        
        serverApps.Get(i)->TraceConnect("Rx", (std::to_string(i)), MakeCallback(&Rxcontent));    
    }

    //need to pass full path to sender
    //GameUserHelper user (port,  10);
    //user.SetAttribute("NPackets", UintegerValue(1000000000000));
    //ApplicationContainer userApp = user.Install(nodes.Get(0));

    userApps.Start(Seconds(1.0));
    //userApp.Stop(Seconds(40.0));

    //GameServerHelper server(Address(InetSocketAddress(interfaces.GetAddress(0), port)), port, "/root/NetProject/ns-allinone-3.29/ns-3.29/scratch/output.txt", 10);
    //ApplicationContainer serverApp = server.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    //serverApp.Stop(Seconds(40.0));
    //serverApp.Get(0)->TraceConnect("Rx", "Arrived", MakeCallback(&Rxcontent));    
    //userApp.Get(0)->TraceConnect("Tx", "Sent", MakeCallback(&Txcontent));    

    free(ports);

    Simulator::Stop(Seconds(55.0));
    Simulator::Run();
    Simulator::Destroy();
}
