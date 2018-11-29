#include <global.hh>
#include <json.hpp>

namespace Global{
	json settings;
	std::string map_osrm;
	json area_zone;
	json initial_zones;
	json reference_zones;
	//json reference_point;
	
	bool  showProgressBar;
	float closeEnough;
	float randomWalkwayRadius;
	float attractionRadius;
}

namespace Stats{
	std::vector<std::string> logZonesDensity;
}

namespace Counters{
	uint32_t timeExecMakeAgents;
	uint32_t timeExecCalibrate;
	uint32_t timeExecSimulate;
}

std::map<std::string, uint32_t> model_t = {
	{"ShortestPath",   0},
	{"FollowTheCrowd", 1},
	{"RandomWalkway",  2}
};
