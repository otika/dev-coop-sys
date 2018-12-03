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

#ifndef PROPAGATION_CONTROL_HEADER_H
#define PROPAGATION_CONTROL_HEADER_H

#include "ns3/log.h"
#include "ns3/header.h"
#include "cluster-sap.h"
#include "cluster-control-client.h"

namespace ns3 {

/**
 * \ingroup clustering
 * \class ClusterInfoHeader
 * \brief Packet header for ClusterControlClient application.
 *
 * The header is made of a 64bits sequence number followed by
 * a mobility information structure.
 */
class DistroMapHeader: public Header {
public:

    DistroMapHeader();
    virtual ~DistroMapHeader();

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
    void SetDistroMap(float* distroMap);

    /**
     * \return the id of the node
     */
    float* GetDistroMap(float* distroMap) const;

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
    uint64_t m_seq;                                 //!< Sequence number
    ClusterSap::NeighborInfo m_mobilityInfo;         //!< Mobile Info
    float m_distroMap[ClusterControlClient::Constants::DISTRO_MAP_SIZE
					  * ClusterControlClient::Constants::DISTRO_MAP_SIZE];
};

/**
 * \ingroup clustering
 * \class InterClusterPropagationHeader
 * \brief Packet header for ClusterControlClient application.
 *
 * The header is made of a 64bits sequence number followed by
 * a mobility information structure.
 */
class InterClusterPropagationHeader: public Header {
public:

    InterClusterPropagationHeader();
    virtual ~InterClusterPropagationHeader();

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

    void SetInterClusterInfo(ClusterSap::InterClusterPropagationInfo info);

    ClusterSap::InterClusterPropagationInfo GetInterClusterInfo(void) const;

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
    uint64_t m_seq;             //!< Sequence number
    ClusterSap::InterClusterPropagationInfo m_interClusterInfo;
};

/**
 * \ingroup clustering
 * \class IntraClusterPropagationHeader
 * \brief Packet header for ClusterControlClient application.
 *
 * The header is made of a 64bits sequence number followed by
 * a mobility information structure.
 */
class IntraClusterPropagationHeader: public Header {
public:

    IntraClusterPropagationHeader();
    virtual ~IntraClusterPropagationHeader();

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

    void SetIntraClusterInfo(ClusterSap::IntraClusterPropagationInfo info);

    ClusterSap::IntraClusterPropagationInfo GetIntraClusterInfo(void) const;

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
    uint64_t m_seq;             //!< Sequence number
    ClusterSap::IntraClusterPropagationInfo m_intraClusterInfo;
};

/**
 * \ingroup clustering
 * \class InterNodePropagationHeader
 * \brief Packet header for ClusterControlClient application.
 *
 * The header is made of a 64bits sequence number followed by
 * a mobility information structure.
 */
class InterNodePropagationHeader: public Header {
public:

    InterNodePropagationHeader();
    virtual ~InterNodePropagationHeader();

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

    void SetInterNodeInfo(ClusterSap::InterNodePropagationInfo info);

    ClusterSap::InterNodePropagationInfo GetInterNodeInfo(void) const;

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
    uint64_t m_seq;             //!< Sequence number
    ClusterSap::InterNodePropagationInfo m_interNodeInfo;
};

/**
 * \ingroup clustering
 * \class AckHeader
 * \brief Packet header for ClusterControlClient application.
 *
 * The header is made of a 64bits sequence number followed by
 * a mobility information structure.
 */
class AckHeader: public Header {
public:

    AckHeader();
    virtual ~AckHeader();

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
     * \param the type ID of acknowledge header.
     * \return the object TypeId
     */
    void SetAckTypeId(TypeId id);

    /**
     * \return the object TypeId of acknowledge header.
     */
    TypeId GetAckTypeId(void) const;

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
    uint64_t m_seq;                                 //!< Sequence number
    TypeId m_tid;
};

} // namespace ns3

#endif // PROPAGATION_CONTROL_HEADER_H
