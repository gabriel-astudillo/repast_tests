#include <agent.hh>


GolAgent::GolAgent(repast::AgentId id, double& probLiveAgent){
	_id = id;
	_probLiveAgent = probLiveAgent;
	
	double idx = repast::Random::instance()->nextDouble();

	_agentState[ACTUAL] = idx < _probLiveAgent ? LIVE : DEAD;
}

GolAgent::GolAgent(repast::AgentId id, double& probLiveAgent, State& agentState){
	_id = id;
	_probLiveAgent = probLiveAgent;
	_agentState[ACTUAL] = agentState;
	
}

GolAgent::~GolAgent(){
	
}

void GolAgent::set(int currentRank, double probLiveAgent, State agentState){
	
	_id.currentRank(currentRank);
	_agentState[ACTUAL]  = agentState;
	_probLiveAgent       = probLiveAgent;
	
}

void GolAgent::applyRules(repast::SharedContext<GolAgent>* context, \
	repast::SharedDiscreteSpace<GolAgent, repast::WrapAroundBorders, repast::SimpleAdder<GolAgent> >* space ){
	
	uint32_t myRank   = repast::RepastProcess::instance()->rank();	
	double   currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
	
	std::vector<GolAgent*> neighbors;
		
	std::vector<int32_t> agentLoc;
    space->getLocation(_id, agentLoc);
    repast::Point<int32_t> center(agentLoc);
    repast::Moore2DGridQuery<GolAgent> moore2DQuery(space);
	
    moore2DQuery.query(center, 1, false, neighbors);
	
	std::cout << currTick << " Rank:" << myRank << " " << _id << " State->"<< this->getState() <<std::endl;
	
	uint16_t neighborsAlive = 0;
	for(auto fooAgent : neighbors){
		State fooAgentState;
		
		fooAgentState = fooAgent->getState();
		neighborsAlive += fooAgentState;
		//std::cout << currTick << " Rank:" << myRank << " " << _id << "->"<< fooAgent->getId() << ": " << fooAgentState <<std::endl;
	}
	std::cout << currTick << " Rank:" << myRank << " " << _id << " neighborsAlive->"<< neighborsAlive <<std::endl;
	
	if( ((neighborsAlive==2) || (neighborsAlive==3)) && this->getState() == LIVE ){
		_agentState[FUTURE] = _agentState[ACTUAL];
	}
	else if( neighborsAlive==3) {
		_agentState[FUTURE] = LIVE;
	}
	else{
		_agentState[FUTURE] = DEAD;
	}
}

void GolAgent::updateState(){
	_agentState[ACTUAL] = _agentState[FUTURE];
}

State GolAgent::getState() const {
	return(_agentState[ACTUAL]);
}

double GolAgent::getProbLive() const {
	return(_probLiveAgent);
}


/* Serializable Agent Package Data */

GolAgentPackage::GolAgentPackage(){ }

GolAgentPackage::GolAgentPackage(int id, int rank, int type, int currentRank, State agentState, double probLiveAgent):
	_id(id), _rank(rank), _type(type), _currentRank(currentRank), _agentState(agentState), _probLiveAgent(probLiveAgent){ 

}







