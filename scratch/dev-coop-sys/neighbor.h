/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef NEIGHBOR_H
#define NEIGHBOR_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include <vector>

namespace ns3 {

  std::vector<double> GetNeighbors(Ptr<Node> targetNode, NodeContainer nodes, double distance);
  double GetDistance(Vector3D pos1, Vector3D pos2);
  Vector3D GetPosition(Ptr<Node> node);

}

#endif /* NEIGHBOR_H */
