#ifndef _AGENT_H_
#define _AGENT_H_

#include <global.hh>

class SIRAgent {
private:
	repast::AgentId   _id;
	State  _agentState;
	uint32_t _tiempoInfeccion;

public:

	SIRAgent(repast::AgentId id);
	SIRAgent(repast::AgentId id, State& agentState, uint32_t& tiempoInfeccion);
	~SIRAgent();

	/* Required Getters */
	virtual       repast::AgentId& getId()
	{
		return _id;
	}
	virtual const repast::AgentId& getId() const
	{
		return _id;
	}

	/* Setter */
	void set(int currentRank, State agentState, uint32_t& tiempoInfeccion);

	void move(\
	          repast::SharedDiscreteSpace  <SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* dSpace, \
	          repast::SharedContinuousSpace<SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* cSpace\
	         );


	void applyRules(repast::SharedContext<SIRAgent>* context, \
	                repast::SharedDiscreteSpace  <SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* dSpace, \
	                repast::SharedContinuousSpace<SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* cSpace);


	State getState() const;
	void setState(const State& st);
	uint32_t getTiempoInfeccion() const;
};


/* Serializable Agent Package */
struct SIRAgentPackage {

public:
	int    _id;
	int    _rank;
	int    _type;
	int    _currentRank;
	State  _agentState;
	uint32_t _tiempoInfeccion;

	/* Constructors */
	SIRAgentPackage(); // For serialization
	SIRAgentPackage(int id, int rank, int type, int currentRank, State agentState, uint32_t tiempoInfeccion);

	/* For archive packaging */
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & _id;
		ar & _rank;
		ar & _type;
		ar & _currentRank;
		ar & _agentState;
		ar & _tiempoInfeccion;
	}
};


#endif
