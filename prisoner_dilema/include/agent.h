#ifndef _AGENT_H_
#define _AGENT_H_

#include <global.h>

/* Agents */
class Agent{
	
private:
    repast::AgentId   id_;
    double              c;
    double          total;
	
public:
    Agent(repast::AgentId id);
	Agent(){}
    Agent(repast::AgentId id, double newC, double newTotal);
	
    ~Agent();
	
    /* Required Getters */
    virtual repast::AgentId& getId(){                   return id_;    }
    virtual const repast::AgentId& getId() const {      return id_;    }
	
    /* Getters specific to this kind of Agent */
    double getC(){                                      return c;      }
    double getTotal(){                                  return total;  }
	
    /* Setter */
    void set(uint32_t currentRank, double newC, double newTotal);
	
    /* Actions */
    bool cooperate();                                                 // Will indicate whether the agent cooperates or not; probability determined by = c / total
    void play(repast::SharedContext<Agent>* context);    // Choose three other agents from the given context and see if they cooperate or not
	
};

/* Serializable Agent Package */
struct AgentPackage {
	
public:
    uint32_t    id;
    uint32_t    rank;
    uint32_t    type;
    uint32_t    currentRank;
    double c;
    double total;
	
    /* Constructors */
    AgentPackage(); // For serialization
    AgentPackage(uint32_t _id, uint32_t _rank, uint32_t _type, uint32_t _currentRank, double _c, double _total);
	
    /* For archive packaging */
    template<class Archive>
    void serialize(Archive &ar, const uint32_t version){
        ar & id;
        ar & rank;
        ar & type;
        ar & currentRank;
        ar & c;
        ar & total;
    }
	
};

#endif