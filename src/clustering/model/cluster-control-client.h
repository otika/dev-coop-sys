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

#ifndef CLUSTER_CONTROL_CLIENT_H_
#define CLUSTER_CONTROL_CLIENT_H_

#include <map>
#include "ns3/ptr.h"
#include "ns3/double.h"
#include "ns3/address.h"
#include "ns3/event-id.h"
#include "ns3/application.h"
#include "ns3/mobility-model.h"
#include "ns3/traced-callback.h"
#include "ns3/cluster-sap.h"
#include "ns3/cluster-header.h"

namespace ns3 {

class Socket;
class Address;

class ClusterControlClient: public Application {
public:
	struct Constants
	{
		// Constant
		constexpr static const double RANGE = 2.5;	// Communication Range inner Cluster
		constexpr static const int DISTRO_MAP_SIZE = 5; // must be bigger than RANGE
		constexpr static const int DISTRO_MAP_SCALE = 1.0;
		constexpr static const double PROPAGATION_THETA = M_PI / 2;
	};

	enum NodeStatus{
        CLUSTER_INITIALIZATION = 0,
        CLUSTER_HEAD_ELECTION,
        CLUSTER_FORMATION,
        CLUSTER_UPDATE,
		EXCHANGE_DISTRO_MAP,
		DECIDE_PROPAGATION_PARAM,
		PROPAGATION_READY,
		PROPAGATION_RUNNING,
		PROPAGATION_COMPLETE,
        CLUSTER_STATES
	};

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
    static TypeId GetTypeId (void);

	ClusterControlClient();
    virtual ~ClusterControlClient ();

	/**
	 * \return pointer to listening socket
	 */
    Ptr<Socket> GetListeningSocket (void) const;

	/**
	 * \brief Return a pointer to associated socket.
	 * \return pointer to associated socket
	 */
    Ptr<Socket> GetSocket (void) const;

   /**
   * \brief Specify application start time
   * \param start Start time for this application,
   *        relative to the current simulation time.
   *
   * Applications start at various times in the simulation scenario.
   * The Start method specifies when the application should be
   * started.  The application subclasses should override the
   * private "StartApplication" method defined below, which is called at the
   * time specified, to cause the application to begin.
   */
    void SetClusteringStartTime(Time start);

   /**
   * \brief Specify application stop time
   * \param stop Stop time for this application, relative to the
   *        current simulation time.
   *
   * Once an application has started, it is sometimes useful
   * to stop the application.  The Stop method specifies when an
   * application is to stop.  The application subclasses should override
   * the private StopApplication method, to be notified when that
   * time has come.
   */
    void SetClusteringStopTime(Time stop);

	/**
	 * \return Information about node and cluster
	 */
	const ClusterSap::NeighborInfo GetCurrentMobility() const;

	const ClusterControlClient::NodeStatus GetNodeStatus() const;

	/**
	 * \brief Specify starting node
	 */
	void SetStartingNode(bool isStartingNode = true);

	/**
	 * \brief Specify propagation direction and velocity
	 */
	void SetBasePropagationVector(Vector vector);

protected:
    virtual void DoDispose (void);

private:
   	/// inherited from Application base class.
    virtual void StartApplication (void);    // Called at time specified by Start
    virtual void StopApplication (void);     // Called at time specified by Stop
    void StartClustering (void);    // Called at time specified by Start
    void StopClustering (void);     // Called at time specified by Stop

    void ExchangeDistroMap (void);    // Called at time specified by Start of ExchangeDistroMap
    void DecidePropagationParam(void);
    void TransmitPropagationDirection(uint64_t id, Vector propVector);
    uint64_t FindNodeByPosition(Vector pos);
    Time CalcPropagationDelay(Vector source, Vector destination, Vector direction);
    bool IsInSector(Vector source, Vector destination, Vector direction, double radius, double theta = M_PI);

    void ScheduleInterNodePropagation();
    void StartNodePropagation();
    void StopNodePropagation();

    void SendTo(uint64_t id, Ptr<Packet> packet, bool *ack = 0);

    virtual void DoInitialize (void);

    void StartListeningLocal (void);	// Called from StartApplication()
    void StopListeningLocal (void);	// Called from StopApplication()

    void ConnectSocketInterCH(void);
    void DisconnectSocketInterCH(void);

    /**
     * \brief Print sent/received packets statistics.
     */
    void PrintStatistics (std::ostream &os);


    //!< Receive locally
	/**
	 * \brief Handle a packet received by the application
	 * \param socket the receiving socket
	 */
    void HandleRead (Ptr<Socket> socket);

    //!< Receive locally
	/**
	 * \brief Handle a packet received by the application
	 * \param socket the receiving socket
	 */
    void HandleReadInterCluster (Ptr<Socket> socket);

	/**
	 * \brief Handle an incoming connection
	 * \param socket the incoming connection socket
	 * \param from the address the connection is from
	 */
    void HandleAccept (Ptr<Socket> socket, const Address& from);
	/**
	 * \brief Handle an connection close
	 * \param socket the connected socket
	 */
    void HandlePeerClose (Ptr<Socket> socket);
	/**
	 * \brief Handle an connection error
	 * \param socket the connected socket
	 */
    void HandlePeerError (Ptr<Socket> socket);


    //!< Send locally
	/**
	 * \brief Schedule the next packet transmission
	 * \param dt time interval between packets.
	 */
    void ScheduleTransmit (Time dt);

	/**
	 * \brief Send a packet
	 */
    void Send (void);

	/**
	 * \brief Handle a Connection Succeed event
	 * \param socket the connected socket
	 */
    void ConnectionSucceeded (Ptr<Socket> socket);

	/**
	 * \brief Handle a Connection Failed event
	 * \param socket the not connected socket
	 */
    void ConnectionFailed (Ptr<Socket> socket);

    void ConnectionClosed(Ptr<Socket> socket);

    void ConnectionClosedWithError(Ptr<Socket> socket);

    //!< Cluster Functionality
    /**
	 * \brief Check if the speed of the node is the lowest in the cluster
	 * \return boolean
	 */
    bool HasMaxId (void);

    /**
     * \brief Update status
     */
    void UpdateNeighbors (void);

	/**
	 * \brief Start the clusterHead election procedure
	 */
    void InitiateCluster (void);

	/**
	 * \brief Start the clusterHead election procedure
	 */
    void FormCluster (void);

	/**
	 * \brief Send Initiate Cluster message
	 */
	void ScheduleInitiateCluster (Time dt);

	/**
	 * \brief Send Initiate Cluster message
	 */
    //void SendInitiateCluster (void);

	/**
	 * \brief Report the status of the node
	 */
    void StatusReport (void);

    /**
     * @brief Update Neighbor's lists according to latest Timestamps
     */
    void UpdateNeighborList (void);

    /**
     * \brief return the id of the suitable CH
     */
    uint64_t MergeCheck (void);

    /**
     * @brief Acquire current mobility info
     */
    void AcquireMobilityInfo (void);


    /**
     * @brief Update distribution map of node in the cluster
     */
    void UpdateDistroMap(void);

    //!< Incident
    /**
     * @brief CreateIncidentSocket
     */
    void CreateIncidentSocket (Address from);

    /**
     * @brief RemoveIncidentSocket
     */
    void RemoveIncidentSocket (void);

    /**
     * @brief ScheduleIncidentEvent
     */
    void ScheduleIncidentEvent (Time dt);

    /**
     * @brief SendIncident
     */
    void SendIncident (void);

    /**
     * \brief Handle a Connection Succeed event
     * \param socket the connected socket
     */
    void ConnectionCHSucceeded (Ptr<Socket> socket);

    /**
     * \brief Handle a Connection Failed event
     * \param socket the not connected socket
     */
    void ConnectionCHFailed (Ptr<Socket> socket);


    uint32_t m_formationCounter;            //!< Count the cluster formation messages
    uint32_t m_changesCounter;              //!< Count the changes of the state of the vehicle
    double m_incidentWindow;                //!< Time Window for incident event generation

    /* Incident Report */
    Time m_incidentTimestamp;               //!< Timestamp of last incident report
    TypeId m_tidIncident;   				//!< Type of the socket used
    Address m_peerIncident;         		//!< Peer address
    double m_overalDelay;                   //!< The aggregated delay of the incident messages
    uint32_t m_incidentCounter;             //!< Counter for sent incident packets
    Ptr<Socket> m_socketIncident;           //!< Socket with Cluster Head to send incidents
    EventId m_sendIncidentEvent;            //!< Event id of scheduled incident

	/* Receive Socket */
	TypeId m_tidListening;          		//!< Protocol TypeId
	Address m_peerListening;       	 		//!< Local address to bind to
	Ptr<Socket> m_socketListening;      	//!< Listening socket

	Ptr<Socket> m_socketListeningInterCH;

	/* Send Socket */
	TypeId m_tid;          					//!< Type of the socket used
	Address m_peer;        	 				//!< Peer address
	EventId m_sendEvent;    				//!< Event id of pending "send packet" event
	Ptr<Socket> m_socket;       			//!< Associated socket

    Time m_interval; 						//!< Packet inter-send time
    double m_timeWindow;                    //!< Time Window for cluster formation
	uint32_t m_pktSize;      				//!< Size of packets
    uint32_t m_sentCounter; 				//!< Counter for sent packets
    uint32_t m_recvCounter;					//!< Counter for receive packets

    TracedCallback<Ptr<const Packet> > m_txTrace;
	TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;

	Time m_clusteringStartTime;         //!< The simulation time that clustering of the application will start
    Time m_clusteringStopTime;          //!< The simulation time that clustering of the application will end
    EventId m_clusteringStartEvent;     //!< The event that will fire at m_startTime to start clustering of the application
    EventId m_clusteringStopEvent;      //!< The event that will fire at m_stopTime to end clustering of the application
    EventId m_exchangeDistroMapEvent;

    EventId m_neighborsListUpdateEvent;


    uint32_t m_maxUes;                      //!< maximun number of ues
    double m_minimumTdmaSlot;               //!< the minimum tdma slot
    double m_clusterTimeMetric;             //!< the timeslot for the node to schedule transmission

	/// Node information for cluster formation
    EventId m_chElectionEvent;                          //!< Event id of pending "CH Request" event
    ClusterSap::NeighborInfo m_currentMobility;
    Ptr<MobilityModel> m_mobilityModel;
    enum NodeStatus m_status;                           //!< Node Degree

    std::map<uint64_t, ClusterSap::NeighborInfo> m_clusterList;     //!< Cluster Member's List
    std::map<uint64_t, ClusterSap::NeighborInfo> m_neighborList;    //!< Neighbor's List
    std::map<uint64_t, ClusterSap::NeighborInfo> m_neighborClusterList; //!< Neighbor Cluster's List

    std::map<uint64_t, Ptr<Socket>> m_neighborClustersSocket; //!< Socket List of connected to Neighbor Cluster's CH

    float m_distroMap[Constants::DISTRO_MAP_SIZE * Constants::DISTRO_MAP_SIZE];
    std::map<uint64_t, std::vector<float>> m_neighborDistroMap;
    std::map<uint64_t, bool> m_ackDistroMap;

    uint64_t m_firstPropagationStartNodeId = std::numeric_limits<uint64_t>::max();
    Vector m_basePropagationDirection = Vector(0, 0, 0);
    Vector m_propagationDirection = Vector(0, 0, 0);
    Time m_firstPropagationStartingTime = Time::Max();
    Time m_propagationStartTime = Time::Max();

   	std::vector<EventId> m_sendingInterClusterPropagationEvent;
	std::map<uint64_t, bool> m_ackInterClusterPropagation;

	EventId m_interNodePropagationEvent;

    //!< Incident Info
    ClusterSap::IncidentInfo m_incidentInfo;

	TracedCallback<Ptr<const ClusterControlClient> > m_statusTrace;
};

} // namespace ns3

#endif /* CLUSTER_CONTROL_CLIENT_H_ */
