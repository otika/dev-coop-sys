/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

// ref https://github.com/Narendra-Kamath/NS3-SIMPLE-ROUTING-AODV-DSR

// NS-3.29 標準モジュール
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/aodv-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"

// NS-3 自作モジュール
#include "util.h"
#include "clustering.h"
#include "neighbor.h"

// C++ 標準モジュール
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace ns3;
using namespace std;

int packetsSent = 0;
int packetsReceived = 0;

void ReceivePacket (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;

  uint8_t buf[1024];
  memset(buf, 0, sizeof(buf));

  while ((packet = socket->RecvFrom (from))) {
    if (packet->GetSize () > 0) {
      packet->CopyData(buf, 1024);
      InetSocketAddress iaddr = InetSocketAddress::ConvertFrom (from);
      char t[10];
      sprintf(t,"%8.5f",Simulator::Now ().GetSeconds ());
      std::cout << std::setw(10) << std::setprecision(4)
      << t << " received "<< packet->GetSize ()
      << " bytes from: (" << iaddr.GetIpv4 ()
      << ", " << iaddr.GetPort () << ")"
      << " --- " << buf << std::endl;
      packetsReceived++;
    }
  }
}

static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
  uint32_t pktCount, Time pktInterval )
  {
    if (pktCount > 0)
    {
      // パケット作成
      std::string greeting_msg = "hello";
      uint8_t  buf[1024];
      uint16_t bufsize;
      bufsize = greeting_msg.size();
      memcpy(buf, greeting_msg.c_str(),bufsize);
      Ptr<Packet> packet = Create<Packet> (buf, bufsize);
//
      //socket->Send (Create<Packet> (pktSize));
      socket->Send (packet);
      packetsSent++;
      std::cout<<"Packet sent - "<<packetsSent<<std::endl;

      Simulator::Schedule (pktInterval, &GenerateTraffic,
        socket, pktSize,pktCount-1, pktInterval);
      }
      else
      {
        socket->Close ();
      }
    }

    int main(int argc, char **argv)
    {
      uint32_t size=100;
      double step=10;
      double totalTime=10;

      int packetSize = 1024;
      int totalPackets = totalTime-1;
      double interval = 0.1;
      Time interPacketInterval = Seconds (interval);

      // ネットワークの物理構成要素
      NodeContainer nodes;
      NetDeviceContainer devices;
      Ipv4InterfaceContainer interfaces;

      std::cout << "Creating " << (unsigned)size << " nodes " << step << " m apart.\n";
      nodes.Create (size);

      // ネットワークの構造
      double distance = 10; // (m)
      MobilityHelper mobility;
      mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
      "MinX", DoubleValue (0.0),
      "MinY", DoubleValue (0.0),
      "DeltaX", DoubleValue (distance),
      "DeltaY", DoubleValue (distance),
      "GridWidth", UintegerValue (10),
      "LayoutType", StringValue ("RowFirst"));
      mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      mobility.Install (nodes);

      // 通信メディア
      // NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
      WifiMacHelper wifiMac; // for upper than 3.28wifiMac.SetType ("ns3::AdhocWifiMac");

      YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
      YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
      wifiPhy.SetChannel (wifiChannel.Create ());
      WifiHelper wifi;
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));
      devices = wifi.Install (wifiPhy, wifiMac, nodes);

      // ルーチング
      AodvHelper aodv;
      InternetStackHelper stack;
      stack.SetRoutingHelper (aodv);
      stack.Install (nodes);
      Ipv4AddressHelper address;
      address.SetBase ("10.0.0.0", "255.0.0.0");
      interfaces = address.Assign (devices);

      Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

      // UDPソケット
      // 受信側　待ち受けるIPアドレスとポート番号を指定
      // 0.0.0.0(GetAny)を待ち受けているので任意のIPアドレスからの8080番ポートへのパケットを受信する
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 8080);

      std::vector<Ptr<Socket>> recvSinkList(size);
      for(auto itr = recvSinkList.begin(); itr != recvSinkList.end(); ++itr)
      {
        int i = std::distance(recvSinkList.begin(), itr);
        std::cout << i << ", apps:" << nodes.Get(i)->GetObject<ns3::MobilityModel>()->GetPosition().x << std::endl;

        *itr = Socket::CreateSocket (nodes.Get (i), tid);
        (*itr)->Bind (local);
        (*itr)->SetRecvCallback (MakeCallback (&ReceivePacket));
      }

      printVector(GetNeighbors(nodes.Get(0), nodes, 25.0));

      // 送信側
      Ptr<Socket> source = Socket::CreateSocket (nodes.Get (0), tid);
      InetSocketAddress remote = InetSocketAddress (interfaces.GetAddress (/*size-*/1,0), 8080);
      source->Connect (remote);

      AnimationInterface anim ("scratch/aodv-output2.xml");
      anim.EnablePacketMetadata (); // Optional
      // anim.EnableIpv4L3ProtocolCounters (Seconds (0), Seconds (10)); // Optional
      anim.UpdateNodeColor(0, 0, 0, 255);

      Simulator::Schedule (Seconds (1), &GenerateTraffic, source, packetSize, totalPackets, interPacketInterval);

      std::cout << "Starting simulation for " << totalTime << " s ...\n";

      Simulator::Stop (Seconds (totalTime));
      Simulator::Run ();

      Simulator::Destroy ();

      std::cout<<"\n\n***** OUTPUT *****\n\n";
      std::cout<<"Total Packets sent = "<<packetsSent<<std::endl;
      std::cout<<"Total Packets received = "<<packetsReceived<<std::endl;
      std::cout<<"Packet delivery ratio = "<<(float)(packetsReceived/packetsSent)*100<<" %%"<<std::endl;
      return 0;
    }



    /****Neighbor 本当はneighbor.cppに実装したい******************************************/



  /****Clustering 本当はclustering.cppに実装したい******************************************/
  /***************************************************************************************/
