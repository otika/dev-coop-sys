/*
 * DistroMap.h
 *
 *  Created on: 2018/12/02
 *      Author: akito
 */

#ifndef SRC_CLUSTERING_MODEL_META_DATA_H_
#define SRC_CLUSTERING_MODEL_META_DATA_H_

#include <map>
#include <vector>
#include "cluster-sap.h"
#include "ns3/vector.h"

namespace ns3 {

class MetaData {
private:
	MetaData() = default;
	~MetaData() = default;

public:
	using DistroMap = std::map<uint64_t, std::vector<float>>;
	using ChInfoMap = std::map<uint64_t, ClusterSap::NeighborInfo>;
	using PropagationVectorMap = std::map<uint64_t, Vector>;

	MetaData(const MetaData&) = delete;
	MetaData& operator=(const MetaData&) = delete;
	MetaData(MetaData&&) = delete;
	MetaData& operator=(MetaData&&) = delete;

	static MetaData& GetInstance()
	{
		static MetaData instance;
		return instance;
	}

	std::map<uint64_t, std::vector<float>> distroMap;
	std::map<uint64_t, ClusterSap::NeighborInfo> chInfo;
	std::map<uint64_t, Vector> basePropagationVector;
	std::map<uint64_t, Vector> propagationVector;
};

} /* namespace ns3 */

#endif /* SRC_CLUSTERING_MODEL_META_DATA_H_ */
