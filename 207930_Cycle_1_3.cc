/*
 * Author: mahendrasinghbora
 * Date: 2022-04-12 11:01:12
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"  // For NetAnim

// Network Topology Used
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4  <- Server
//         Client    point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("Cycle - 1 (Experiment - 3)");

int main(int argc, char *argv[]) {
    cout << string(80, '=') << endl;
    cout << "Cycle - 1 (Experiment - 3)" << endl;
    cout << string(80, '=') << endl;
    bool verbose = true;
    int number_of_CSMA_nodes = 3;
    int nWifi = 3;
    bool tracing = true;

    CommandLine cmd(__FILE__);
    cmd.AddValue("number_of_CSMA_nodes", "Number of \"extra\" CSMA nodes/devices", number_of_CSMA_nodes);
    cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);

    cmd.Parse(argc, argv);

    // The underlying restriction of 18 is due to the grid position
    // allocator's configuration; the grid layout will exceed the
    // bounding box if more than 18 nodes are provided.
    if (nWifi > 18) {
        cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << endl;
        return 1;
    }

    if (verbose) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    NodeContainer p2p_nodes;
    p2p_nodes.Create(2);

    PointToPointHelper point_to_point;
    point_to_point.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    point_to_point.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2p_devices;
    p2p_devices = point_to_point.Install(p2p_nodes);

    NodeContainer csma_nodes;
    csma_nodes.Add(p2p_nodes.Get(1));
    csma_nodes.Create(number_of_CSMA_nodes);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csma_devices;
    csma_devices = csma.Install(csma_nodes);

    // Wifi network setup: node n0 will be the AP (access point) for the wifi network
    // STA: Station Mode
    NodeContainer wifi_station_mode_nodes;
    wifi_station_mode_nodes.Create(nWifi);
    NodeContainer wifi_access_point_node = p2p_nodes.Get(0);

    // YANS: Yet Another Network Simulator
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;

    // Set up the communication path
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    // ActiveProbing: When true, checks if there are any wifi signals available in the nearby area
    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid),
                "ActiveProbing", BooleanValue(false));

    NetDeviceContainer sta_devices;
    sta_devices = wifi.Install(phy, mac, wifi_station_mode_nodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid));

    NetDeviceContainer ap_devices;
    ap_devices = wifi.Install(phy, mac, wifi_access_point_node);

    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(5.0),
                                  "DeltaY", DoubleValue(10.0),
                                  "GridWidth", UintegerValue(3),
                                  "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)));
    mobility.Install(wifi_station_mode_nodes);

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifi_access_point_node);

    //  No need to install Internet Stack on p2p node as there are 2 p2p nodes
    // n0 is covered under wifi_access_point_node
    // n1 is covered under csma_nodes

    InternetStackHelper stack;
    stack.Install(csma_nodes);
    stack.Install(wifi_access_point_node);
    stack.Install(wifi_station_mode_nodes);

    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2p_interfaces;
    p2p_interfaces = address.Assign(p2p_devices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csma_interfaces;
    csma_interfaces = address.Assign(csma_devices);

    address.SetBase("10.1.3.0", "255.255.255.0");
    address.Assign(sta_devices);
    address.Assign(ap_devices);

    // Set up the last node i.e. nth node as server
    UdpEchoServerHelper echoServer(3019);
    ApplicationContainer serverApps = echoServer.Install(csma_nodes.Get(number_of_CSMA_nodes));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Set up echo client for node n0 (using the interface address of the server)
    // csma_interfaces.GetAddress (number_of_CSMA_nodes): interface address of the server
    UdpEchoClientHelper echoClient(csma_interfaces.GetAddress(number_of_CSMA_nodes), 3019);
    echoClient.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    //  n7: client
    ApplicationContainer client_apps = echoClient.Install(wifi_station_mode_nodes.Get(nWifi - 1));
    client_apps.Start(Seconds(2.0));
    client_apps.Stop(Seconds(10.0));

    // Populate the routing tables for all the nodes
    // default routing algorithm in ns3 is global routing population
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(10.0));

    if (tracing) {
        // Packet tracing using Wireshark
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        point_to_point.EnablePcapAll("207930_Cycle_1_3_wireshark_p2p");
        phy.EnablePcap("207930_Cycle_1_3_wireshark_wireless", ap_devices.Get(0));
        csma.EnablePcap("207930_Cycle_1_3_wireshark_csma", csma_devices.Get(0), true);
    }

    // Animation using NetAnim
    AnimationInterface net_anim("207930_Cycle_1_3_net_anim.xml");
    net_anim.SetConstantPosition(csma_nodes.Get(0), 10.0, 10.0);
    net_anim.SetConstantPosition(csma_nodes.Get(1), 10.0, 30.0);
    net_anim.SetConstantPosition(csma_nodes.Get(2), 10.0, 50.0);
    net_anim.SetConstantPosition(csma_nodes.Get(3), 10.0, 70.0);
    net_anim.SetConstantPosition(wifi_station_mode_nodes.Get(0), 30.0, 10.0);
    net_anim.SetConstantPosition(wifi_station_mode_nodes.Get(1), 50.0, 10.0);
    net_anim.SetConstantPosition(wifi_station_mode_nodes.Get(2), 70.0, 10.0);
    net_anim.SetConstantPosition(wifi_access_point_node.Get(0), 100.0, 100.0);

    // ASCII tracing for throughput using TraceMetrics (throughput as well)
    AsciiTraceHelper ascii_trace_helper;
    point_to_point.EnableAsciiAll(ascii_trace_helper.CreateFileStream("207930_Cycle_1_3_trace_p2p.tr"));
    csma.EnableAsciiAll(ascii_trace_helper.CreateFileStream("207930_Cycle_1_3_trace_csma.tr"));

    Simulator::Run();

    cout << string(80, '=') << endl;

    Simulator::Destroy();
    return 0;
}
