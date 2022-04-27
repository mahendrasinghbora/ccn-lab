/*
 * Author: mahendrasinghbora
 * Date: 2022-03-28 16:21:44
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h" // For NetAnim
#include "ns3/flow-monitor.h" // For throughput
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("Cycle - 1 (Experiment - 1)");

int main() {
    int packet_size = 2048;
    cout << string(80, '=') << endl;
    cout << "Cycle - 1 (Experiment - 1)" << endl;
    cout << string(80, '=') << endl;

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(2);

    PointToPointHelper point_to_point;
    point_to_point.SetDeviceAttribute("DataRate", StringValue("8Mbps"));
    point_to_point.SetChannelAttribute("Delay", StringValue("5ms"));

    NetDeviceContainer devices;
    devices = point_to_point.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    UdpEchoServerHelper echoServer1(7000);

    ApplicationContainer serverApps = echoServer1.Install(nodes.Get(1));
    serverApps.Start(Seconds(0.0));
    serverApps.Stop(Seconds(5.0));

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 7000);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(packet_size));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    UdpEchoServerHelper echoServer2(8000);

    ApplicationContainer serverApps2 = echoServer2.Install(nodes.Get(1));
    serverApps2.Start(Seconds(1.0));
    serverApps2.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient2(interfaces.GetAddress(1), 8000);
    echoClient2.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient2.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient2.SetAttribute("PacketSize", UintegerValue(packet_size));

    ApplicationContainer clientApps2 = echoClient2.Install(nodes.Get(0));
    clientApps2.Start(Seconds(2.0));
    clientApps2.Stop(Seconds(10.0));

    // Animation using NetAnim
    AnimationInterface net_anim("207930_Cycle_1_1_net_anim.xml");
    net_anim.SetConstantPosition(nodes.Get(0), 30.0, 10.0, 10);
    net_anim.SetConstantPosition(nodes.Get(1), 50.0, 50.0, 0);

    // ASCII tracing for throughput using TraceMetric
    AsciiTraceHelper ascii_trace_helper;
    point_to_point.EnableAsciiAll(ascii_trace_helper.CreateFileStream("207930_Cycle_1_1_trace.tr"));

    // Packet tracing using Wireshark
    point_to_point.EnablePcapAll("207930_Cycle_1_1_wireshark");

    // Calculate the throughput
    FlowMonitorHelper flow_monitor;
    Ptr <FlowMonitor> monitor = flow_monitor.InstallAll();

    NS_LOG_INFO("Run the Simulation.");

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    monitor->CheckForLostPackets();

    cout << string(80, '=') << endl;

    Ptr <Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flow_monitor.GetClassifier());
    map <FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();

    while (i != stats.end()) {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);

        if ((t.sourceAddress == "10.1.1.1" && t.destinationAddress == "10.1.1.2")) {
            cout << "Flow " << i->first << " (" << t.sourceAddress << " to " << t.destinationAddress << ")\n";
            cout << "Transmitted (Tx) Bytes: " << i->second.txBytes << "\n";
            cout << "Received (Rx) Bytes: " << i->second.rxBytes << "\n";
            cout << "Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() -
                                                                 i->second.timeFirstTxPacket.GetSeconds()) /
                                      packet_size / packet_size << " Mbps\n";
        }

        i++;
    }

    monitor->SerializeToXmlFile("207930_Cycle_1_1_flow_monitor.xml", true, true);
    cout << string(80, '=') << endl;

    Simulator::Destroy();

    return 0;
}
