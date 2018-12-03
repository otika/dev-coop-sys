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

#include "ns3/simulator.h"
#include "propagation-control-header.h"

NS_LOG_COMPONENT_DEFINE ("PropagationControlHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(DistroMapHeader);

DistroMapHeader::DistroMapHeader() :
   m_seq(0){
   NS_LOG_FUNCTION (this);
}

DistroMapHeader::~DistroMapHeader(){
   NS_LOG_FUNCTION (this);
}

void
DistroMapHeader::SetSeq(uint64_t seq) {
   NS_LOG_FUNCTION (this << seq);
   m_seq = seq;
}

uint64_t
DistroMapHeader::GetSeq(void) const {
   NS_LOG_FUNCTION (this);
   return m_seq;
}


void
DistroMapHeader::SetClusterId(uint64_t clusterId){
    NS_LOG_FUNCTION (this << clusterId);
    m_clusterId = clusterId;
}

uint64_t
DistroMapHeader::GetClusterId(void) const {
    NS_LOG_FUNCTION (this);
    return m_clusterId;
}

void
DistroMapHeader::SetDistroMap(float* distroMap) {
	NS_LOG_FUNCTION (this);
	int element_num = ClusterControlClient::Constants::DISTRO_MAP_SIZE * ClusterControlClient::Constants::DISTRO_MAP_SIZE;
	for(int i = 0; i < element_num; i++){
		m_distroMap[i] = distroMap[i];
	}
}

float*
DistroMapHeader::GetDistroMap(float* distroMap) const {
	NS_LOG_FUNCTION (this);
	int element_num = ClusterControlClient::Constants::DISTRO_MAP_SIZE * ClusterControlClient::Constants::DISTRO_MAP_SIZE;
	for(int i = 0; i < element_num; i++){
		distroMap[i] = m_distroMap[i];
	}
	return distroMap;
}

void
DistroMapHeader::SetMobilityInfo(ClusterSap::NeighborInfo mobilityInfo){
    NS_LOG_FUNCTION (this << mobilityInfo.imsi);
    m_mobilityInfo = mobilityInfo;
}

ClusterSap::NeighborInfo
DistroMapHeader::GetMobilityInfo(void) const {
    NS_LOG_FUNCTION (this);
    return m_mobilityInfo;
}

TypeId
DistroMapHeader::GetTypeId(void) {
   static TypeId tid = TypeId("ns3::DistroMapHeader").SetParent<Header>().AddConstructor<DistroMapHeader>();
   return tid;
}

TypeId
DistroMapHeader::GetInstanceTypeId(void) const {
   return GetTypeId();
}

void
DistroMapHeader::Print(std::ostream &os) const {
    NS_LOG_FUNCTION (this << &os);
    os << "( ClusterId=" << m_clusterId << " Seq=" << m_seq << ")";
}

uint32_t
DistroMapHeader::GetSerializedSize(void) const {
   NS_LOG_FUNCTION (this);
   return sizeof(uint64_t) + sizeof(uint64_t)
		   + (sizeof(float) * ClusterControlClient::Constants::DISTRO_MAP_SIZE
				   * ClusterControlClient::Constants::DISTRO_MAP_SIZE)
				   + sizeof(ClusterSap::NeighborInfo);
}

void
DistroMapHeader::Serialize(Buffer::Iterator start) const {
    NS_LOG_FUNCTION (this << &start);

    Buffer::Iterator i = start;
    i.WriteHtonU64(m_clusterId);
    i.WriteHtonU64(m_seq);
    // Write distro map
    unsigned char temp[(sizeof(float) * ClusterControlClient::Constants::DISTRO_MAP_SIZE
				   * ClusterControlClient::Constants::DISTRO_MAP_SIZE)];
    memcpy( temp, &m_distroMap, (sizeof(float) * ClusterControlClient::Constants::DISTRO_MAP_SIZE
				   * ClusterControlClient::Constants::DISTRO_MAP_SIZE));
    i.Write(temp, (sizeof(float) * ClusterControlClient::Constants::DISTRO_MAP_SIZE
				   * ClusterControlClient::Constants::DISTRO_MAP_SIZE));
    // Write mobility structure
    unsigned char temp2[sizeof(ClusterSap::NeighborInfo)];
    memcpy( temp2, &m_mobilityInfo, sizeof(ClusterSap::NeighborInfo));
    i.Write(temp2, sizeof(ClusterSap::NeighborInfo));
}

uint32_t
DistroMapHeader::Deserialize(Buffer::Iterator start) {
   NS_LOG_INFO (this << &start);

   Buffer::Iterator i = start;
   m_clusterId = i.ReadNtohU64();
   m_seq = i.ReadNtohU64();

   unsigned char temp[sizeof(ClusterSap::NeighborInfo)];
   i.Read(temp, (sizeof(float) * ClusterControlClient::Constants::DISTRO_MAP_SIZE
				   * ClusterControlClient::Constants::DISTRO_MAP_SIZE));
   memcpy(&m_distroMap, &temp, sizeof(ClusterSap::NeighborInfo));

    unsigned char temp2[sizeof(ClusterSap::NeighborInfo)];
    i.Read(temp2, sizeof(ClusterSap::NeighborInfo));
    memcpy(&m_mobilityInfo, &temp2, sizeof(ClusterSap::NeighborInfo) );

   return GetSerializedSize();
}

NS_OBJECT_ENSURE_REGISTERED(InterClusterPropagationHeader);

InterClusterPropagationHeader::InterClusterPropagationHeader() :
   m_seq(0){
   NS_LOG_FUNCTION (this);
}

InterClusterPropagationHeader::~InterClusterPropagationHeader(){
   NS_LOG_FUNCTION (this);
}

void
InterClusterPropagationHeader::SetSeq(uint64_t seq) {
   NS_LOG_FUNCTION (this << seq);
   m_seq = seq;
}

uint64_t
InterClusterPropagationHeader::GetSeq(void) const {
   NS_LOG_FUNCTION (this);
   return m_seq;
}

void
InterClusterPropagationHeader::SetClusterId(uint64_t clusterId){
    NS_LOG_FUNCTION (this << clusterId);
    m_clusterId = clusterId;
}

uint64_t
InterClusterPropagationHeader::GetClusterId(void) const {
    NS_LOG_FUNCTION (this);
    return m_clusterId;
}

void
InterClusterPropagationHeader::SetInterClusterInfo(ClusterSap::InterClusterPropagationInfo info) {
	m_interClusterInfo = info;
}

ClusterSap::InterClusterPropagationInfo
InterClusterPropagationHeader::GetInterClusterInfo(void) const {
	return m_interClusterInfo;
}


TypeId
InterClusterPropagationHeader::GetTypeId(void) {
   static TypeId tid = TypeId("ns3::InterClusterPropagationHeader").SetParent<Header>().AddConstructor<InterClusterPropagationHeader>();
   return tid;
}

TypeId
InterClusterPropagationHeader::GetInstanceTypeId(void) const {
   return GetTypeId();
}

void
InterClusterPropagationHeader::Print(std::ostream &os) const {
    NS_LOG_FUNCTION (this << &os);
    os << "( ClusterId=" << m_clusterId << " Seq=" << m_seq << ")";
}

uint32_t
InterClusterPropagationHeader::GetSerializedSize(void) const {
   NS_LOG_FUNCTION (this);
   return sizeof(uint64_t) + sizeof(uint64_t)
		   + sizeof(ClusterSap::InterClusterPropagationInfo);
}

void
InterClusterPropagationHeader::Serialize(Buffer::Iterator start) const {
    NS_LOG_FUNCTION (this << &start);

    Buffer::Iterator i = start;
    i.WriteHtonU64(m_clusterId);
    i.WriteHtonU64(m_seq);

    // Write mobility structure
    unsigned char temp[sizeof(ClusterSap::InterClusterPropagationInfo)];
    memcpy( temp, &m_interClusterInfo, sizeof(ClusterSap::InterClusterPropagationInfo));
    i.Write(temp, sizeof(ClusterSap::InterClusterPropagationInfo));
}

uint32_t
InterClusterPropagationHeader::Deserialize(Buffer::Iterator start) {
   NS_LOG_INFO (this << &start);

   Buffer::Iterator i = start;
   m_clusterId = i.ReadNtohU64();
   m_seq = i.ReadNtohU64();

    unsigned char temp[sizeof(ClusterSap::InterClusterPropagationInfo)];
    i.Read(temp, sizeof(ClusterSap::InterClusterPropagationInfo));
    memcpy(&m_interClusterInfo, &temp, sizeof(ClusterSap::InterClusterPropagationInfo) );

   return GetSerializedSize();
}


NS_OBJECT_ENSURE_REGISTERED(AckHeader);

AckHeader::AckHeader() :
   m_seq(0){
   NS_LOG_FUNCTION (this);
}

AckHeader::~AckHeader(){
   NS_LOG_FUNCTION (this);
}

void
AckHeader::SetSeq(uint64_t seq) {
   NS_LOG_FUNCTION (this << seq);
   m_seq = seq;
}

uint64_t
AckHeader::GetSeq(void) const {
   NS_LOG_FUNCTION (this);
   return m_seq;
}

void
AckHeader::SetClusterId(uint64_t clusterId){
    NS_LOG_FUNCTION (this << clusterId);
    m_clusterId = clusterId;
}

uint64_t
AckHeader::GetClusterId(void) const {
    NS_LOG_FUNCTION (this);
    return m_clusterId;
}

void
AckHeader::SetAckTypeId(TypeId id){
	m_tid = id;
}

TypeId
AckHeader::GetAckTypeId(void) const{
	return m_tid;
}

TypeId
AckHeader::GetTypeId(void) {
   static TypeId tid = TypeId("ns3::AckHeader").SetParent<Header>().AddConstructor<AckHeader>();
   return tid;
}

TypeId
AckHeader::GetInstanceTypeId(void) const {
   return GetTypeId();
}

void
AckHeader::Print(std::ostream &os) const {
    NS_LOG_FUNCTION (this << &os);
    os << "( ClusterId=" << m_clusterId << " Seq=" << m_seq << ")";
}

uint32_t
AckHeader::GetSerializedSize(void) const {
   NS_LOG_FUNCTION (this);
   return sizeof(uint64_t) + sizeof(uint64_t)
		   + sizeof(TypeId);
}

void
AckHeader::Serialize(Buffer::Iterator start) const {
    NS_LOG_FUNCTION (this << &start);

    Buffer::Iterator i = start;
    i.WriteHtonU64(m_clusterId);
    i.WriteHtonU64(m_seq);

    // Write mobility structure
    unsigned char temp[sizeof(TypeId)];
    memcpy( temp, &m_tid, sizeof(TypeId));
    i.Write(temp, sizeof(TypeId));
}

uint32_t
AckHeader::Deserialize(Buffer::Iterator start) {
   NS_LOG_INFO (this << &start);

   Buffer::Iterator i = start;
   m_clusterId = i.ReadNtohU64();
   m_seq = i.ReadNtohU64();

    unsigned char temp[sizeof(TypeId)];
    i.Read(temp, sizeof(TypeId));
    memcpy(&m_tid, &temp, sizeof(TypeId) );

   return GetSerializedSize();
}
} // namespace ns3
