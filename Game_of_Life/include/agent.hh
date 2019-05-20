#ifndef _AGENT_H_
#define _AGENT_H_

#include <global.hh>

class GolAgent {
private:
	repast::AgentId   _id;
	double _probLiveAgent;
	State  _agentState[2];
	
public:
	
	GolAgent(repast::AgentId id, double& probLiveAgent);
	GolAgent(repast::AgentId id, double& probLiveAgent, State& agentState);
	~GolAgent();
	
	/* Required Getters */
    virtual       repast::AgentId& getId()       { return _id; }
    virtual const repast::AgentId& getId() const { return _id; }
	
	/* Setter */
	void set(int currentRank, double probLiveAgent, State agentState);
	
	void applyRules(repast::SharedContext<GolAgent>* context, \
		repast::SharedDiscreteSpace<GolAgent, repast::WrapAroundBorders, repast::SimpleAdder<GolAgent> >* space);
	
	void updateState();
	
	State getState() const;
	double getProbLive() const;
};


/* Serializable Agent Package */
struct GolAgentPackage {
	
public:
    int    _id;
    int    _rank;
    int    _type;
    int    _currentRank;
	double _probLiveAgent;
    State  _agentState;
   
    /* Constructors */
    GolAgentPackage(); // For serialization
    GolAgentPackage(int id, int rank, int type, int currentRank, State agentState, double probLiveAgent);
	
    /* For archive packaging */
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version){
        ar & _id;
        ar & _rank;
        ar & _type;
        ar & _currentRank;
		ar & _probLiveAgent;
        ar & _agentState;      
    }
};


#endif
