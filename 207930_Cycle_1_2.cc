/*
 * Author: mahendrasinghbora
 * Date: 2022-04-11 12:21:44
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h" // For NetAnim

// Network Topology Used
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

/* ----------------------------------------------------- 
 * Notes
 * -----------------------------------------------------
 * P2P: Peer to Peer
 * CSMA: Carrier Sense Multiple Access (used for LAN)
 * -----------------------------------------------------
 * 4 CSMA nodes : 1 P2P + 3 CSMA
 * number_of_CSMA_nodes = 3
 * -----------------------------------------------------
 * [1] n0: p2pNodes.Get(0)
 * [2] n1: p2pNodes.Get(1) or csma.Get(0)
 * [3] csmaNodes.Get(3) -> n4 is server here
 * [4] n0: client 
 * [5] Port used: 5000
 * -----------------------------------------------------
 */

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("Cycle - 1 (Experiment - 2)");

int main(int argc, char *argv[]) {
    bool verbose = true;
    int number_of_CSMA_nodes = 3;

    cout << string(80, '=') << endl;
    cout << "Cycle - 1 (Experiment - 2)" << endl;
    cout << string(80, '=') << endl;

    CommandLine cmd(__FILE__);
    cmd.AddValue("number_of_CSMA_nodes", "Number of \"extra\" CSMA nodes/devices", number_of_CSMA_nodes);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);

    cmd.Parse(argc, argv);

    if (verbose) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    number_of_CSMA_nodes = number_of_CSMA_nodes == 0 ? 1 : number_of_CSMA_nodes;

    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes.Get(1));
    csmaNodes.Create(number_of_CSMA_nodes);

    PointToPointHelper point_to_point;
    point_to_point.SetDeviceAttribute("DataRate", StringValue("8Mbps"));
    point_to_point.SetChannelAttribute("Delay", StringValue("5ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = point_to_point.Install(p2pNodes);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("50Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices;
    // Install InternetStack on csmaNodes
    csmaDevices = csma.Install(csmaNodes);

    // No need to install InternetStack on p2pNodes.Get(1) i.e. n1 (already installed on csmaNodes)
    // p2pNodes.Get(1) = csmaNodes.Get(0)
    InternetStackHelper stack;
    stack.Install(p2pNodes.Get(0));
    stack.Install(csmaNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    UdpEchoServerHelper echoServer(5000);

    // number_of_CSMA_nodes = 3
    // n0: client
    // last node(n4): server
    // Set up the last node as server
    ApplicationContainer serverApps = echoServer.Install(csmaNodes.Get(number_of_CSMA_nodes));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Set up echo client for node n0 (using the interface address of the server)
    // csmaInterfaces.GetAddress (number_of_CSMA_nodes): interface address of the server
    UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(number_of_CSMA_nodes), 5000);
    echoClient.SetAttribute("MaxPackets", UintegerValue(3));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(2048));

    // n0: client
    ApplicationContainer clientApps = echoClient.Install(p2pNodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // Populate the routing tables for all the nodes
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Packet tracing using Wireshark
    // generate .pcap file for tracing packets of n0 and n1 (P2P nodes) and the server n4
    point_to_point.EnablePcapAll("207930_Cycle_1_2_wireshark_p2p");
    csma.EnablePcap("207930_Cycle_1_2_wireshark_csma", csmaDevices.Get(3), true);

    // Animation using NetAnim
    AnimationInterface net_anim("207930_Cycle_1_2_net_anim.xml");
    net_anim.SetConstantPosition(p2pNodes.Get(0), 5.0, 5.0);
    net_anim.SetConstantPosition(csmaNodes.Get(0), 20.0, 20.0);
    net_anim.SetConstantPosition(csmaNodes.Get(1), 35.0, 35.0);
    net_anim.SetConstantPosition(csmaNodes.Get(2), 50.0, 50.0);
    net_anim.SetConstantPosition(csmaNodes.Get(3), 65.0, 65.0);

    // ASCII tracing for throughput using TraceMetrics (throughput as well)
    AsciiTraceHelper ascii_trace_helper;
    point_to_point.EnableAsciiAll(ascii_trace_helper.CreateFileStream("207930_Cycle_1_2_trace_p2p.tr"));
    csma.EnableAsciiAll(ascii_trace_helper.CreateFileStream("207930_Cycle_1_2_trace_csma.tr"));

    Simulator::Run();

    cout << string(80, '=') << endl;

    Simulator::Destroy();
    return 0;
}
