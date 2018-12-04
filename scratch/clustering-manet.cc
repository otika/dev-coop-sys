/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 University of Athens (UOA)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:  - Lampros Katsikas <lkatsikas@di.uoa.gr>
 *          - Konstantinos Chatzikokolakis <kchatzi@di.uoa.gr>
 */

#include "ns3/core-module.h"
#include "ns3/ocb-wifi-mac.h"
#include "ns3/network-module.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/aodv-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/random-variable-stream.h"
#include "ns3/cluster-control-client-helper.h"
#include "ns3/cluster-sap.h"

 #include "ns3/netanim-module.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <stdexcept>

#define SIMULATION_TIME_FORMAT(s) Seconds(s)

#define STD_NODE_SIZE 0.1
#define WIFI_POWER -25 // -24

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("ClusteringExample");

AnimationInterface *pAnim = 0;
NodeContainer nodes_copy;
std::ofstream outputfile("/media/sf_WaveVisualizer/render.txt");

struct Rgb {
    uint8_t r, g, b;
};

static const Vector position_list[] ={
		{0, 0, 0},
		{1, 0, 0},
		{2, 0, 0},
		{3, 0, 0},
		{4, 0, 0},
		{5, 0, 0},
		{6, 0, 0},
		{7, 0, 0},
		{8, 0, 0},
		{9, 0, 0},
		{0, 1, 0},
		{1, 1, 0},
		{2, 1, 0},
		{3, 1, 0},
		{4, 1, 0},
		{5, 1, 0},
		{6, 1, 0},
		{7, 1, 0},
		{8, 1, 0},
		{9, 1, 0},
		{0, 2, 0},
		{1, 2, 0},
		{2, 2, 0},
		{3, 2, 0},
		{4, 2, 0},
		{5, 2, 0},
		{6, 2, 0},
		{7, 2, 0},
		{8, 2, 0},
		{9, 2, 0},
		{0, 3, 0},
		{1, 3, 0},
		{2, 4, 0},
		{3, 3, 0},
		{4, 3, 0},
		{5, 3, 0},
		{6, 3, 0},
		{7, 3, 0},
		{8, 3, 0},
		{9, 3, 0},
		{0, 4, 0},
		{1, 4, 0},
		{2, 4, 0},
		{3, 4, 0},
		{4, 4, 0},
		{5, 4, 0},
		{6, 4, 0},
		{7, 4, 0},
		{8, 4, 0},
		{9, 4, 0},
		{0, 5, 0},
		{1, 5, 0},
		{2, 5, 0},
		{3, 5, 0},
		{4, 5, 0},
		{5, 5, 0},
		{6, 5, 0},
		{7, 5, 0},
		{8, 5, 0},
		{9, 5, 0},
		{0, 5, 0},
		{1, 5, 0},
		{2, 5, 0},
		{3, 5, 0},
		{4, 5, 0},
		{5, 5, 0},
		{6, 5, 0},
		{7, 5, 0},
		{8, 5, 0},
		{9, 5, 0},
		{0, 6, 0},
		{1, 6, 0},
		{2, 6, 0},
		{3, 6, 0},
		{4, 6, 0},
		{5, 6, 0},
		{6, 6, 0},
		{7, 6, 0},
		{8, 6, 0},
		{9, 6, 0},
		{0, 7, 0},
		{1, 7, 0},
		{2, 7, 0},
		{3, 7, 0},
		{4, 7, 0},
		{5, 7, 0},
		{6, 7, 0},
		{7, 7, 0},
		{8, 7, 0},
		{9, 7, 0},
		{0, 8, 0},
		{1, 8, 0},
		{2, 8, 0},
		{3, 8, 0},
		{4, 8, 0},
		{5, 8, 0},
		{6, 8, 0},
		{7, 8, 0},
		{8, 8, 0},
		{9, 8, 0},
		{0, 9, 0},
		{1, 9, 0},
		{2, 9, 0},
		{3, 9, 0},
		{4, 9, 0},
		{5, 9, 0},
		{6, 9, 0},
		{7, 9, 0},
		{8, 9, 0},
		{9, 9, 0},

		{10, 0, 0},
		{11, 0, 0},
		{12, 0, 0},
		{13, 0, 0},
		{14, 0, 0},
		{15, 0, 0},
		{16, 0, 0},
		{17, 0, 0},
		{18, 0, 0},
		{19, 0, 0},
		{10, 1, 0},
		{11, 1, 0},
		{12, 1, 0},
		{13, 1, 0},
		{14, 1, 0},
		{15, 1, 0},
		{16, 1, 0},
		{17, 1, 0},
		{18, 1, 0},
		{19, 1, 0},
		{10, 2, 0},
		{11, 2, 0},
		{12, 2, 0},
		{13, 2, 0},
		{14, 2, 0},
		{15, 2, 0},
		{16, 2, 0},
		{17, 2, 0},
		{18, 2, 0},
		{19, 2, 0},
		{10, 3, 0},
		{11, 3, 0},
		{12, 4, 0},
		{13, 3, 0},
		{14, 3, 0},
		{15, 3, 0},
		{16, 3, 0},
		{17, 3, 0},
		{18, 3, 0},
		{19, 3, 0},
		{10, 4, 0},
		{11, 4, 0},
		{12, 4, 0},
		{13, 4, 0},
		{14, 4, 0},
		{15, 4, 0},
		{16, 4, 0},
		{17, 4, 0},
		{18, 4, 0},
		{19, 4, 0},
		{10, 5, 0},
		{11, 5, 0},
		{12, 5, 0},
		{13, 5, 0},
		{14, 5, 0},
		{15, 5, 0},
		{16, 5, 0},
		{17, 5, 0},
		{18, 5, 0},
		{19, 5, 0},
		{10, 5, 0},
		{11, 5, 0},
		{12, 5, 0},
		{13, 5, 0},
		{14, 5, 0},
		{15, 5, 0},
		{16, 5, 0},
		{17, 5, 0},
		{18, 5, 0},
		{19, 5, 0},
		{10, 6, 0},
		{11, 6, 0},
		{12, 6, 0},
		{13, 6, 0},
		{14, 6, 0},
		{15, 6, 0},
		{16, 6, 0},
		{17, 6, 0},
		{18, 6, 0},
		{19, 6, 0},
		{10, 7, 0},
		{11, 7, 0},
		{12, 7, 0},
		{13, 7, 0},
		{14, 7, 0},
		{15, 7, 0},
		{16, 7, 0},
		{17, 7, 0},
		{18, 7, 0},
		{19, 7, 0},
		{10, 8, 0},
		{11, 8, 0},
		{12, 8, 0},
		{13, 8, 0},
		{14, 8, 0},
		{15, 8, 0},
		{16, 8, 0},
		{17, 8, 0},
		{18, 8, 0},
		{19, 8, 0},
		{10, 9, 0},
		{11, 9, 0},
		{12, 9, 0},
		{13, 9, 0},
		{14, 9, 0},
		{15, 9, 0},
		{16, 9, 0},
		{17, 9, 0},
		{18, 9, 0},
		{19, 9, 0},

		{10, 10, 0},
		{11, 20, 0},
		{12, 10, 0},
		{13, 10, 0},
		{14, 10, 0},
		{15, 10, 0},
		{16, 10, 0},
		{17, 10, 0},
		{18, 10, 0},
		{19, 10, 0},
		{10, 11, 0},
		{11, 11, 0},
		{12, 11, 0},
		{13, 11, 0},
		{14, 11, 0},
		{15, 11, 0},
		{16, 11, 0},
		{17, 11, 0},
		{18, 11, 0},
		{19, 11, 0},
		{10, 12, 0},
		{11, 12, 0},
		{12, 12, 0},
		{13, 12, 0},
		{14, 12, 0},
		{15, 12, 0},
		{16, 12, 0},
		{17, 12, 0},
		{18, 12, 0},
		{19, 12, 0},
		{10, 13, 0},
		{11, 13, 0},
		{12, 14, 0},
		{13, 13, 0},
		{14, 13, 0},
		{15, 13, 0},
		{16, 13, 0},
		{17, 13, 0},
		{18, 13, 0},
		{19, 13, 0},
		{10, 14, 0},
		{11, 14, 0},
		{12, 14, 0},
		{13, 14, 0},
		{14, 14, 0},
		{15, 14, 0},
		{16, 14, 0},
		{17, 14, 0},
		{18, 14, 0},
		{19, 14, 0},
		{10, 15, 0},
		{11, 15, 0},
		{12, 15, 0},
		{13, 15, 0},
		{14, 15, 0},
		{15, 15, 0},
		{16, 15, 0},
		{17, 15, 0},
		{18, 15, 0},
		{19, 15, 0},
		{10, 15, 0},
		{11, 15, 0},
		{12, 15, 0},
		{13, 15, 0},
		{14, 15, 0},
		{15, 15, 0},
		{16, 15, 0},
		{17, 15, 0},
		{18, 15, 0},
		{19, 15, 0},
		{10, 16, 0},
		{11, 16, 0},
		{12, 16, 0},
		{13, 16, 0},
		{14, 16, 0},
		{15, 16, 0},
		{16, 16, 0},
		{17, 16, 0},
		{18, 16, 0},
		{19, 16, 0},
		{10, 17, 0},
		{11, 17, 0},
		{12, 17, 0},
		{13, 17, 0},
		{14, 17, 0},
		{15, 17, 0},
		{16, 17, 0},
		{17, 17, 0},
		{18, 17, 0},
		{19, 17, 0},
		{10, 18, 0},
		{11, 18, 0},
		{12, 18, 0},
		{13, 18, 0},
		{14, 18, 0},
		{15, 18, 0},
		{16, 18, 0},
		{17, 18, 0},
		{18, 18, 0},
		{19, 18, 0},
		{10, 19, 0},
		{11, 19, 0},
		{12, 19, 0},
		{13, 19, 0},
		{14, 19, 0},
		{15, 19, 0},
		{16, 19, 0},
		{17, 19, 0},
		{18, 19, 0},
		{19, 19, 0},

		{10, 20, 0},
		{11, 20, 0},
		{12, 20, 0},
		{13, 20, 0},
		{14, 20, 0},
		{15, 20, 0},
		{16, 20, 0},
		{17, 20, 0},
		{18, 20, 0},
		{19, 20, 0},
		{10, 21, 0},
		{11, 21, 0},
		{12, 21, 0},
		{13, 21, 0},
		{14, 21, 0},
		{15, 21, 0},
		{16, 21, 0},
		{17, 21, 0},
		{18, 21, 0},
		{19, 21, 0},
		{10, 22, 0},
		{11, 22, 0},
		{12, 22, 0},
		{13, 22, 0},
		{14, 22, 0},
		{15, 22, 0},
		{16, 22, 0},
		{17, 22, 0},
		{18, 22, 0},
		{19, 22, 0},
		{10, 23, 0},
		{11, 23, 0},
		{12, 24, 0},
		{13, 23, 0},
		{14, 23, 0},
		{15, 23, 0},
		{16, 23, 0},
		{17, 23, 0},
		{18, 23, 0},
		{19, 23, 0},
		{10, 24, 0},
		{11, 24, 0},
		{12, 24, 0},
		{13, 24, 0},
		{14, 24, 0},
		{15, 24, 0},
		{16, 24, 0},
		{17, 24, 0},
		{18, 24, 0},
		{19, 24, 0},
		{10, 25, 0},
		{11, 25, 0},
		{12, 25, 0},
		{13, 25, 0},
		{14, 25, 0},
		{15, 25, 0},
		{16, 25, 0},
		{17, 25, 0},
		{18, 25, 0},
		{19, 25, 0},
		{10, 25, 0},
		{11, 25, 0},
		{12, 25, 0},
		{13, 25, 0},
		{14, 25, 0},
		{15, 25, 0},
		{16, 25, 0},
		{17, 25, 0},
		{18, 25, 0},
		{19, 25, 0},
		{10, 26, 0},
		{11, 26, 0},
		{12, 26, 0},
		{13, 26, 0},
		{14, 26, 0},
		{15, 26, 0},
		{16, 26, 0},
		{17, 26, 0},
		{18, 26, 0},
		{19, 26, 0},
		{10, 27, 0},
		{11, 27, 0},
		{12, 27, 0},
		{13, 27, 0},
		{14, 27, 0},
		{15, 27, 0},
		{16, 27, 0},
		{17, 27, 0},
		{18, 27, 0},
		{19, 27, 0},
		{10, 28, 0},
		{11, 28, 0},
		{12, 28, 0},
		{13, 28, 0},
		{14, 28, 0},
		{15, 28, 0},
		{16, 28, 0},
		{17, 28, 0},
		{18, 28, 0},
		{19, 28, 0},
		{10, 29, 0},
		{11, 29, 0},
		{12, 29, 0},
		{13, 29, 0},
		{14, 29, 0},
		{15, 29, 0},
		{16, 29, 0},
		{17, 29, 0},
		{18, 29, 0},
		{19, 29, 0}
};

template<class T> void shuffle(T ary[],int size);
Rgb getColor(int id);
Rgb getNewColor(uint8_t differential);
void StatusTraceCallback (Ptr<const ClusterControlClient> app);
void OutputRender(void);

int main(int argc, char *argv[]) {

    /*--------------------- Logging System Configuration -------------------*/
    LogLevel logLevel = (LogLevel) (LOG_PREFIX_ALL | LOG_LEVEL_WARN);
    //logLevel = LOG_ALL;
    LogComponentEnable("ClusteringExample", logLevel);
    LogComponentEnable("ClusterControlClient", logLevel);

    NS_LOG_INFO("/------------------------------------------------\\");
    NS_LOG_INFO(" - ClusteringExample [Example] -> Cluster vehicles communication");
    NS_LOG_INFO("\\------------------------------------------------/");
    /*----------------------------------------------------------------------*/

    /*---------------------- Simulation Default Values ---------------------*/
    std::string phyMode ("OfdmRate6MbpsBW10MHz");

    //uint16_t numberOfUes = 400;
    uint16_t numberOfUes = 100;
    int column = 10;
    double distance = 1.0;

    double minimumTdmaSlot = 0.001;         /// Time difference between 2 transmissions
    double clusterTimeMetric = 5.0;         /// Clustering Time Metric for Waiting Time calculation
    double incidentWindow = 30.0;

    double simTime = 30.0;

    /*-------------------- Set explicitly default values -------------------*/
    Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold",
                            StringValue ("2200"));
    // turn off RTS/CTS for frames below 2200 bytes
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold",
                            StringValue ("2200"));
    // Fix non-unicast data rate to be the same as that of unicast
    Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",
                            StringValue (phyMode));

    /*-------------------- Command Line Argument Values --------------------*/
    CommandLine cmd;
    cmd.AddValue("ueNumber", "Number of UE", numberOfUes);
    cmd.AddValue("simTime", "Simulation Time in Seconds", simTime);
    cmd.Parse (argc, argv);

    NS_LOG_INFO("");
    NS_LOG_INFO("|---"<< " SimTime -> " << simTime <<" ---|\n");
    NS_LOG_INFO("|---"<< " Number of UE -> " << numberOfUes <<" ---|\n");

    /*------------------------- Create UEs-EnodeBs -------------------------*/
    NodeContainer nodes;
    nodes.Create(numberOfUes);

    AodvHelper aodv;
    InternetStackHelper internet;
    internet.SetRoutingHelper(aodv);
    internet.Install(nodes);

    /*-------------------- Install Mobility Model in Ue --------------------*/
    MobilityHelper ueMobility;

    ueMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

    ueMobility.Install(nodes);

    /// Create a 3 line grid of vehicles
    int list[numberOfUes];
    for (uint16_t i = 0; i < numberOfUes; i++)
    {
    	list[i] = i;
    }
    shuffle<int>(list+1,numberOfUes-1);
    for (uint16_t i = 0; i < numberOfUes; i++)
    {
    	// Vector pos = Vector ( (i-i%column)/column*distance, i%column*distance, 0);
    	Vector pos = position_list[i];
        nodes.Get(list[i])->GetObject<MobilityModel> ()->SetPosition(pos);
    }

    /*-------------------------- Setup Wifi nodes --------------------------*/
    // The below set of helpers will help us to put together the wifi NICs we want
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    Ptr<YansWifiChannel> channel = wifiChannel.Create();

    YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
    wifiPhy.SetChannel (channel);
    wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
//    wifiPhy.Set ("TxPowerLevels", UintegerValue(16));
//    wifiPhy.Set ("TxPowerStart", DoubleValue(0.0));
//    wifiPhy.Set ("TxPowerEnd", DoubleValue(32));
    wifiPhy.Set ("TxGain", DoubleValue(WIFI_POWER));
    wifiPhy.Set ("RxGain", DoubleValue(WIFI_POWER));
    // wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-90));
    // wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-94));

    // using IEEE802.11p
    NqosWaveMacHelper wifiMac = NqosWaveMacHelper::Default ();
    Wifi80211pHelper wifi = Wifi80211pHelper::Default ();

    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue (phyMode),
                                      "ControlMode",StringValue (phyMode));

    NetDeviceContainer devices =  wifi.Install (wifiPhy, wifiMac, nodes);

    NS_LOG_INFO ("Assign IP Addresses.");
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase ("10.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer i1 = ipv4h.Assign (devices);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    uint16_t controlPort = 3999;
    ApplicationContainer controlApps;

    /**
     * Setting Control Channel
     */
    PacketMetadata::Enable ();
    for (uint32_t u = 0; u < nodes.GetN(); ++u) {
        //!< Initial TDMA UE synchronization Function
		// double tdmaStart = (u+1)*minimumTdmaSlot;
        double tdmaStart = (nodes.GetN() - u)*minimumTdmaSlot;

        Ptr<ConstantPositionMobilityModel> mobilityModel = nodes.Get(u)->GetObject<ConstantPositionMobilityModel>();

        // クラスタリング機能
        ClusterControlClientHelper ueClient(
        		"ns3::UdpSocketFactory",
        		Address(InetSocketAddress(Ipv4Address::GetBroadcast(), controlPort)),
                "ns3::UdpSocketFactory",
				InetSocketAddress(Ipv4Address::GetAny(), controlPort),
                mobilityModel, tdmaStart, numberOfUes, minimumTdmaSlot, clusterTimeMetric);

        ueClient.SetAttribute ("IncidentWindow", DoubleValue(incidentWindow));
        controlApps.Add(ueClient.Install(nodes.Get(u)));

        Ptr<Application> super_app = nodes.Get(u)->GetApplication(0);
        super_app->TraceConnectWithoutContext("Status", MakeCallback(&StatusTraceCallback));

        Ptr<ClusterControlClient> app = Ptr<ClusterControlClient> ( dynamic_cast<ClusterControlClient *> (PeekPointer(super_app)) );
        app->SetClusteringStartTime(Seconds(0.1 + tdmaStart));
        app->SetClusteringStopTime(Seconds(3.0 + tdmaStart));

        if(u == 0) {
        	app->SetStartingNode(true);
        	app->SetBasePropagationVector(Vector(1.0, 0.0, 0.0));
        }
    }

    nodes_copy = nodes;
    Simulator::Schedule(Seconds(0.1) ,&OutputRender);

//    pAnim = new AnimationInterface("scratch/clustering-manet.xml");
//
//	for (uint32_t u = 0; u < nodes.GetN(); ++u) {
//		pAnim->UpdateNodeSize(u, STD_NODE_SIZE, STD_NODE_SIZE);
//		pAnim->UpdateNodeColor(u, 255,255,255);
//	}

    controlApps.Start (Seconds(0.0));
    controlApps.Stop (Seconds(simTime-0.1));

//     AsciiTraceHelper ascii;
//     wifiPhy.EnableAsciiAll(ascii.CreateFileStream ("scratch/socket-options-ipv4.txt"));
//     wifiPhy.EnablePcapAll ("scratch/cluser.socket.pcap", false);
    // Flow monitor
//    Ptr<FlowMonitor> flowMonitor;
//    FlowMonitorHelper flowHelper;
//    flowMonitor = flowHelper.InstallAll();

    /*----------------------------------------------------------------------*/

    /*---------------------- Simulation Stopping Time ----------------------*/
    Simulator::Stop(SIMULATION_TIME_FORMAT(simTime));
    /*----------------------------------------------------------------------*/

    /*--------------------------- Simulation Run ---------------------------*/
    Simulator::Run();
    outputfile.close();
    std::cout << "IsFinished " << Simulator::IsFinished() << std::endl;
    Simulator::Destroy();
    /*----------------------------------------------------------------------*/

//    flowMonitor->SerializeToXmlFile("scratch/flow-mon.xml", true, true);

//    delete pAnim;

    return EXIT_SUCCESS;
}


void StatusTraceCallback (Ptr<const ClusterControlClient> app)
{
  Rgb rgb;
  NS_LOG_DEBUG("StatusChanged");
  if(pAnim != NULL){
    ClusterSap::NeighborInfo info = app->GetCurrentMobility();
    int id = info.imsi;
    int clusterId = info.clusterId;
    ClusterSap::NodeDegree degree = info.degree;

    // Set color
    if(degree == ClusterSap::STANDALONE){
          rgb = {255, 255, 255};
    }
    if(degree == ClusterSap::CM || degree == ClusterSap::CH)
    {
      rgb = getColor(clusterId);
    }

    pAnim->UpdateNodeColor(id, rgb.r, rgb.g, rgb.b);

    // Set size
    if(degree == ClusterSap::CH)
    {
      pAnim->UpdateNodeSize(id, STD_NODE_SIZE * 4, STD_NODE_SIZE * 4); // CH
    }
    else
    {
      pAnim->UpdateNodeSize(id, STD_NODE_SIZE * 2, STD_NODE_SIZE * 2); // CM
    }
  }
  return;
}

Rgb getColor(int id)
{
  static std::vector<Rgb> rgb(2000, (Rgb){255, 255, 255}); // History
  Rgb _rgb = {255, 255, 255}; // workmemory

  // Load rgb from output history
  try {
    _rgb = rgb.at(id);
  }  catch(const std::out_of_range oor) {
	  _rgb = (Rgb) {255, 255, 255};
  }

  // Generate New Color
  if(_rgb.r == 255 && _rgb.g == 255 && _rgb.b == 255){
    bool duplicated = false;
    unsigned long long loop_count = 0;
    do{
      duplicated = false;
      _rgb = getNewColor(3);
      //for(auto itr = std::begin(rgb); itr != std::end(rgb); ++itr){
      for(auto itr : rgb){
        if(_rgb.r == itr.r && _rgb.g == itr.g && _rgb.b == itr.b)
        {
          duplicated = true;
          break;
        }
      }
      loop_count++;
      if(loop_count > 1000){
    	  std::cout << "[TOO MANY LOOP] " << loop_count << " times" << std::endl;
      }
    }while(duplicated);
    rgb[id] = _rgb;
  }

  return _rgb;
}

Rgb getNewColor(uint8_t differential){
  // avoid error of division by zero
  if(differential == 0){
    differential = 1;
  }
  static std::random_device rnd;     // 非決定的な乱数生成器を生成
  static std::mt19937 mt(12345);     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値

  std::uniform_int_distribution<> rand255(0, 255 / differential);

  int _r = rand255(mt) * differential;
  int _g = rand255(mt) * differential;
  int _b = rand255(mt) * differential;
  Rgb rgb = {(uint8_t)_r, (uint8_t)_g, (uint8_t)_b};

  return rgb;
}

template<class T> void shuffle(T ary[],int size)
{
	static std::random_device rnd;     // 非決定的な乱数生成器を生成
	static std::mt19937 mt(12345);     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_int_distribution<> rand(0, size - 1);

	for(int i=0;i<size;i++)
    {
        int j = rand(mt);
        T t = ary[i];
        ary[i] = ary[j];
        ary[j] = t;
    }
}

void OutputRender(void){
	std::cout << "rendered " << (double)Simulator::Now().GetSeconds() << std::endl;

	outputfile << (double)Simulator::Now().GetSeconds() << " ";
	for(uint32_t i = 0; i < nodes_copy.GetN(); i++){
		Ptr<Application> super_app = nodes_copy.Get(i)->GetApplication(0);
		Ptr<ClusterControlClient> app = Ptr<ClusterControlClient> ( dynamic_cast<ClusterControlClient *> (PeekPointer(super_app)) );
		ClusterSap::NeighborInfo info = app->GetCurrentMobility();
		ClusterControlClient::NodeStatus nodeStatus = app->GetNodeStatus();
		int status = 0;
		if(nodeStatus == ClusterControlClient::PROPAGATION_READY){
			status = 1;
		}
		else if(nodeStatus == ClusterControlClient::PROPAGATION_RUNNING){
			status = 2;
		}
		else if(nodeStatus == ClusterControlClient::PROPAGATION_COMPLETE){
			status = 3;
		}

		Rgb rgb;
		// Set color
		if(info.degree == ClusterSap::STANDALONE){
			  rgb = {255, 255, 255};
		}
		if(info.degree == ClusterSap::CM || info.degree == ClusterSap::CH)
		{
		  rgb = getColor(info.clusterId);
		}

		outputfile << info.imsi 	<< " "		// id
				<< info.clusterId	<< " "		// clusterId
				<< info.degree 		<< " "		// degree
				<< info.position.x	<< " "		// pos.x
				<< info.position.y	<< " " 		// pos.y
				<< info.position.z	<< " "		// pos.z
				<< 0				<< " "		// state (ready, active, finish)
				<< (int)rgb.r			<< " "
				<< (int)rgb.g			<< " "
				<< (int)rgb.b			<< " "
				<< status 				<< " ";
	}
	outputfile << std::endl;
	Simulator::Schedule(Seconds(0.01) ,&OutputRender);
}
