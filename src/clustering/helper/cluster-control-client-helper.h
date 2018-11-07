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

#ifndef CLUSTER_CONTROL_CLIENT_HELPER_H
#define CLUSTER_CONTROL_CLIENT_HELPER_H

#include <stdint.h>
#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/cluster-control-client.h"

namespace ns3 {

/**
 * \brief A helper to make it easier to instantiate an ns3::ClusterControlClient
 * on a set of nodes.
 */
class ClusterControlClientHelper {
public:

    /**
     * @brief Create an ClusterControllClientHelper to make it easier to work with ClusterControllClient
     * @param protocolLocal the name of the protocol to use to send traffic
     * @param addressLocal the address of the node(s) to send traffic
     * @param protocolListeningLocal the name of the protocol to use to receive traffic
     * @param addressListeningLocal the address to receive traffic
     * @param mobilityModel a pointer to the mobility model of the node
     * @param timeFactor
     */
    ClusterControlClientHelper(std::string protocolLocal, Address addressLocal,
        std::string protocolListeningLocal, Address addressListeningLocal,
        Ptr<MobilityModel> mobilityModel, double timeFactor, uint32_t maxUes, double minimumTdmaSlot, double clusterTimeMetric);

	/**
	 * Helper function used to set the underlying application attributes.
	 *
	 * \param name the name of the application attribute to set
	 * \param value the value of the application attribute to set
	 */
	void SetAttribute(std::string name, const AttributeValue &value);

	/**
	 * Install an ns3::ClusterControlClient on each node of the input container
	 * configured with all the attributes set with SetAttribute.
	 *
	 * \param c NodeContainer of the set of nodes on which an ClusterControlClient
	 * will be installed.
	 * \returns Container of Ptr to the applications installed.
	 */
	ApplicationContainer Install(NodeContainer c) const;

	/**
	 * Install an ns3::ClusterControlClient on the node configured with all the
	 * attributes set with SetAttribute.
	 *
	 * \param node The node on which an ClusterControlClient will be installed.
	 * \returns Container of Ptr to the applications installed.
	 */
	ApplicationContainer Install(Ptr<Node> node) const;

	/**
	 * Install an ns3::ClusterControlClient on the node configured with all the
	 * attributes set with SetAttribute.
	 *
	 * \param nodeName The node on which an ClusterControlClient will be installed.
	 * \returns Container of Ptr to the applications installed.
	 */
	ApplicationContainer Install(std::string nodeName) const;

private:
	/**
	 * Install an ns3::ClusterControlClient on the node configured with all the
	 * attributes set with SetAttribute.
	 *
	 * \param node The node on which an ClusterControlClient will be installed.
	 * \returns Ptr to the application installed.
	 */
	Ptr<Application> InstallPriv(Ptr<Node> node) const;

    ObjectFactory m_factory;
};

} // namespace ns3

#endif /* CKUSTER_CONTROL_CLIENT_HELPER_H */
