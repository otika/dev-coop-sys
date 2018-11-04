/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef CLUSTERING_H
#define CLUSTERING_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include <vector>

namespace ns3 {
  class Cluster{
  public:
    static std::vector<Ptr<Node>> CH_List;
  };

  class ClusterMember{
  private:
    std::vector<Ptr<Node>> members_list = {}; // for CH
    std::vector<Ptr<Node>> CH_list = {}; // for CH
    Ptr<Node> myCH; // for RN
    bool isCH = false;
    bool isBorder = false;

  public:
    void MayBecomeCH();
    static ClusterMember entryAnyCluster(Node);
  };
}

#endif /* CLUSTERING_H */
