#include "neighbor.h"


#include "ns3/mobility-module.h"
namespace ns3 {

  std::vector<double> GetNeighbors(Ptr<Node> targetNode, NodeContainer nodes, double distance)
  {
    std::vector<double> neighborList = {};

    Vector3D targetPos = GetPosition(targetNode);
    uint32_t targetId = targetNode->GetId();

    for(auto itr = nodes.Begin(); itr != nodes.End(); ++itr)
    {
      if(targetId == (**itr).GetId()){
        continue;
      }
      double dist = GetDistance(targetPos, GetPosition(*itr));
      if(dist <= distance){
        neighborList.push_back((**itr).GetId());
      }
    }
    return neighborList;
  }

  double GetDistance(Vector3D pos1, Vector3D pos2)
   {
    double x = pos1.x - pos2.x;
    double y = pos1.y - pos2.y;
    double z = pos1.z - pos2.z;
    double dist = std::sqrt(x*x + y*y + z*z);
    return dist;
  }

  Vector3D GetPosition(Ptr<Node> node)
  {
    return node->GetObject<ns3::MobilityModel>()->GetPosition();
  }

}
