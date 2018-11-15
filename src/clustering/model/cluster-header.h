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

#ifndef CLUSTER_HEADER_H
#define CLUSTER_HEADER_H

#include "ns3/log.h"
#include "ns3/header.h"
#include "cluster-sap.h"

namespace ns3 {

/**
 * \ingroup v2v
 * \class ClusterInfoHeader
 * \brief Packet header for V2vControlClient application.
 *
 * The header is made of a 64bits sequence number followed by
 * a mobility information structure.
 */
class ClusterInfoHeader: public Header {
public:

    ClusterInfoHeader();
    virtual ~ClusterInfoHeader();

    /**
     * \param seq the sequence number
     */
    void SetSeq(uint64_t seq);

    /**
     * \return the sequence number
     */
    uint64_t GetSeq(void) const;

    /**
     * \param nodeId the id of the node
     */
    void SetMobilityInfo(ClusterSap::NeighborInfo mobilityInfo);
    /**
     * \return the id of the node
     */
    ClusterSap::NeighborInfo GetMobilityInfo(void) const;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream &os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

private:

    uint64_t m_seq;                                 //!< Sequence number
    ClusterSap::NeighborInfo m_mobilityInfo;     //!< Mobile Info
};


/**
 * \ingroup cluster
 * \class InitiateClusterHeader
 * \brief Packet header for ControlClient application.
 *
 * The header is made of a 32bits cluster ID followed by
 * a 64bits time stamp and a 64bits seq number.
 */
class InitiateClusterHeader: public Header {
public:

    InitiateClusterHeader();
    virtual ~InitiateClusterHeader();

    /**
     * \return the time stamp
     */
    Time GetTs(void) const;

    /**
     * \param seq the sequence number
     */
    void SetSeq(uint64_t seq);

    /**
     * \return the sequence number
     */
    uint64_t GetSeq(void) const;

    /**
     * \param degree the degree of the node
     */
    void SetClusterId(uint64_t clusterId);
    /**
     * \return the degree of the node
     */
    uint64_t GetClusterId(void) const;

    /**
     * \param nodeId the id of the node
     */
    void SetMobilityInfo(ClusterSap::NeighborInfo mobilityInfo);
    /**
     * \return the id of the node
     */
    ClusterSap::NeighborInfo GetMobilityInfo(void) const;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream &os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

private:

    uint64_t m_clusterId;       //!< Cluster id
    uint64_t m_ts;              //!< Timestamp
    uint64_t m_seq;             //!< Sequence number
    ClusterSap::NeighborInfo m_mobilityInfo;         //!< Mobile Info

};


/**
 * \ingroup v2v
 * \class FormClusterHeader
 * \brief Packet header for V2vControlClient application.
 *
 * The header is made of a 64bits sequence number followed by
 * a mobility information structure.
 */
class FormClusterHeader: public Header {
public:

    FormClusterHeader();
    virtual ~FormClusterHeader();

    /**
     * \param seq the sequence number
     */
    void SetSeq(uint64_t seq);

    /**
     * \return the sequence number
     */
    uint64_t GetSeq(void) const;

    /**
     * \param nodeId the id of the node
     */
    void SetMobilityInfo(ClusterSap::NeighborInfo mobilityInfo);
    /**
     * \return the id of the node
     */
    ClusterSap::NeighborInfo GetMobilityInfo(void) const;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream &os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

private:

    uint64_t m_seq;                                     //!< Sequence number
    ClusterSap::NeighborInfo m_mobilityInfo;         //!< Mobile Info
};


/**
 * \ingroup v2v
 * \class IncidentEventHeader
 * \brief Packet header for incident event
 *
 * The header is made of a 64bits timestamp followed by
 * an IncidentInfo structure
 */
class IncidentEventHeader: public Header {
public:

    IncidentEventHeader();
    virtual ~IncidentEventHeader();

    /**
     * \return the time stamp
     */
    Time GetTs(void) const;

    /**
     * \param incidentType the incident type
     */
    void SetIncidentInfo(ClusterSap::IncidentInfo incidentInfo);

    /**
     * \return the incident type of the event
     */
    ClusterSap::IncidentInfo GetIncidentInfo(void) const;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream &os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

private:

    uint64_t m_ts;                                      //!< Timestamp
    ClusterSap::IncidentInfo m_incidentInfo;         //!< Incident Information
};

} // namespace ns3

#endif // V2V_CLUSTER_HEADER_H
