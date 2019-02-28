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
#include "ns3/olsr-module.h"
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
#define WIFI_POWER -18 // -24

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("ClusteringExample");

AnimationInterface *pAnim = 0;
NodeContainer nodes_copy;

const std::string dir = "./iofiles/";
const std::string arrange_file = dir + "arrange.txt";
const std::string render_file  = dir + "render.txt";
const std::string note_file = dir + "note.txt";
// char render_file[] = "scratch/render.txt";
std::ofstream outputfile(render_file);
std::ofstream notefile(note_file);

int seed = 0;

const bool CLUSTERING_DISABLE = false;

struct Rgb {
    uint8_t r, g, b;
};

template<class T> void shuffle(T ary[],int size);
Rgb getColor(int id);
Rgb getNewColor(uint8_t differential);
void StatusTraceCallback (Ptr<const ClusterControlClient> app);
void OutputRender(void);

Vector split(std::string& input, char delimiter);
std::vector<Vector> load_arrange(std::ifstream& ifs);

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

    std::ifstream ifs(arrange_file);
    std::vector<Vector> position_list = load_arrange(ifs);

    // std::cout << "Loaded " << position_list.size() << " positions" << std::endl;

    uint16_t numberOfMaxUes = position_list.size();
    // uint16_t numberOfUes = 223; // sparse
    // uint16_t numberOfUes = 669;
    uint16_t numberOfUes = 892;

    if (numberOfMaxUes < numberOfUes){
    	std::cout << "numberOfUes is too large" << std::endl;
    	numberOfUes = numberOfMaxUes;
    }

    int column = 10;
//    double distance = 1.0;
    double distance = 1.0; // reverse

    double minimumTdmaSlot = 0.001;         /// Time difference between 2 transmissions
    double clusterTimeMetric = 5.0;         /// Clustering Time Metric for Waiting Time calculation
    double incidentWindow = 40.0;

    double simTime = 100.0;

    int disableStartingNode = 0;

    /*-------------------- Set explicitly default values -------------------*/
//    Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold",
//                            StringValue ("2200"));
    // turn off RTS/CTS for frames below 2200 bytes
//    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold",
//                            StringValue ("2200"));
    // Fix non-unicast data rate to be the same as that of unicast
    Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",
                            StringValue (phyMode));

    /*-------------------- Command Line Argument Values --------------------*/
    CommandLine cmd;
    cmd.AddValue("ueNumber", "Number of UE", numberOfUes);
    cmd.AddValue("simTime", "Simulation Time in Seconds", simTime);
    cmd.AddValue("seed", "Seed of random number generator", seed);
    cmd.AddValue("disableStartingNode", "disable starting node in each cluster", disableStartingNode);
    cmd.Parse (argc, argv);

    NS_LOG_INFO("");
    NS_LOG_INFO("|---"<< " SimTime -> " << simTime <<" ---|\n");
    NS_LOG_INFO("|---"<< " Number of UE -> " << numberOfUes <<" ---|\n");

    /*------------------------- Create UEs-EnodeBs -------------------------*/
    NodeContainer nodes;
    nodes.Create(numberOfUes);

    //AodvHelper aodv;
//    OlsrHelper olsr;
//    olsr.Set("HelloInterval", TimeValue(Seconds(4.0)));
//    olsr.Set("TcInterval", TimeValue(Seconds(5.0)));
    InternetStackHelper internet;
    // internet.SetRoutingHelper(aodv);
    //internet.SetRoutingHelper(olsr);
    internet.Install(nodes);

    /*-------------------- Install Mobility Model in Ue --------------------*/
    MobilityHelper ueMobility;

    ueMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

    ueMobility.Install(nodes);

    /// Create a 3 line grid of vehicles
//    int list[numberOfUes];
//    for (uint16_t i = 0; i < numberOfUes; i++)
//    {
//    	list[i] = i;
//    }
//    shuffle<int>(list+1,numberOfUes-1);
    int list[numberOfMaxUes];
	for (int i = 0; i < numberOfMaxUes; i++)
	{
		list[i] = i;
	}
    shuffle<int>(list+1, numberOfMaxUes - 1);
    for (int i = 0; i < numberOfUes; i++)
    {
    	// Vector pos = Vector ( (i-i%column)/column*distance, i%column*distance, 0);
    	Vector pos = position_list[list[i]];
    	pos.x = pos.x * distance;
    	pos.y = pos.y * distance;
        nodes.Get(i)->GetObject<MobilityModel> ()->SetPosition(pos);
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
        ClusterControlClient::DISABLE_STARTINGNODE = (disableStartingNode > 0);
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

        if(!CLUSTERING_DISABLE){
        	app->SetClusteringStartTime(Seconds(1.0 + tdmaStart));
        }
        else{
        	app->SetClusteringStartTime(Seconds(0.0));
        }
        app->SetClusteringStopTime(Seconds(5.0));

//        app->SetClusteringStartTime(Seconds(1.0));
//        app->SetClusteringStopTime(Seconds(1.0));



        if(u == 0) {
        	app->SetStartingNode(true);
        	// app->SetBasePropagationDirection(Vector(2.0, 0.0, 0.0));
        	app->SetBasePropagationDirection(Vector(1.4, 1.4, 0.0));
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
    // std::cout << "IsFinished " << Simulator::IsFinished() << std::endl;

    int completed_num = 0;
    for (uint32_t u = 0; u < nodes.GetN(); ++u) {
        Ptr<Application> super_app = nodes.Get(u)->GetApplication(0);
        Ptr<ClusterControlClient> app = Ptr<ClusterControlClient> ( dynamic_cast<ClusterControlClient *> (PeekPointer(super_app)) );
        ClusterControlClient::NodeStatus status = app->GetNodeStatus();
        if(status == ClusterControlClient::PROPAGATION_COMPLETE){
        	completed_num++;
        }
        else{
        	//std::cout << u << ", " << status << std::endl;
        }
    }
    std::cout << "propagation_complete: " << completed_num << " total: " << nodes.GetN() << " rate: " << (double)completed_num / (double)nodes.GetN() << ": disableStartingNode :" << std::boolalpha << ClusterControlClient::DISABLE_STARTINGNODE  << std::endl;
    notefile << "propagation_complete: " << completed_num << " total: " << nodes.GetN() << " rate: " << (double)completed_num / (double)nodes.GetN() << std::endl;
    notefile.close();
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
	static std::mt19937 mt(seed);     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
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
//	std::cout << "rendered " << (double)Simulator::Now().GetSeconds() << std::endl;

	outputfile << (double)Simulator::Now().GetSeconds() << " ";
	for(uint32_t i = 0; i < nodes_copy.GetN(); i++){
		Ptr<Application> super_app = nodes_copy.Get(i)->GetApplication(0);
		Ptr<ClusterControlClient> app = Ptr<ClusterControlClient> ( dynamic_cast<ClusterControlClient *> (PeekPointer(super_app)) );
		ClusterSap::NeighborInfo info = app->GetCurrentMobility();
		ClusterControlClient::NodeStatus nodeStatus = app->GetNodeStatus();
		Vector direction = app->GetPropagationDirection();
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
		else if(nodeStatus == ClusterControlClient::ACTIVE){
			status = 4;
		}

		Rgb rgb;
		// Set color
		if(info.degree == ClusterSap::STANDALONE) {
			  rgb = {200, 200, 200};
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
				<< (int)rgb.r		<< " "
				<< (int)rgb.g		<< " "
				<< (int)rgb.b		<< " "
				<< status 			<< " "
				<< direction.x 		<< " "
				<< direction.y 		<< " ";
	}
	outputfile << std::endl;
	Simulator::Schedule(Seconds(0.2) ,&OutputRender);
}

Vector split(std::string& input, char delimiter)
{
    std::istringstream stream(input);
    std::string field;
    Vector result;
    std::getline(stream, field, delimiter);
    result.x = std::stod(field);
    std::getline(stream, field, delimiter);
    result.y = std::stod(field);

    return result;
}

std::vector<Vector> load_arrange(std::ifstream& ifs)
{
    std::string line;
    std::vector<Vector> arrange;
    while (std::getline(ifs, line)) {
	Vector v = split(line, ' ');
        arrange.push_back(v);
    }
    return arrange;
}
