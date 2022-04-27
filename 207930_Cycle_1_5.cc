/*
 * Author: mahendrasinghbora
 * Date: 2022-04-17 11:07:05
 */


#include <fstream>
#include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/internet-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/applications-module.h"
#include "ns3/netanim-module.h" // For NetAnim
 

 using namespace ns3;
using namespace std;

 
// Network Topology Used
//
//       n0 ---+      +--- n2
//             |      |
//             n4 -- n5
//             |      |
//       n1 ---+      +--- n3
//
// TCP flow form n0 to n2
// UDP flow from n1 to n3

NS_LOG_COMPONENT_DEFINE ("Cycle - 1 (Experiment - 5)");

 
 class MyApp : public Application 
 {
 public:
 
   MyApp ();
   virtual ~MyApp();
 
   void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
 
 private:
   virtual void StartApplication (void);
   virtual void StopApplication (void);
 
   void ScheduleTx (void);
   void SendPacket (void);
 
   Ptr<Socket>     m_socket;
   Address         m_peer;
   uint32_t        m_packetSize;
   uint32_t        m_nPackets;
   DataRate        m_dataRate;
   EventId         m_sendEvent;
   bool            m_running;
   uint32_t        m_packetsSent;
 };
 
 MyApp::MyApp ()
   : m_socket (0), 
     m_peer (), 
     m_packetSize (0), 
     m_nPackets (0), 
     m_dataRate (0), 
     m_sendEvent (), 
     m_running (false), 
     m_packetsSent (0)
 {
 }
 
 MyApp::~MyApp()
 {
   m_socket = 0;
 }
 
 void
 MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
 {
   m_socket = socket;
   m_peer = address;
   m_packetSize = packetSize;
   m_nPackets = nPackets;
   m_dataRate = dataRate;
 }
 
 void
 MyApp::StartApplication (void)
 {
   m_running = true;
   m_packetsSent = 0;
   m_socket->Bind ();
   m_socket->Connect (m_peer);
   SendPacket ();
 }
 
 void 
 MyApp::StopApplication (void)
 {
   m_running = false;
 
   if (m_sendEvent.IsRunning ())
     {
       Simulator::Cancel (m_sendEvent);
     }
 
   if (m_socket)
     {
       m_socket->Close ();
     }
 }
 
 void 
 MyApp::SendPacket (void)
 {
   Ptr<Packet> packet = Create<Packet> (m_packetSize);
   m_socket->Send (packet);
 
   if (++m_packetsSent < m_nPackets)
     {
       ScheduleTx ();
     }
 }
 
 void 
 MyApp::ScheduleTx (void)
 {
   if (m_running)
     {
       Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
       m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
     }
 }
 
 static void
 CwndChange (uint32_t oldCwnd, uint32_t newCwnd)
 {
   NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
 }
 
 static void
 RxDrop (Ptr<const Packet> p)
 {
   NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
 }
 
 int 
 main (int argc, char *argv[])
 {
       cout << string(80, '=') << endl;
    cout << "Cycle - 1 (Experiment - 5)" << endl;
    cout << string(80, '=') << endl;
   CommandLine cmd (__FILE__);
   cmd.Parse (argc, argv);
   
   NodeContainer nodes;
   nodes.Create (2);
 
   PointToPointHelper point_to_point;
   point_to_point.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   point_to_point.SetChannelAttribute ("Delay", StringValue ("2ms"));
 
   NetDeviceContainer devices;
   devices = point_to_point.Install (nodes);
 
   Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
   em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
   devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
 
   InternetStackHelper stack;
   stack.Install (nodes);
 
   Ipv4AddressHelper address;
   address.SetBase ("10.1.1.0", "255.255.255.252");
   Ipv4InterfaceContainer interfaces = address.Assign (devices);
 
   uint16_t sink_port = 8080;
   Address sinkAddress (InetSocketAddress (interfaces.GetAddress (1), sink_port));
   PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sink_port));
   ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (1));
   sinkApps.Start (Seconds (0.));
   sinkApps.Stop (Seconds (20.));
 
   Ptr<Socket> ns3_tcp_socket = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
   ns3_tcp_socket->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));
 
   Ptr<MyApp> app = CreateObject<MyApp> ();
   app->Setup (ns3_tcp_socket, sinkAddress, 1040, 1000, DataRate ("1Mbps"));
   nodes.Get (0)->AddApplication (app);
   app->SetStartTime (Seconds (1.));
   app->SetStopTime (Seconds (20.));
 
   devices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback (&RxDrop));
 

// Animation using NetAnim
    // AnimationInterface net_anim("207930_Cycle_1_5_net_anim.xml");
    // net_anim.SetConstantPosition(nodes.Get(0), 40.0, 40.0);
    // net_anim.SetConstantPosition(nodes.Get(1), 40.0, 10.0);

    // ASCII tracing for throughput using TraceMetrics
    AsciiTraceHelper ascii_trace_helper;
    point_to_point.EnableAsciiAll(ascii_trace_helper.CreateFileStream("207930_Cycle_1_5_trace.tr"));

    // Packet tracing using Wireshark
    point_to_point.EnablePcapAll("207930_Cycle_1_5_wireshark"); 
   Simulator::Stop (Seconds (20));
   Simulator::Run ();
    cout << string(80, '=') << endl;
   Simulator::Destroy ();
 
   return 0;
 }
 