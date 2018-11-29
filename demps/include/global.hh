#ifndef _GLOB_H_
#define _GLOB_H_


#include <CGAL/Line_2.h>
#include <CGAL/Origin.h>
#include <CGAL/Polygon_2.h>
//#include <CGAL/Cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>  //new 
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/Aff_transformation_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>

#define CGAL_HAS_THREADS

#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Constants.hpp>
#include <GeographicLib/GeoCoords.hpp>
#include <GeographicLib/LocalCartesian.hpp>
using namespace GeographicLib;

#include <osrm/match_parameters.hpp>
#include <osrm/nearest_parameters.hpp>
#include <osrm/route_parameters.hpp>
#include <osrm/table_parameters.hpp>
#include <osrm/trip_parameters.hpp>
#include <osrm/coordinate.hpp>
#include <osrm/engine_config.hpp>
#include <osrm/json_container.hpp>
#include <osrm/osrm.hpp>
#include <osrm/status.hpp>



struct FaceInfo2
{
  FaceInfo2(){}
  int nesting_level;
  bool in_domain(){ 
    return nesting_level%2 == 1;
  }
};

typedef CGAL::Exact_predicates_inexact_constructions_kernel      K;//new 
//typedef CGAL::Simple_cartesian<double> K;

typedef CGAL::Aff_transformation_2<K> Transformation;

typedef CGAL::Line_2<K>     Line2D;
typedef CGAL::Point_2<K>    Point2D;
typedef CGAL::Triangle_2<K> Triangle2D;//new
typedef CGAL::Vector_2<K>   Vector2D;
typedef CGAL::Polygon_2<K>  Polygon2D;

typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2,K>    Fbb;//new 
//typedef CGAL::Delaunay_mesh_face_base_2<K>                        Fb;
typedef CGAL::Constrained_triangulation_face_base_2<K,Fbb>        Fb;//new
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>              Tds;

typedef CGAL::Exact_predicates_tag                                Itag;//new
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds, Itag>  CDT;//new
//typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds>        CDT;

typedef CGAL::Delaunay_mesh_size_criteria_2<CDT>                  Mesh_2_criteria;



#include <iostream>
#include <vector>
#include <exception>
#include <utility>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <mutex>
#include <list>
#include <random>
#include <omp.h>
#include <iomanip>
#include <stdexcept>
#include <chrono>
#include <cmath>
#include <cassert>

#include <json.hpp>
#include <progressBar.hpp>

using json=nlohmann::json;



#include <boost/mpi.hpp>
#include <repast_hpc/RepastProcess.h>
#include <repast_hpc/Utilities.h>
#include <repast_hpc/Schedule.h>
#include <repast_hpc/Properties.h>

#include <repast_hpc/AgentId.h>
#include <repast_hpc/SharedContext.h>
#include "repast_hpc/AgentRequest.h"

#include "repast_hpc/initialize_random.h"

#include <repast_hpc/SharedDiscreteSpace.h>
#include <repast_hpc/SharedContinuousSpace.h>

#include <repast_hpc/GridComponents.h>
#include <repast_hpc/Point.h>
#include <repast_hpc/Moore2DGridQuery.h>

namespace Global{
	extern json settings;
	extern std::string map_osrm;
	extern json area_zone;
	extern json initial_zones;
	extern json reference_zones;
	//extern json reference_point;
	
	extern bool  showProgressBar;
	extern float closeEnough;
	extern float randomWalkwayRadius;
	extern float attractionRadius;
}

namespace Stats{
	extern std::vector<std::string> logZonesDensity;
}

namespace Counters{
	extern uint32_t timeExecMakeAgents;
	extern uint32_t timeExecCalibrate;
	extern uint32_t timeExecSimulate;
}

enum model_codes {
	ShortestPath=0,
	FollowTheCrowd=1,
	RandomWalkway=2
}; 

extern std::map<std::string, uint32_t> model_t;



#endif