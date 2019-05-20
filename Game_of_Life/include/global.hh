#ifndef _GLOB_H_
#define _GLOB_H_

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


//
//  Bibliotecas de Repast_HPC
//
#include <boost/mpi.hpp>
#include <boost/mpi/timer.hpp>

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

}

enum TIME { 
	ACTUAL,
	FUTURE
};


enum STATE { 
	DEAD,
	LIVE
};
typedef enum STATE State;



#endif