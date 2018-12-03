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
#include "cluster-header.h"

NS_LOG_COMPONENT_DEFINE ("ClusterHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(ClusterInfoHeader);

ClusterInfoHeader::ClusterInfoHeader() :
   m_seq(0){
   NS_LOG_FUNCTION (this);
}

ClusterInfoHeader::~ClusterInfoHeader(){
   NS_LOG_FUNCTION (this);
}

void
ClusterInfoHeader::SetSeq(uint64_t seq) {
   NS_LOG_FUNCTION (this << seq);
   m_seq = seq;
}

uint64_t
ClusterInfoHeader::GetSeq(void) const {
   NS_LOG_FUNCTION (this);
   return m_seq;
}

void
ClusterInfoHeader::SetMobilityInfo(ClusterSap::NeighborInfo mobilityInfo){
   NS_LOG_FUNCTION (this << mobilityInfo.imsi);
   m_mobilityInfo = mobilityInfo;
}

ClusterSap::NeighborInfo
ClusterInfoHeader::GetMobilityInfo(void) const {
   NS_LOG_FUNCTION (this);
   return m_mobilityInfo;
}

TypeId
ClusterInfoHeader::GetTypeId(void) {
   static TypeId tid = TypeId("ns3::ClusterInfoHeader").SetParent<Header>().AddConstructor<ClusterInfoHeader>();
   return tid;
}

TypeId
ClusterInfoHeader::GetInstanceTypeId(void) const {
   return GetTypeId();
}

void
ClusterInfoHeader::Print(std::ostream &os) const {
   NS_LOG_FUNCTION (this << &os);
   os << "(seq=" << m_seq
           << "IMSI=" << m_mobilityInfo.imsi
           << "ClusterId=" << m_mobilityInfo.clusterId
           << "Degree=" << m_mobilityInfo.degree
           << "Position=" << m_mobilityInfo.position
           <<")";
}

uint32_t
ClusterInfoHeader::GetSerializedSize(void) const {
   NS_LOG_FUNCTION (this);
   return sizeof(uint64_t) + sizeof(ClusterSap::NeighborInfo);
}

void
ClusterInfoHeader::Serialize(Buffer::Iterator start) const {
   NS_LOG_FUNCTION (this << &start);

   Buffer::Iterator i = start;
   i.WriteHtonU64(m_seq);

   // Write mobility structure
   unsigned char temp[sizeof(ClusterSap::NeighborInfo)];
   memcpy( temp, &m_mobilityInfo, sizeof(ClusterSap::NeighborInfo) );
   i.Write(temp, sizeof(ClusterSap::NeighborInfo));

}

uint32_t
ClusterInfoHeader::Deserialize(Buffer::Iterator start) {
   NS_LOG_INFO (this << &start);

   Buffer::Iterator i = start;
   m_seq = i.ReadNtohU64();

   unsigned char temp[sizeof(ClusterSap::NeighborInfo)];
   i.Read(temp, sizeof(ClusterSap::NeighborInfo));
   memcpy(&m_mobilityInfo, &temp, sizeof(ClusterSap::NeighborInfo));

   return GetSerializedSize();
}


/////////////////////////////////////////////////////////////////////
NS_OBJECT_ENSURE_REGISTERED(InitiateClusterHeader);


InitiateClusterHeader::InitiateClusterHeader() :
        m_clusterId(0),
        m_ts(Simulator::Now().GetTimeStep()),
        m_seq(0){
    NS_LOG_FUNCTION (this);
}

InitiateClusterHeader::~InitiateClusterHeader(){
    NS_LOG_FUNCTION (this);
}

Time
InitiateClusterHeader::GetTs(void) const {
    NS_LOG_FUNCTION (this);
    return TimeStep(m_ts);
}

void
InitiateClusterHeader::SetSeq(uint64_t seq) {
    NS_LOG_FUNCTION (this << seq);
    m_seq = seq;
}
uint64_t
InitiateClusterHeader::GetSeq(void) const {
    NS_LOG_FUNCTION (this);
    return m_seq;
}

void
InitiateClusterHeader::SetClusterId(uint64_t clusterId){
    NS_LOG_FUNCTION (this << clusterId);
    m_clusterId = clusterId;
}

uint64_t
InitiateClusterHeader::GetClusterId(void) const {
    NS_LOG_FUNCTION (this);
    return m_clusterId;
}

void
InitiateClusterHeader::SetMobilityInfo(ClusterSap::NeighborInfo mobilityInfo){
    NS_LOG_FUNCTION (this << mobilityInfo.imsi);
    m_mobilityInfo = mobilityInfo;
}

ClusterSap::NeighborInfo
InitiateClusterHeader::GetMobilityInfo(void) const {
    NS_LOG_FUNCTION (this);
    return m_mobilityInfo;
}

TypeId
InitiateClusterHeader::GetTypeId(void) {
    static TypeId tid =
            TypeId("ns3::InitiateClusterHeader").SetParent<Header>().AddConstructor<InitiateClusterHeader>();
    return tid;
}

TypeId
InitiateClusterHeader::GetInstanceTypeId(void) const {
    return GetTypeId();
}

void
InitiateClusterHeader::Print(std::ostream &os) const {
    NS_LOG_FUNCTION (this << &os);
    os << "(time=" << TimeStep(m_ts).GetSeconds() << " ClusterId=" << m_clusterId << " Seq=" << m_seq <<")";
}

uint32_t
InitiateClusterHeader::GetSerializedSize(void) const {
    NS_LOG_FUNCTION (this);
    return sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(ClusterSap::NeighborInfo);
}

void
InitiateClusterHeader::Serialize(Buffer::Iterator start) const {
    NS_LOG_FUNCTION (this << &start);

    Buffer::Iterator i = start;
    i.WriteHtonU64(m_clusterId);
    i.WriteHtonU64(m_ts);
    i.WriteHtonU64(m_seq);
    // Write mobility structure
    unsigned char temp[sizeof(ClusterSap::NeighborInfo)];
    memcpy( temp, &m_mobilityInfo, sizeof(ClusterSap::NeighborInfo));
    i.Write(temp, sizeof(ClusterSap::NeighborInfo));
}

uint32_t
InitiateClusterHeader::Deserialize(Buffer::Iterator start) {
    NS_LOG_INFO (this << &start);

    Buffer::Iterator i = start;
    m_clusterId = i.ReadNtohU64 ();
    m_ts = i.ReadNtohU64 ();
    m_seq = i.ReadNtohU64 ();

    unsigned char temp[sizeof(ClusterSap::NeighborInfo)];
    i.Read(temp, sizeof(ClusterSap::NeighborInfo));
    memcpy(&m_mobilityInfo, &temp, sizeof(ClusterSap::NeighborInfo) );

    return GetSerializedSize();
}


/////////////////////////////////////////////////////////////////////
NS_OBJECT_ENSURE_REGISTERED(FormClusterHeader);

FormClusterHeader::FormClusterHeader() :
        m_seq(0){
    NS_LOG_FUNCTION (this);
}

FormClusterHeader::~FormClusterHeader(){
    NS_LOG_FUNCTION (this);
}

void
FormClusterHeader::SetSeq(uint64_t seq) {
    NS_LOG_FUNCTION (this << seq);
    m_seq = seq;
}
uint64_t
FormClusterHeader::GetSeq(void) const {
    NS_LOG_FUNCTION (this);
    return m_seq;
}

void
FormClusterHeader::SetMobilityInfo(ClusterSap::NeighborInfo mobilityInfo){
    NS_LOG_FUNCTION (this << mobilityInfo.imsi);
    m_mobilityInfo = mobilityInfo;
}

ClusterSap::NeighborInfo
FormClusterHeader::GetMobilityInfo(void) const {
    NS_LOG_FUNCTION (this);
    return m_mobilityInfo;
}

TypeId
FormClusterHeader::GetTypeId(void) {
    static TypeId tid =
            TypeId("ns3::FormClusterHeader").SetParent<Header>().AddConstructor<FormClusterHeader>();
    return tid;
}

TypeId
FormClusterHeader::GetInstanceTypeId(void) const {
    return GetTypeId();
}

void
FormClusterHeader::Print(std::ostream &os) const {
    NS_LOG_FUNCTION (this << &os);
    os << "(seq=" << m_seq
            << "IMSI=" << m_mobilityInfo.imsi
            << "ClusterId=" << m_mobilityInfo.clusterId
            << "Degree=" << m_mobilityInfo.degree
            << "Position=" << m_mobilityInfo.position
            <<")";
}

uint32_t
FormClusterHeader::GetSerializedSize(void) const {
    NS_LOG_FUNCTION (this);
    return sizeof(uint64_t) + sizeof(ClusterSap::NeighborInfo);
}

void
FormClusterHeader::Serialize(Buffer::Iterator start) const {
    NS_LOG_FUNCTION (this << &start);

    Buffer::Iterator i = start;
    i.WriteHtonU64(m_seq);

    // Write mobility structure
    unsigned char temp[sizeof(ClusterSap::NeighborInfo)];
    memcpy( temp, &m_mobilityInfo, sizeof(ClusterSap::NeighborInfo));
    i.Write(temp, sizeof(ClusterSap::NeighborInfo));

}

uint32_t
FormClusterHeader::Deserialize(Buffer::Iterator start) {
    NS_LOG_INFO (this << &start);

    Buffer::Iterator i = start;
    m_seq = i.ReadNtohU64();

    unsigned char temp[sizeof(ClusterSap::NeighborInfo)];
    i.Read(temp, sizeof(ClusterSap::NeighborInfo));
    memcpy(&m_mobilityInfo, &temp, sizeof(ClusterSap::NeighborInfo) );

    return GetSerializedSize();
}


/////////////////////////////////////////////////////////////////////
NS_OBJECT_ENSURE_REGISTERED(IncidentEventHeader);

IncidentEventHeader::IncidentEventHeader():
    m_ts(Simulator::Now().GetTimeStep()){
    NS_LOG_FUNCTION (this);
}

IncidentEventHeader::~IncidentEventHeader(){
    NS_LOG_FUNCTION (this);
}

Time
IncidentEventHeader::GetTs(void) const {
    NS_LOG_FUNCTION (this);
    return TimeStep(m_ts);
}

void
IncidentEventHeader::SetIncidentInfo(ClusterSap::IncidentInfo incidentInfo) {
    NS_LOG_FUNCTION (this << incidentInfo.clusterId);
    m_incidentInfo = incidentInfo;
}

ClusterSap::IncidentInfo
IncidentEventHeader::GetIncidentInfo(void) const {
    NS_LOG_FUNCTION (this);
    return m_incidentInfo;
}

TypeId
IncidentEventHeader::GetTypeId(void) {
    static TypeId tid =
            TypeId("ns3::IncidentEventHeader").SetParent<Header>().AddConstructor<IncidentEventHeader>();
    return tid;
}

TypeId
IncidentEventHeader::GetInstanceTypeId(void) const {
    return GetTypeId();
}

void
IncidentEventHeader::Print(std::ostream &os) const {
    NS_LOG_FUNCTION (this << &os);
    os << "(time=" << TimeStep(m_ts).GetSeconds() << " ClusterId=" << m_incidentInfo.clusterId
       << " IncidentType =" << m_incidentInfo.incidentType << ")";
}

uint32_t
IncidentEventHeader::GetSerializedSize(void) const {
    NS_LOG_FUNCTION (this);
    return sizeof(uint64_t) + sizeof(ClusterSap::IncidentInfo);
}

void
IncidentEventHeader::Serialize(Buffer::Iterator start) const {
    NS_LOG_FUNCTION (this << &start);

    Buffer::Iterator i = start;
    i.WriteHtonU64(m_ts);

    // Write IncidentInfo structure
    unsigned char temp[sizeof(ClusterSap::IncidentInfo)];
    memcpy( temp, &m_incidentInfo, sizeof(ClusterSap::IncidentInfo) );
    i.Write(temp, sizeof(ClusterSap::IncidentInfo));
}

uint32_t
IncidentEventHeader::Deserialize(Buffer::Iterator start) {
    NS_LOG_INFO (this << &start);

    Buffer::Iterator i = start;
    m_ts = i.ReadNtohU64 ();

    unsigned char temp[sizeof(ClusterSap::IncidentInfo)];
    i.Read(temp, sizeof(ClusterSap::IncidentInfo));
    memcpy(&m_incidentInfo, &temp, sizeof(ClusterSap::IncidentInfo) );

    return GetSerializedSize();
}

///////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NeighborClusterInfoHeader);

NeighborClusterInfoHeader::NeighborClusterInfoHeader() :
   m_seq(0){
   NS_LOG_FUNCTION (this);
}

NeighborClusterInfoHeader::~NeighborClusterInfoHeader(){
   NS_LOG_FUNCTION (this);
}

void
NeighborClusterInfoHeader::SetSeq(uint64_t seq) {
   NS_LOG_FUNCTION (this << seq);
   m_seq = seq;
}

uint64_t
NeighborClusterInfoHeader::GetSeq(void) const {
   NS_LOG_FUNCTION (this);
   return m_seq;
}

void
NeighborClusterInfoHeader::SetClusterId(uint64_t clusterId){
    NS_LOG_FUNCTION (this << clusterId);
    m_clusterId = clusterId;
}

uint64_t
NeighborClusterInfoHeader::GetClusterId(void) const {
    NS_LOG_FUNCTION (this);
    return m_clusterId;
}

void
NeighborClusterInfoHeader::SetMobilityInfo(ClusterSap::NeighborInfo mobilityInfo){
   NS_LOG_FUNCTION (this << mobilityInfo.imsi);
   m_mobilityInfo = mobilityInfo;
}

ClusterSap::NeighborInfo
NeighborClusterInfoHeader::GetMobilityInfo(void) const {
   NS_LOG_FUNCTION (this);
   return m_mobilityInfo;
}

TypeId
NeighborClusterInfoHeader::GetTypeId(void) {
   static TypeId tid = TypeId("ns3::NeighborClusterInfoHeader").SetParent<Header>().AddConstructor<NeighborClusterInfoHeader>();
   return tid;
}

TypeId
NeighborClusterInfoHeader::GetInstanceTypeId(void) const {
   return GetTypeId();
}

void
NeighborClusterInfoHeader::Print(std::ostream &os) const {
   NS_LOG_FUNCTION (this << &os);
   os << "(seq=" << m_seq
           << "IMSI=" << m_mobilityInfo.imsi
           << "ClusterId=" << m_mobilityInfo.clusterId
           << "Degree=" << m_mobilityInfo.degree
           << "Position=" << m_mobilityInfo.position
           <<")";
}

uint32_t
NeighborClusterInfoHeader::GetSerializedSize(void) const {
   NS_LOG_FUNCTION (this);
   return sizeof(uint64_t) + sizeof(uint64_t)  + sizeof(ClusterSap::NeighborInfo);
}

void
NeighborClusterInfoHeader::Serialize(Buffer::Iterator start) const {
   NS_LOG_FUNCTION (this << &start);

   Buffer::Iterator i = start;
   i.WriteHtonU64(m_seq);
   i.WriteHtonU64(m_clusterId);

   // Write mobility structure
   unsigned char temp[sizeof(ClusterSap::NeighborInfo)];
   memcpy( temp, &m_mobilityInfo, sizeof(ClusterSap::NeighborInfo) );
   i.Write(temp, sizeof(ClusterSap::NeighborInfo));

}

uint32_t
NeighborClusterInfoHeader::Deserialize(Buffer::Iterator start) {
   NS_LOG_INFO (this << &start);

   Buffer::Iterator i = start;
   m_seq = i.ReadNtohU64();
   m_clusterId = i.ReadNtohU64();

   unsigned char temp[sizeof(ClusterSap::NeighborInfo)];
   i.Read(temp, sizeof(ClusterSap::NeighborInfo));
   memcpy(&m_mobilityInfo, &temp, sizeof(ClusterSap::NeighborInfo));

   return GetSerializedSize();
}


} // namespace ns3
