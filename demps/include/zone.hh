#ifndef _ZONE_H_
#define _ZONE_H_
#include <global.hh>

class Zone {
private:
	std::string          _nameID;
	double               _area; 
	LocalCartesian       _projector;
	Polygon2D            _polygon;
	CDT                  _cdt;
	
	std::set< repast::AgentId > _agentsInZone;
	double                      _agentsDensity;

public:
	Zone(void);
	Zone(const json&,const json&);
	Zone(const Zone&);
	~Zone(void);

	Zone& operator=(const Zone&);

	bool pointIsInside(const Point2D& testPoint);
	void addAgent(const repast::AgentId& idAgent);
	void deleteAgent(const repast::AgentId& idAgent);
	void updateAgentsDensity(void);
	double getAgentDensity(void);
	std::string getNameID(void);
	Point2D generate(void);
	
private:
	void mark_domains(CDT& cdt);
	void mark_domains(CDT& ct, CDT::Face_handle start, int index, std::list<CDT::Edge>& border );
};
#endif
