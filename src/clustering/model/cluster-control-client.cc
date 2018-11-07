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

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/socket.h"
#include "ns3/string.h"
#include "ns3/packet.h"
#include "ns3/address.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include "ns3/simulator.h"
#include "ns3/udp-socket.h"
#include "ns3/address-utils.h"
#include "ns3/socket-factory.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/trace-source-accessor.h"
#include "cluster-control-client.h"

#include "ns3/random-variable-stream.h"

#include <cmath>
#include <limits>

//#define CLUSTER_CONTROL_CLIENT_DEBUG

namespace ns3 {

static const std::string ClusterStatusName[ClusterControlClient::CLUSTER_STATES] =
		{ "CLUSTER_INITIALIZATION", "CLUSTER_HEAD_ELECTION",
				"CLUSTER_FORMATION", "CLUSTER_UPDATE" };

static const std::string & ToString(ClusterControlClient::NodeStatus status) {
	return ClusterStatusName[status];
}

static const std::string IncidentName[ClusterSap::INCIDENT_STATES] = {
		"EMERGENCY_EVENT", "NOTIFICATION_EVENT" };

static const std::string & ToString(ClusterSap::IncidentType incidentType) {
	return IncidentName[incidentType];
}

static const std::string DegreeName[ClusterSap::DEGREE_STATES] = { "STANDALONE",
		"CH", "CM" };

static const std::string & ToString(ClusterSap::NodeDegree nodeDegree) {
	return DegreeName[nodeDegree];
}

NS_LOG_COMPONENT_DEFINE("ClusterControlClient");
NS_OBJECT_ENSURE_REGISTERED(ClusterControlClient);

TypeId ClusterControlClient::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::ClusterControlClient").SetParent<Application>().AddConstructor<
					ClusterControlClient>().AddAttribute("ListeningLocal",
					"The Address on which to Bind the rx socket.",
					AddressValue(),
					MakeAddressAccessor(&ClusterControlClient::m_peerListening),
					MakeAddressChecker()).AddAttribute("ProtocolListeningLocal",
					"The type id of the protocol to use for the rx socket.",
					TypeIdValue(UdpSocketFactory::GetTypeId()),
					MakeTypeIdAccessor(&ClusterControlClient::m_tidListening),
					MakeTypeIdChecker()).AddTraceSource("RxLocal",
					"A packet has been received",
					MakeTraceSourceAccessor(&ClusterControlClient::m_rxTrace))

			.AddAttribute("IncidentWindow", "The incident time window",
					DoubleValue(4),
					MakeDoubleAccessor(&ClusterControlClient::m_incidentWindow),
					MakeDoubleChecker<double>()).AddAttribute(
					"ClusterTimeMetric", "The maximun size of the TDMA window",
					DoubleValue(0.5),
					MakeDoubleAccessor(
							&ClusterControlClient::m_clusterTimeMetric),
					MakeDoubleChecker<double>()).AddAttribute("MinimumTdmaSlot",
					"The maximun size of the TDMA window", DoubleValue(0.001),
					MakeDoubleAccessor(
							&ClusterControlClient::m_minimumTdmaSlot),
					MakeDoubleChecker<double>()).AddAttribute("MaxUes",
					"The maximun size of ues permitted", UintegerValue(100),
					MakeUintegerAccessor(&ClusterControlClient::m_maxUes),
					MakeUintegerChecker<uint32_t>(1)).AddAttribute("PacketSize",
					"The size of packets sent in on state", UintegerValue(512),
					MakeUintegerAccessor(&ClusterControlClient::m_pktSize),
					MakeUintegerChecker<uint32_t>(1)).AddAttribute("TimeWindow",
					"The time to wait between packets", DoubleValue(1.0),
					MakeDoubleAccessor(&ClusterControlClient::m_timeWindow),
					MakeDoubleChecker<double>()).AddAttribute("Interval",
					"The time to wait between packets", TimeValue(Seconds(1.0)),
					MakeTimeAccessor(&ClusterControlClient::m_interval),
					MakeTimeChecker()).AddAttribute("SendingLocal",
					"The address of the destination", AddressValue(),
					MakeAddressAccessor(&ClusterControlClient::m_peer),
					MakeAddressChecker()).AddAttribute("ProtocolSendingLocal",
					"The type of protocol for the tx socket.",
					TypeIdValue(UdpSocketFactory::GetTypeId()),
					MakeTypeIdAccessor(&ClusterControlClient::m_tid),
					MakeTypeIdChecker()).AddAttribute("MobilityModel",
					"The mobility model of the node.", PointerValue(),
					MakePointerAccessor(&ClusterControlClient::m_mobilityModel),
					MakePointerChecker<MobilityModel>()).AddTraceSource(
					"TxLocal", "A new packet is created and is sent",
					MakeTraceSourceAccessor(&ClusterControlClient::m_txTrace)).AddTraceSource(
					"Status", "Status chenged",
					MakeTraceSourceAccessor(
							&ClusterControlClient::m_statusTrace),
					"ns3::V2ClusterControlClient::StatusTraceCallback");
	return tid;
}

// Public Members
ClusterControlClient::ClusterControlClient() {
	NS_LOG_FUNCTION(this);

	m_socket = 0;
	m_socketIncident = 0;
	m_socketListening = 0;

	m_overalDelay = 0;
	m_sentCounter = 0;
	m_changesCounter = 0;
	m_incidentCounter = 0;
	m_formationCounter = 0;

	m_sendEvent = EventId();
	m_chElectionEvent = EventId();
}

ClusterControlClient::~ClusterControlClient() {
	NS_LOG_FUNCTION(this);

	m_socket = 0;
	m_socketIncident = 0;
	m_socketListening = 0;

	m_overalDelay = 0;
	m_sentCounter = 0;
	m_changesCounter = 0;
	m_incidentCounter = 0;
	m_formationCounter = 0;
}

void ClusterControlClient::PrintStatistics(std::ostream &os) {
	if (m_incidentCounter == 0) {
		m_incidentCounter = 1;       // Avoid division with zero
	}

	os << "***********************" << std::endl << "  - Cluster Metrics -  "
			<< std::endl << "Node:" << m_currentMobility.imsi
			<< " Sent overal: " << m_sentCounter << " Packets." << std::endl
			<< " Formation Messages: " << m_formationCounter << std::endl
			<< " Status Changes: " << m_changesCounter << std::endl
			<< "-----------------------" << std::endl
			<< "  - Insident Metrics -  " << std::endl
			<< "Mean delay of incidents delivered: "
			<< (double) m_overalDelay / m_incidentCounter << std::endl
			<< "***********************" << std::endl;
}

// Protected Members
void ClusterControlClient::DoDispose(void) {
	NS_LOG_FUNCTION(this);

	m_socket = 0;
	m_socketListening = 0;

	// chain up
	Application::DoDispose();
}

void ClusterControlClient::StartApplication(void) {
	NS_LOG_FUNCTION(this);
	m_status = ClusterControlClient::CLUSTER_INITIALIZATION;

	// Create the socket if not already
	if (!m_socket) {
		m_socket = Socket::CreateSocket(GetNode(), m_tid);
		if (Inet6SocketAddress::IsMatchingType(m_peer)) {
			m_socket->Bind6();
		} else if (InetSocketAddress::IsMatchingType(m_peer)
				|| PacketSocketAddress::IsMatchingType(m_peer)) {
			m_socket->Bind();
		}
		m_socket->Connect(m_peer);
		m_socket->SetAllowBroadcast(true);
		m_socket->ShutdownRecv();

		m_socket->SetConnectCallback(
				MakeCallback(&ClusterControlClient::ConnectionSucceeded, this),
				MakeCallback(&ClusterControlClient::ConnectionFailed, this));
	}

	if (m_maxUes > 10000) {
		NS_FATAL_ERROR("Error: Maximum number of ues is 100.");
	}

	StartListeningLocal();
	ScheduleTransmit(Seconds(m_timeWindow));
	AcquireMobilityInfo();

	Simulator::Schedule(Seconds(1.5 + m_minimumTdmaSlot * m_maxUes), &ClusterControlClient::UpdateNeighborList, this);
}

void ClusterControlClient::StartListeningLocal(void) // Called at time specified by Start
		{
	NS_LOG_FUNCTION(this);

	m_clusterList.clear();
	m_2rStableList.clear();
	// Create the socket if not already
	if (!m_socketListening) {
		m_socketListening = Socket::CreateSocket(GetNode(), m_tidListening);
		m_socketListening->Bind(m_peerListening);
		m_socketListening->Listen();
		m_socketListening->ShutdownSend();
		if (addressUtils::IsMulticast(m_peerListening)) {
			Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket>(
					m_socketListening);
			if (udpSocket) {
				// equivalent to setsockopt (MCAST_JOIN_GROUP)
				udpSocket->MulticastJoinGroup(0, m_peerListening);
			} else {
				NS_FATAL_ERROR("Error: joining multicast on a non-UDP socket");
			}
		}
	}

	m_socketListening->SetRecvCallback(
			MakeCallback(&ClusterControlClient::HandleRead, this));
	m_socketListening->SetAcceptCallback(
			MakeNullCallback<bool, Ptr<Socket>, const Address &>(),
			MakeCallback(&ClusterControlClient::HandleAccept, this));
	m_socketListening->SetCloseCallbacks(
			MakeCallback(&ClusterControlClient::HandlePeerClose, this),
			MakeCallback(&ClusterControlClient::HandlePeerError, this));
}

void ClusterControlClient::StopApplication(void) // Called at time specified by Stop
		{
	NS_LOG_FUNCTION(this);

	if (m_socket != 0) {
		m_socket->Close();
		m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket> >());
		m_socket = 0;
	} else {
		NS_LOG_WARN(
				"ClusterControlClient found null socket to close in StopApplication");
	}
	Simulator::Cancel(m_sendEvent);
	Simulator::Cancel(m_sendIncidentEvent);
	StopListeningLocal();
	// PrintStatistics(std::cout);
}

void ClusterControlClient::StopListeningLocal(void) // Called at time specified by Stop
		{
	NS_LOG_FUNCTION(this);
	if (m_socketListening) {
		m_socketListening->Close();
		m_socketListening->SetRecvCallback(
				MakeNullCallback<void, Ptr<Socket> >());
		m_socketListening = 0;
	}
}

Ptr<Socket> ClusterControlClient::GetListeningSocket(void) const {
	NS_LOG_FUNCTION(this);
	return m_socketListening;
}

Ptr<Socket> ClusterControlClient::GetSocket(void) const {
	NS_LOG_FUNCTION(this);
	return m_socket;
}

const ClusterSap::NeighborInfo ClusterControlClient::GetCurrentMobility() const {
	const ClusterSap::NeighborInfo info = m_currentMobility;
	return info;
}

// Private Members
void ClusterControlClient::HandleRead(Ptr<Socket> socket) {
	NS_LOG_FUNCTION(this << socket);
	Ptr<Packet> packet;
	Address from;
	while ((packet = socket->RecvFrom(from))) {
		if (packet->GetSize() == 0) { //EOF
			break;
		}

		ClusterSap::NeighborInfo prev_mobility = m_currentMobility;

		PacketMetadata::ItemIterator metadataIterator = packet->BeginItem();
		PacketMetadata::Item item;
		while (metadataIterator.HasNext()) {
			item = metadataIterator.Next();

			// ClusterInfoHeader
			if (item.tid.GetName() == "ns3::ClusterInfoHeader") {

				ClusterSap::ClusterSap::NeighborInfo otherInfo;
				ClusterInfoHeader clusterInfo;
				packet->RemoveHeader(clusterInfo);
				otherInfo = clusterInfo.GetMobilityInfo();

				//!< Update 2rStable and cluster List
				std::map<uint64_t, ClusterSap::NeighborInfo>::iterator it2r = m_2rStableList.find(otherInfo.imsi);

				if (it2r == m_2rStableList.end()) {
					m_2rStableList.insert(std::map<uint64_t, ClusterSap::NeighborInfo>::value_type(otherInfo.imsi, otherInfo));
				} else {
					it2r->second = otherInfo;
				}
				// ClusterInfoHeader @ CLUSTER_UPDATE
				if (m_status == ClusterControlClient::CLUSTER_UPDATE || m_status == ClusterControlClient::CLUSTER_HEAD_ELECTION ) {
					if (m_currentMobility.degree == ClusterSap::CH) {
						if (otherInfo.clusterId == m_currentMobility.imsi) {
							std::map<uint64_t, ClusterSap::NeighborInfo>::iterator itc = m_clusterList.find(otherInfo.imsi);
							if (itc == m_clusterList.end()) {
								m_clusterList.insert(
										std::map<uint64_t, ClusterSap::NeighborInfo>::value_type(otherInfo.imsi, otherInfo));
							}
							else {
								//!< Update UE Info
								itc->second = otherInfo;
							}
						}
						else {
							//!< Check Cluster Merge
							if (m_clusterList.size() == 0) {
								uint64_t potentialCH = MergeCheck();
								if (m_2rStableList.count(potentialCH) > 0 && potentialCH != UINT64_MAX) {
									ClusterSap::NeighborInfo potential = m_2rStableList.find(potentialCH)->second;

									if (m_currentMobility.imsi < potential.imsi) {
										NS_LOG_DEBUG("[HandleRead] => Node:" << m_currentMobility.imsi << " - merge with node:" << potential.imsi);
										m_currentMobility.degree = ClusterSap::CM;
										m_currentMobility.clusterId = potential.imsi;
										m_changesCounter++;

										RemoveIncidentSocket();
										CreateIncidentSocket(from);
									}
								}
							}
						}
					}
					else if (m_currentMobility.degree == ClusterSap::STANDALONE) {
						uint64_t potentialCH = MergeCheck();
						if (m_2rStableList.count(potentialCH) > 0 && potentialCH != UINT64_MAX) {
							ClusterSap::NeighborInfo potential =
									m_2rStableList.find(potentialCH)->second;

							NS_LOG_DEBUG(
									"[HandleRead] => Node:" << m_currentMobility.imsi << " - Attach to new CH node:" << potential.imsi);
							m_currentMobility.degree = ClusterSap::CM;
							m_currentMobility.clusterId = potential.imsi;
							m_changesCounter++;

							RemoveIncidentSocket();
							CreateIncidentSocket(from);
						} else {
							NS_LOG_DEBUG(
									"[HandleRead] => To Become new CH: " << m_currentMobility.imsi);

							NS_LOG_DEBUG("Node Status: " << ToString(m_status));

#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
							std::cout << "id: " << m_currentMobility.imsi << " become CH in HandleRead()@CLUSTER_UPDATE" << std::endl;
#endif


							m_currentMobility.degree = ClusterSap::CH;
							m_currentMobility.clusterId = m_currentMobility.imsi;
							m_changesCounter++;
							Simulator::Schedule(Seconds(0.), &ClusterControlClient::UpdateNeighbors, this);
						}
					}
				}
			}

			else if (item.tid.GetName() == "ns3::InitiateClusterHeader") {
				if (m_status == ClusterControlClient::CLUSTER_INITIALIZATION) {
					m_status = ClusterControlClient::CLUSTER_HEAD_ELECTION;

					//!< Parse InitiateClusterHeader Info
					InitiateClusterHeader initiateCluster;
					packet->RemoveHeader(initiateCluster);
//					m_currentMobility.clusterId = initiateCluster.GetClusterId();
//					m_currentMobility.degree = ClusterSap::CM;
					m_currentMobility.clusterId = m_currentMobility.imsi;
					m_currentMobility.degree = ClusterSap::CH;

#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
					std::cout << "id: " << m_currentMobility.imsi << " become CH in HandleRead()@InitiateCluster " << m_currentMobility.clusterId << std::endl;
#endif

					std::map<uint64_t, ClusterSap::NeighborInfo>::iterator foundIt = m_2rStableList.find(initiateCluster.GetClusterId());
					if ((foundIt != m_2rStableList.end())) {

						//!< Suitability check [should be applied to r-distance neighbors]
						double waitingTime = SuitabilityCheck();
						NS_LOG_UNCOND("[HandleRead] => NodeId: " << m_currentMobility.imsi << " WaitingTime is: " << waitingTime);

						//!< Handle chElection Event
						m_chElectionEvent = Simulator::Schedule(Seconds(waitingTime), &ClusterControlClient::FormCluster, this);

						// Ptr<UniformRandomVariable> randomIncident = CreateObject<UniformRandomVariable> (); //Nova declaração, conforme atualização da variavel RandomVariableStream
						// ScheduleIncidentEvent (Seconds (randomIncident->GetValue ((int)waitingTime + 1, m_incidentWindow)));

					} else {
						/// Not in 2rStableList
						m_status = ClusterControlClient::CLUSTER_INITIALIZATION;
					}
				} else {
					//!< Process only the first request and ignore the rest
					NS_LOG_DEBUG("[HandleRead] => NodeId: " << m_currentMobility.imsi << " Ignore further requests for CH suitability...");
				}
			}

			else if (item.tid.GetName() == "ns3::FormClusterHeader") {

				NS_ASSERT((m_status != ClusterControlClient::CLUSTER_HEAD_ELECTION)
								|| (m_status != ClusterControlClient::CLUSTER_FORMATION));

				FormClusterHeader formCluster;
				packet->RemoveHeader(formCluster);

				//!< Update 2rStable and cluster List
				ClusterSap::NeighborInfo otherInfo = formCluster.GetMobilityInfo();
				std::map<uint64_t, ClusterSap::NeighborInfo>::iterator it2r = m_2rStableList.find(otherInfo.imsi);
				if (it2r == m_2rStableList.end()) {
					NS_LOG_DEBUG("[HandleRead] => Node:" << m_currentMobility.imsi << " Insert packet:" << otherInfo.imsi);
					m_2rStableList.insert(std::map<uint64_t, ClusterSap::NeighborInfo>::value_type(otherInfo.imsi, otherInfo));
				}
				else {
					it2r->second = otherInfo;
				}

				std::map<uint64_t, ClusterSap::NeighborInfo>::iterator it = m_2rStableList.find(formCluster.GetMobilityInfo().clusterId);

				if (it != m_2rStableList.end()) {
					if (m_status == ClusterControlClient::CLUSTER_HEAD_ELECTION) {

						//m_status = ClusterControlClient::CLUSTER_FORMATION;
						m_chElectionEvent.Cancel();
						NS_LOG_DEBUG("[HandleRead] => NodeId: " << m_currentMobility.imsi << " connected to cluster: " << formCluster.GetMobilityInfo().clusterId);

						//!< Apply received CH info
						m_status = ClusterControlClient::CLUSTER_UPDATE;
						m_currentMobility.degree = ClusterSap::CM;
						m_currentMobility.clusterId = formCluster.GetMobilityInfo().clusterId;
						ScheduleTransmit(Seconds((m_timeWindow)));

//						double updateTime = (int) Simulator::Now().GetSeconds() + 1.5;
//							Simulator::Schedule(Seconds(updateTime - Simulator::Now().GetSeconds()),
//									&ClusterControlClient::UpdateNeighborList, this);

						CreateIncidentSocket(from);
					}
					else if (m_status == ClusterControlClient::CLUSTER_FORMATION) {
						NS_LOG_DEBUG("[HandleRead] => NodeId: " << m_currentMobility.imsi << " Node is already a Cluster Member.");
					}
				} else {
					/// Not in 2rStableList
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
					std::cout << "id:" << m_currentMobility.imsi << " Get FormClusterHeader from "
							<< formCluster.GetMobilityInfo().imsi << ", "
							<< formCluster.GetMobilityInfo().clusterId << ", but it's unknown node" << std::endl;
#endif
					StatusReport();
				}
			}

			else if (item.tid.GetName() == "ns3::IncidentEventHeader") {

				IncidentEventHeader incidentHeader;
				packet->RemoveHeader(incidentHeader);

				if (m_incidentTimestamp.GetSeconds()
						== incidentHeader.GetTs().GetSeconds()) {

					/// Calculate Delay
					m_overalDelay += Simulator::Now().GetSeconds()
							- m_incidentTimestamp.GetSeconds();
					NS_LOG_UNCOND(
							"Node: " << m_currentMobility.imsi << " received back IncidentEventHeader:" << ". Incident Delay is: "
							<< Simulator::Now ().GetSeconds () - m_incidentTimestamp.GetSeconds () << " Seconds");
					NS_LOG_UNCOND(
							"--------------------------------------------------------------------------------------------");
				}

				if ((m_currentMobility.degree == ClusterSap::CH)
						&& (m_currentMobility.clusterId
								== incidentHeader.GetIncidentInfo().clusterId)) {

					//!< Broadcast Incident to Cluster Members
					Ptr<Packet> packet = Create<Packet>(0);
					packet->AddHeader(incidentHeader);

					m_socket->Send(packet);
					if (InetSocketAddress::IsMatchingType(m_peer)) {
						NS_LOG_UNCOND(
								"[Send] Broadcast Incident Message from " << m_currentMobility.imsi << "=> At time "
								<< Simulator::Now ().GetSeconds () <<" sent " << packet->GetSize () << " bytes to "
								<< InetSocketAddress::ConvertFrom(m_peer).GetIpv4 () << " port "
								<< InetSocketAddress::ConvertFrom (m_peer).GetPort () << " - Event Type is:"
								<< ToString (incidentHeader.GetIncidentInfo ().incidentType));
					} else if (Inet6SocketAddress::IsMatchingType(m_peer)) {
						NS_LOG_UNCOND(
								"[Send] Broadcast Incident Message from " << m_currentMobility.imsi << "=> At time "
								<< Simulator::Now ().GetSeconds () <<" sent " << packet->GetSize () << " bytes to "
								<< packet->GetSize () << " bytes to " << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6 ()
								<< " port " << Inet6SocketAddress::ConvertFrom (m_peer).GetPort ()
								<< " - Event Type is:" << ToString (incidentHeader.GetIncidentInfo ().incidentType));
					}
				}

			}
			m_rxTrace(packet, from);
		}

		// Callback for Update anim
		if ((prev_mobility.clusterId != m_currentMobility.clusterId)
				|| (prev_mobility.degree != m_currentMobility.degree)) {
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
			std::cout << "cluster changed@receive : " << m_currentMobility.imsi
					<< ", " << prev_mobility.clusterId << " -> "
					<< m_currentMobility.clusterId << ", "
					<< ToString(prev_mobility.degree) << " -> "
					<< ToString(m_currentMobility.degree) << std::endl;
#endif
			m_statusTrace(this);

		}
	}
}

void ClusterControlClient::CreateIncidentSocket(Address from) {
	NS_LOG_FUNCTION(this);

	//!< Create p2p socket with ClusterHead for incident event transmission
	Ipv4Address chAddress = InetSocketAddress::ConvertFrom(from).GetIpv4();
	uint16_t chPort = InetSocketAddress::ConvertFrom(m_peer).GetPort();
	m_peerIncident = Address(InetSocketAddress(chAddress, chPort));

	// Create the socket if not already
	if (!m_socketIncident) {
		m_socketIncident = Socket::CreateSocket(GetNode(), m_tid);
		if (Inet6SocketAddress::IsMatchingType(m_peerIncident)) {
			m_socketIncident->Bind6();
		} else if (InetSocketAddress::IsMatchingType(m_peerIncident)
				|| PacketSocketAddress::IsMatchingType(m_peerIncident)) {
			m_socketIncident->Bind();
		}
		m_socketIncident->Connect(m_peerIncident);
		m_socketIncident->SetAllowBroadcast(false);
		m_socketIncident->ShutdownRecv();

		m_socketIncident->SetConnectCallback(
				MakeCallback(&ClusterControlClient::ConnectionCHSucceeded,
						this),
				MakeCallback(&ClusterControlClient::ConnectionCHFailed, this));
	}
}

void ClusterControlClient::RemoveIncidentSocket(void) {
	NS_LOG_FUNCTION(this);

	if (m_socketIncident != 0) {
		m_socketIncident->Close();
		m_socketIncident->SetRecvCallback(
				MakeNullCallback<void, Ptr<Socket> >());
		m_socketIncident = 0;
	} else {
		NS_LOG_WARN("m_socketIncident null socket to close...");
	}
}

void ClusterControlClient::ConnectionCHSucceeded(Ptr<Socket> socket) {
	NS_LOG_FUNCTION(this << socket);
	NS_LOG_DEBUG("P2P Connection with CH Successful");
}

void ClusterControlClient::ConnectionCHFailed(Ptr<Socket> socket) {
	NS_LOG_FUNCTION(this << socket);
	NS_FATAL_ERROR("Error: joining CH socket");
}

uint64_t ClusterControlClient::MergeCheck(void) {
	uint64_t id = UINT64_MAX;
	double r = 100;              //!< transmition range
	double rt = 0.0;            //!< Suitability metric for CH  selection
	double boundary = 0.0;
	for (std::map<uint64_t, ClusterSap::NeighborInfo>::iterator itSearch =	m_2rStableList.begin(); itSearch != m_2rStableList.end();++itSearch) {
		ClusterSap::NeighborInfo node = itSearch->second;
		if (node.degree == ClusterSap::CH) {
			rt = r - std::sqrt((m_currentMobility.position.x - node.position.x)	* (m_currentMobility.position.x	- node.position.x)
									+ (m_currentMobility.position.y	- node.position.y) * (m_currentMobility.position.y - node.position.y));
			// rt = (r-fabs(m_currentMobility.position.x - node.position.x));
//			if (rt > boundary) {
//				id = itSearch->first;
//				boundary = rt;
//			}
			if (id == UINT64_MAX || itSearch->first > id) {
				id = itSearch->first;
				boundary = rt;
			}
		}
	}
	NS_LOG_DEBUG(
			"[MergeCheck] => Returned Id is: " << id << " - with Remaining Time(RT):" << boundary);
	return id;
}

void ClusterControlClient::AcquireMobilityInfo(void) {

	//!< Acquire current mobility stats
	m_currentMobility.ts = Simulator::Now();
	m_currentMobility.imsi = this->GetNode()->GetId();
	m_currentMobility.position = m_mobilityModel->GetPosition();
}

double ClusterControlClient::SuitabilityCheck(void) {
	return 0.01;
}

void ClusterControlClient::FormCluster(void) {
	m_status = ClusterControlClient::CLUSTER_FORMATION;
	ScheduleTransmit(Seconds(0.));
}

void ClusterControlClient::StatusReport(void) {

	NS_LOG_UNCOND(
			"\n\n-----------------------------------------------------------------------------");
	NS_LOG_UNCOND(
			"[StatusReport] => At time " << Simulator::Now ().GetSeconds () << "s node ["<< m_currentMobility.imsi << "] is: " << ToString (m_currentMobility.degree)
			<< " in Cluster: " << m_currentMobility.clusterId << " having  ===> \n position: " << m_currentMobility.position
			<< "\n last packet sent:" << m_currentMobility.ts.GetSeconds () << "s" << "\n Neighbors: " << m_2rStableList.size());
	NS_LOG_UNCOND(
			"----------------------------  2rStableList  ---------------------------------");
	for (std::map<uint64_t, ClusterSap::NeighborInfo>::iterator it =
			m_2rStableList.begin(); it != m_2rStableList.end(); ++it) {
		uint64_t id = it->first;
		ClusterSap::NeighborInfo node = it->second;
		NS_LOG_UNCOND(
				" * key: " << id << " clusterId: " << node.clusterId << " Degree:" << ToString (node.degree) << " Imsi:" << node.imsi << " Position:" << node.position
				<< " last packet sent:" << node.ts.GetSeconds() << "s");
	}

	NS_LOG_UNCOND(
			"-----------------------------  clusterList  ---------------------------------");
	for (std::map<uint64_t, ClusterSap::NeighborInfo>::iterator it =
			m_clusterList.begin(); it != m_clusterList.end(); ++it) {
		uint64_t id = it->first;
		ClusterSap::NeighborInfo node = it->second;
		NS_LOG_UNCOND(
				" * key: " << id << " clusterId: " << node.clusterId << " Degree:" << ToString (node.degree) << " Imsi:" << node.imsi << " Position:" << node.position);
	}
}

void ClusterControlClient::HandleAccept(Ptr<Socket> s, const Address& from) {
	NS_LOG_FUNCTION(this << s << from);
	s->SetRecvCallback(MakeCallback(&ClusterControlClient::HandleRead, this));
}

void ClusterControlClient::HandlePeerClose(Ptr<Socket> socket) {
	NS_LOG_FUNCTION(this << socket);
}

void ClusterControlClient::HandlePeerError(Ptr<Socket> socket) {
	NS_LOG_FUNCTION(this << socket);
}

void ClusterControlClient::ScheduleTransmit(Time dt) {
	NS_LOG_FUNCTION(this << dt);
	m_sendEvent = Simulator::Schedule(dt, &ClusterControlClient::Send, this);
	NS_LOG_DEBUG(
			"[ScheduleTransmit] => NodeId:" << m_currentMobility.imsi << " EventInfo:" << m_sendEvent.GetTs() << " status: " << ToString(m_status));
}

void ClusterControlClient::Send(void) {
	NS_LOG_FUNCTION(this);
	NS_LOG_DEBUG("[Send] => NodeId:" << m_currentMobility.imsi << " EventInfo:" << m_sendEvent.GetTs() << " status: " << ToString(m_status));

	if (!m_sendEvent.IsExpired()) {
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
		std::cout << m_currentMobility.imsi << " : Expired "
				<< ToString(m_currentMobility.degree) << ", "
				<< ToString(m_status) << std::endl; // for debug
#endif
		return;
	}
	NS_ASSERT(m_sendEvent.IsExpired());

	ClusterSap::NeighborInfo prev_mobility = m_currentMobility;

	switch (m_status) {
	case ClusterControlClient::CLUSTER_INITIALIZATION: {

		ClusterInfoHeader clusterInfo;
		clusterInfo.SetSeq(m_sentCounter);
		clusterInfo.SetMobilityInfo(m_currentMobility);

		Ptr<Packet> packet = Create<Packet>(0);
		packet->AddHeader(clusterInfo);
		m_txTrace(packet);
		m_socket->Send(packet);
		++m_sentCounter;
		m_formationCounter++;

		Simulator::Schedule(Seconds(m_minimumTdmaSlot * m_maxUes),
				&ClusterControlClient::InitiateCluster, this);
		break;
	}
	case ClusterControlClient::CLUSTER_HEAD_ELECTION: {

		InitiateClusterHeader initiateCluster;
		initiateCluster.SetSeq(m_sentCounter);
		initiateCluster.SetClusterId(m_currentMobility.imsi);

		Ptr<Packet> packet = Create<Packet>(0);
		packet->AddHeader(initiateCluster);
		m_txTrace(packet);
		m_socket->Send(packet);
		++m_sentCounter;
		m_formationCounter++;

		// Simulator::Schedule(Seconds(m_minimumTdmaSlot * m_maxUes), &ClusterControlClient::InitiateCluster, this);
		// std::cout << "scheduled InitiateCluster " << m_minimumTdmaSlot * m_maxUes << " later... : " << m_currentMobility.imsi << std::endl;

		double waitingTime = SuitabilityCheck();
		NS_LOG_UNCOND("[HandleRead] => NodeId: " << m_currentMobility.imsi << " WaitingTime is: " << waitingTime);
		//m_chElectionEvent = Simulator::Schedule(Seconds(waitingTime), &ClusterControlClient::FormCluster, this);

		Ptr<UniformRandomVariable> randomIncident = CreateObject<UniformRandomVariable>(); //Nova declaração, conforme atualização da variavel RandomVariableStream
		ScheduleIncidentEvent(Seconds(randomIncident->GetValue((int) waitingTime + 1, m_incidentWindow)));

		//UniformVariable randomIncident ((int)waitingTime + 1, m_incidentWindow); //Declaração antiga
		//ScheduleIncidentEvent (Seconds (randomIncident.GetValue ()));

		break;
	}
	case ClusterControlClient::CLUSTER_FORMATION: {
		AcquireMobilityInfo();
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
		std::cout << "id: " << m_currentMobility.imsi << " become CH in Send()@CLUSTER_FORMATION" << std::endl;
#endif
		m_currentMobility.degree = ClusterSap::CH;
		m_currentMobility.clusterId = m_currentMobility.imsi;

		FormClusterHeader clusterInfo;
		clusterInfo.SetSeq(m_sentCounter);
		clusterInfo.SetMobilityInfo(m_currentMobility);

		Ptr<Packet> packet = Create<Packet>(0);
		packet->AddHeader(clusterInfo);
		m_txTrace(packet);
		m_socket->Send(packet);
		++m_sentCounter;
		m_formationCounter++;

		Simulator::Schedule(Seconds(0.), &ClusterControlClient::UpdateNeighbors, this);

		break;
	}
	case ClusterControlClient::CLUSTER_UPDATE: {
		if (m_currentMobility.degree == ClusterSap::CH) {
			StatusReport();
		}
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
		std::cout << "Send update from " << m_currentMobility.imsi
				<< "(" << ToString(m_currentMobility.degree) << ")"
				<< " at " << Simulator::Now ().GetSeconds () <<"s" << std::endl;
#endif
		AcquireMobilityInfo();
		ClusterInfoHeader clusterInfo;
		clusterInfo.SetSeq(m_sentCounter);
		clusterInfo.SetMobilityInfo(m_currentMobility);

		Ptr<Packet> packet = Create<Packet>(0);
		packet->AddHeader(clusterInfo);
		m_txTrace(packet);
		m_socket->Send(packet);
		++m_sentCounter;

		ScheduleTransmit(m_interval); // loop
		break;
	}
	default:
		NS_LOG_DEBUG(
				"[Send] => Default Case NodeId [Transmit] " << m_currentMobility.imsi << " - Current Status: " << ToString(m_status));
		break;
	}
	if ((prev_mobility.clusterId != m_currentMobility.clusterId)
			|| (prev_mobility.degree != m_currentMobility.degree)) {
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
		std::cout << "cluster changed@send : " << m_currentMobility.imsi << ", "
				<< prev_mobility.clusterId << " -> "
				<< m_currentMobility.clusterId << ", "
				<< ToString(prev_mobility.degree) << " -> "
				<< ToString(m_currentMobility.degree) << std::endl;
#endif
		m_statusTrace(this);
	}
}

void ClusterControlClient::SendClusterInfo(void) {
	AcquireMobilityInfo();
	ClusterInfoHeader clusterInfo;
	clusterInfo.SetSeq(m_sentCounter);
	clusterInfo.SetMobilityInfo(m_currentMobility);

	Ptr<Packet> packet = Create<Packet>(0);
	packet->AddHeader(clusterInfo);
	m_txTrace(packet);
	m_socket->Send(packet);
	++m_sentCounter;

}

void ClusterControlClient::UpdateNeighbors(void) {
	m_status = ClusterControlClient::CLUSTER_UPDATE;
	ScheduleTransmit(m_interval);
}

void ClusterControlClient::InitiateCluster(void) {
	//std::cout << "Initiating from " << m_currentMobility.imsi << " ...";
	if (m_status == ClusterControlClient::CLUSTER_INITIALIZATION || m_status == ClusterControlClient::CLUSTER_HEAD_ELECTION) {
		if (HasMaxId()) {
			// retry
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
			std::cout << " become CH? " << ToString(m_status) << std::endl;
#endif
			Simulator::Schedule(Seconds(m_minimumTdmaSlot * m_maxUes), &ClusterControlClient::InitiateCluster, this);
		}

		else{
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
			std::cout << " become CM? Initiating to other Node now ... " << std::endl;
#endif
			m_status = ClusterControlClient::CLUSTER_HEAD_ELECTION;
			ScheduleTransmit(Seconds(m_minimumTdmaSlot * m_maxUes));
		}

	}
}

bool ClusterControlClient::HasMaxId(void) {

	uint64_t maxId = m_currentMobility.imsi;
	for (std::map<uint64_t, ClusterSap::NeighborInfo>::iterator it =
			m_2rStableList.begin(); it != m_2rStableList.end(); ++it) {
		ClusterSap::NeighborInfo value = it->second;
		if (value.imsi > maxId && value.degree == ClusterSap::STANDALONE) {
			maxId = value.imsi;
		}
	}
	if (maxId == m_currentMobility.imsi) {
		return true;
	}
	return false;
}

void ClusterControlClient::ConnectionSucceeded(Ptr<Socket> socket) {
	NS_LOG_FUNCTION(this << socket);
}

void ClusterControlClient::ConnectionFailed(Ptr<Socket> socket) {
	NS_LOG_FUNCTION(this << socket);
}

void ClusterControlClient::UpdateNeighborList(void) {

	ClusterSap::NeighborInfo prev_mobility = m_currentMobility;

	bool hasCH = false;

	//!< Update Neighbor's List according to Timestamps
	for (std::map<uint64_t, ClusterSap::NeighborInfo>::iterator it =
			m_2rStableList.begin(); it != m_2rStableList.end();) {
		uint64_t key = it->first;
		ClusterSap::NeighborInfo value = it->second;

		if(m_currentMobility.clusterId == value.imsi &&
				m_currentMobility.clusterId == value.clusterId &&
				value.degree == ClusterSap::CH){
			hasCH = true;
		}

		if (m_currentMobility.ts.GetSeconds() - value.ts.GetSeconds() > (3 * m_interval)) {
			m_2rStableList.erase(it++);
			if (value.imsi == m_currentMobility.clusterId) {
				// Lost CH
				//!< go to STANDALONE State
				m_currentMobility.clusterId = 0;
				m_currentMobility.degree = ClusterSap::STANDALONE;
				NS_LOG_DEBUG(
						"[UpdateNeighborList] => Go to STANDALONE state: " << m_currentMobility.imsi);
			}

			if (m_2rStableList.find(key) != m_2rStableList.end()) {
				// Lost Neighbor
				m_2rStableList.erase(key);
			}

			if (m_clusterList.find(key) != m_clusterList.end()) {
				// Lost CM
				m_clusterList.erase(key);
			}

			if ((m_2rStableList.size() == 0)&& (m_currentMobility.degree != ClusterSap::CH)) {
				// become CH
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
				std::cout << "id: " << m_currentMobility.imsi << " become CH in UpdateNeighborList()" << std::endl;
#endif
				m_currentMobility.degree = ClusterSap::CH;
				m_currentMobility.clusterId = m_currentMobility.imsi;
				m_changesCounter++;
			}
		} else {
			++it;
		}
	}

	if(m_currentMobility.degree == ClusterSap::CM && !hasCH){
		// Lost CH
		//!< go to STANDALONE State
		m_currentMobility.clusterId = UINT64_MAX;
		m_currentMobility.degree = ClusterSap::STANDALONE;
	}


	if ((prev_mobility.clusterId != m_currentMobility.clusterId)
			|| (prev_mobility.degree != m_currentMobility.degree)) {
#ifdef CLUSTER_CONTROL_CLIENT_DEBUG
		std::cout << "cluster changed@updateNeighborList : " << m_currentMobility.imsi << ", "
				<< prev_mobility.clusterId << " -> "
				<< m_currentMobility.clusterId << ", "
				<< ToString(prev_mobility.degree) << " -> "
				<< ToString(m_currentMobility.degree) << std::endl;
#endif
		m_statusTrace(this);
	}

	Simulator::Schedule(m_interval, &ClusterControlClient::UpdateNeighborList, this);
}

void ClusterControlClient::ScheduleIncidentEvent(Time dt) {
	NS_LOG_FUNCTION(this << dt);
	m_sendIncidentEvent = Simulator::Schedule(dt,
			&ClusterControlClient::SendIncident, this);
}

void ClusterControlClient::SendIncident(void) {

	ClusterSap::IncidentInfo incidentInfo;
	incidentInfo.clusterId = m_currentMobility.clusterId;
	incidentInfo.incidentType = ClusterSap::EMERGENCY_EVENT;

	IncidentEventHeader incidentHeader;
	incidentHeader.SetIncidentInfo(incidentInfo);
	m_incidentTimestamp = incidentHeader.GetTs();

	Ptr<Packet> packet = Create<Packet>(0);
	packet->AddHeader(incidentHeader);

	if ((m_currentMobility.degree == ClusterSap::CH)
			|| (m_currentMobility.degree == ClusterSap::STANDALONE)) {

		//!< Broadcast Incident to Cluster Members directly
		m_socket->Send(packet);
		if (InetSocketAddress::IsMatchingType(m_peer)) {
			NS_LOG_UNCOND(
					"[Send] Broadcast Incident Message from " << m_currentMobility.imsi << "=> At time " << Simulator::Now ().GetSeconds () <<" sent " << packet->GetSize () << " bytes to "
					<< InetSocketAddress::ConvertFrom(m_peer).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (m_peer).GetPort ()
					<< " - Event Type is:" << ToString (incidentHeader.GetIncidentInfo ().incidentType));
		} else if (Inet6SocketAddress::IsMatchingType(m_peer)) {
			NS_LOG_UNCOND(
					"[Send] Broadcast Incident Message from " << m_currentMobility.imsi << "=> At time " << Simulator::Now ().GetSeconds () <<" sent " << packet->GetSize () << " bytes to "
					<< packet->GetSize () << " bytes to " << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6 () << " port " << Inet6SocketAddress::ConvertFrom (m_peer).GetPort ()
					<< " - Event Type is:" << ToString (incidentHeader.GetIncidentInfo ().incidentType));
		}
	} else {

		//!< Send Incident event to Cluster Head firstly
		m_socketIncident->Send(packet);
		m_incidentCounter++;
		if (InetSocketAddress::IsMatchingType(m_peerIncident)) {
			NS_LOG_UNCOND(
					"[Send] Incident Message => At time " << Simulator::Now ().GetSeconds () << "s node[IMSI] ["<< m_currentMobility.imsi <<"] sent " << packet->GetSize () << " bytes to "
					<< InetSocketAddress::ConvertFrom(m_peerIncident).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (m_peerIncident).GetPort ()
					<< " - Event Type is:" << ToString (incidentInfo.incidentType));
		} else if (Inet6SocketAddress::IsMatchingType(m_peerIncident)) {
			NS_LOG_INFO(
					"[Send] Incident Message => At time " << Simulator::Now ().GetSeconds () << "s node[IMSI] ["<< m_currentMobility.imsi <<"] sent " << packet->GetSize () << " bytes to "
					<< Inet6SocketAddress::ConvertFrom(m_peerIncident).GetIpv6 () << " port " << Inet6SocketAddress::ConvertFrom (m_peerIncident).GetPort ()
					<< " - Event Type is:" << ToString (incidentInfo.incidentType));
		}
	}

	//!< Schedule event generation in random time
	ScheduleIncidentEvent(Seconds(m_incidentWindow));

}

} // Namespace ns3
