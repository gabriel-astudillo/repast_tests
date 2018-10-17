#include <agent.h>

Agent::Agent(repast::AgentId id): id_(id), c(100), total(200){ }

Agent::Agent(repast::AgentId id, double newC, double newTotal): id_(id), c(newC), total(newTotal){ }

Agent::~Agent(){ }

void Agent::set(uint32_t currentRank, double newC, double newTotal){
    id_.currentRank(currentRank);
    c     = newC;
    total = newTotal;
}

bool Agent::cooperate(){
	return repast::Random::instance()->nextDouble() < c/total;
}

void Agent::play(repast::SharedContext<Agent>* context){
    std::vector<Agent*> agentsToPlay;
	
	agentsToPlay.push_back(this);
    context->selectAgents(3, agentsToPlay, true);
	
    double cPayoff     = 0;
    double totalPayoff = 0;
    
    
	for( auto agentToPlay : agentsToPlay ){
		//double currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
		//std::cout << "\n" << currTick << ":\t" << this->getId() << "->" << agentToPlay->getId() << std::endl;
		
		bool iCooperated = cooperate();                          // Do I cooperate?
		double payoff = (iCooperated ?
						 (agentToPlay->cooperate() ?  7 : 1) :     // If I cooperated, did my opponent?
						 (agentToPlay->cooperate() ? 10 : 3));     // If I didn't cooperate, did my opponent?
						 
		if(iCooperated) cPayoff += payoff;
		totalPayoff             += payoff;
		
		agentToPlay++;
		
	}
    c      += cPayoff;
    total  += totalPayoff;
	
	/*	
	std::set<Agent*> agentsToPlay;
	
    agentsToPlay.insert(this); // Prohibit playing against self	
    context->selectAgents(3, agentsToPlay, true);
	
    double cPayoff     = 0;
    double totalPayoff = 0;
	
	std::set<Agent*>::iterator agentToPlay = agentsToPlay.begin();
	auto agentToPlay = agentsToPlay.begin();
	
	while(agentToPlay != agentsToPlay.end()){
		//double currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
		//std::cout << currTick << ":\t" << this->getId() << "->" << (*agentToPlay)->getId() << std::endl;
		
		bool iCooperated = cooperate();                          // Do I cooperate?
		double payoff = (iCooperated ?
						 ((*agentToPlay)->cooperate() ?  7 : 1) :     // If I cooperated, did my opponent?
						 ((*agentToPlay)->cooperate() ? 10 : 3));     // If I didn't cooperate, did my opponent?
						 
		if(iCooperated) cPayoff += payoff;
		totalPayoff             += payoff;
		
		agentToPlay++;
    }
    c      += cPayoff;
    total  += totalPayoff;
	*/
    
	
}

/*void Agent::play(repast::SharedContext<Agent>* context){
    std::set<Agent*> agentsToPlay;
	
	Agent* currAgent = this;
	
    agentsToPlay.insert(this); // Prohibit playing against self	
    context->selectAgents(3, agentsToPlay, true);
	
    double cPayoff     = 0;
    double totalPayoff = 0;
    
	//std::set<Agent*>::iterator agentToPlay = agentsToPlay.begin();
	auto agentToPlay = agentsToPlay.begin();
    
	//for( fooAgent : agentsToPlay){
	
	while(agentToPlay != agentsToPlay.end()){
		//double currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
		//std::cout << currTick << ":\t" << this->getId() << "->" << (*agentToPlay)->getId() << std::endl;
		
		bool iCooperated = cooperate();                          // Do I cooperate?
		double payoff = (iCooperated ?
						 ((*agentToPlay)->cooperate() ?  7 : 1) :     // If I cooperated, did my opponent?
						 ((*agentToPlay)->cooperate() ? 10 : 3));     // If I didn't cooperate, did my opponent?
						 
		if(iCooperated) cPayoff += payoff;
		totalPayoff             += payoff;
		
		agentToPlay++;
    }
    c      += cPayoff;
    total  += totalPayoff;
	
}*/



/* Serializable Agent Package Data */

AgentPackage::AgentPackage(){ }

AgentPackage::AgentPackage(uint32_t _id, uint32_t _rank, uint32_t _type, uint32_t _currentRank, double _c, double _total):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), c(_c), total(_total){ }

