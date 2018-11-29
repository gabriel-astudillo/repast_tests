#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_


#include <global.hh>
#include <agent.hh>
#include <router.hh>
#include <zone.hh>

//template<typename T>
class Environment: public repast::SharedContext<Agent> {
public:
	std::map<uint32_t,std::vector<repast::AgentId>> _agentsInQuad; 
	
private:
	Router _router;
	
	LocalCartesian    _projector;
	json              _reference_point;
	std::vector<Zone> _initial_zones;
	std::vector<Zone> _reference_zones;

	//std::vector<Agent> _vAgents; Esto no debe ser necesario con Repast
	
	std::vector<Agent*>   _neighbors;
	
	struct grid_s{

		//Dimensiones del mapa
		double _xMin, _xMax, _yMin, _yMax;
		double _mapWidth, _mapHeight;

		//Cantidad de cuadrantes en el eje X e Y
		uint32_t _quadX;
		uint32_t _quadY;
	};
	
	struct grid_s _grid;
	
public:
	Environment(boost::mpi::communicator* comm) : repast::SharedContext<Agent>(comm){
		;
	}
	~Environment(){
	}
	
	typedef struct grid_s grid_t;
	
	void setRouter(const std::string &_map_osrm);
	Router* getRouter(void);
	
	void setReferencePoint(const json &_freference_point);
	void setReferenceZones(const json &_freference_zones);
	void setInitialZones(const json &_finitial_zones);
	void setProjector();
	
	void setGrid(const json &_fmap_zone);
	grid_t getGrid();
	void showGrid();
	
	Zone getInitialZone(uint32_t id);
	std::vector<Zone> getInitialZones();
	std::vector<Zone>& getReferenceZones();
	LocalCartesian getProjector();
	
	void setNeighborsOf(Agent* agent, const double& distance); //FALTA

	
	double distance(Agent* a, Agent* b);
	
	
};

#endif