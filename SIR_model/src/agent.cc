#include <agent.hh>


SIRAgent::SIRAgent(repast::AgentId id)
{
	uint32_t myRank   = repast::RepastProcess::instance()->rank();

	_id = id;

	_agentState = (id.id()==0 && myRank==0) ? I : S;
	_tiempoInfeccion    = 0;
}

SIRAgent::SIRAgent(repast::AgentId id, State& agentState, uint32_t& tiempoInfeccion)
{
	_id = id;
	_agentState = agentState;
	_tiempoInfeccion = tiempoInfeccion;
}

SIRAgent::~SIRAgent()
{

}

void SIRAgent::set(int currentRank, State agentState, uint32_t& tiempoInfeccion)
{

	_id.currentRank(currentRank);
	_agentState = agentState;
	_tiempoInfeccion = tiempoInfeccion;

}

void SIRAgent::move(\
                    repast::SharedDiscreteSpace  <SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* dSpace,\
                    repast::SharedContinuousSpace<SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* cSpace )
{

	uint32_t myRank   = repast::RepastProcess::instance()->rank();
	double   currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();

	std::vector<double> agentLoc;
	cSpace->getLocation(_id, agentLoc);

	std::vector<double> agentNewLocC;
	agentNewLocC.push_back(agentLoc[0] + (repast::Random::instance()->nextDouble() - .5) * 2.0);
	agentNewLocC.push_back(agentLoc[1] + (repast::Random::instance()->nextDouble() - .5) * 2.0);
	cSpace->moveTo(_id, agentNewLocC);

	std::vector<int> agentNewLocD;
	agentNewLocD.push_back((int)(floor(agentNewLocC[0])));
	agentNewLocD.push_back((int)(floor(agentNewLocC[1])));
	dSpace->moveTo(_id, agentNewLocD);


}

void SIRAgent::applyRules(repast::SharedContext<SIRAgent>* context, \
                          repast::SharedDiscreteSpace  <SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* dSpace, \
                          repast::SharedContinuousSpace<SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* cSpace)
{

	uint32_t myRank   = repast::RepastProcess::instance()->rank();
	double   currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();

	std::vector<SIRAgent*> neighbors;

	std::vector<int32_t> agentLocCenter;
	dSpace->getLocation(_id, agentLocCenter);
	repast::Point<int32_t> centerD(agentLocCenter);
	repast::Moore2DGridQuery<SIRAgent> moore2DQuery(dSpace);

	moore2DQuery.query(centerD, 2, false, neighbors);

	std::vector<double> agentLoc;
	cSpace->getLocation(_id, agentLoc);
	repast::Point<double> centerC(agentLoc[0], agentLoc[1]);

	for(auto fooAgent : neighbors) {
		State fooAgentState;
		std::vector<double> fooAgentLocC;

		cSpace->getLocation(fooAgent->getId(), fooAgentLocC);
		repast::Point<double> fooAgentPoint(fooAgentLocC[0], fooAgentLocC[1]);
		double distance = cSpace->getDistance(centerC, fooAgentPoint);
		if( distance < Global::radioInfeccion && fooAgent->getState()==S ) {
			double idx = repast::Random::instance()->nextDouble();
			if(idx < Global::probInfeccion) {
				fooAgent->setState(I);
			}
		}		
	}
	
	_tiempoInfeccion++;

	double idx = repast::Random::instance()->nextDouble();
	if( _tiempoInfeccion > Global::tiempoRecuperacion &&  idx < Global::probInmunidad ) {
		this->setState(R);
	}

}

State SIRAgent::getState() const
{
	return(_agentState);
}

void SIRAgent::setState(const State& st)
{
	_agentState	= st;
}

uint32_t SIRAgent::getTiempoInfeccion() const
{
	return(_tiempoInfeccion);
}


/* Serializable Agent Package Data */

SIRAgentPackage::SIRAgentPackage() { }

SIRAgentPackage::SIRAgentPackage(int id, int rank, int type, int currentRank, State agentState, uint32_t tiempoInfeccion):
	_id(id), _rank(rank), _type(type), _currentRank(currentRank), _agentState(agentState), _tiempoInfeccion(tiempoInfeccion)
{

}







