#include <model.h>

AgentPackageProvider::AgentPackageProvider(repast::SharedContext<Agent>* agentPtr): agents(agentPtr){
	
}

void AgentPackageProvider::providePackage(Agent * agent, std::vector<AgentPackage>& out){
	repast::AgentId id = agent->getId();
	AgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), agent->getC(), agent->getTotal());
	out.push_back(package);
}

void AgentPackageProvider::provideContent(repast::AgentRequest req, std::vector<AgentPackage>& out){
	std::vector<repast::AgentId> ids = req.requestedAgents();
	for(size_t i = 0; i < ids.size(); i++){
		providePackage(agents->getAgent(ids[i]), out);
	}
}


AgentPackageReceiver::AgentPackageReceiver(repast::SharedContext<Agent>* agentPtr): agents(agentPtr){
	
}

Agent * AgentPackageReceiver::createAgent(AgentPackage package){
	repast::AgentId id(package.id, package.rank, package.type, package.currentRank);
	return new Agent(id, package.c, package.total);
}

void AgentPackageReceiver::updateAgent(AgentPackage package){
	repast::AgentId id(package.id, package.rank, package.type);
	Agent * agent = agents->getAgent(id);
	agent->set(package.currentRank, package.c, package.total);
}


demoModel::demoModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm){
	props       = new repast::Properties(propsFile, argc, argv, comm);
	stopAt      = repast::strToInt(props->getProperty("stopAt"));
	totalAgents = repast::strToInt(props->getProperty("totalAgents"));
	
	initializeRandom(*props, comm);
	
	if(repast::RepastProcess::instance()->rank() == 0) props->writeToSVFile("./output/record.csv");
	
	provider = new AgentPackageProvider(&context);
	receiver = new AgentPackageReceiver(&context);

}

demoModel::~demoModel(){
	delete props;
	delete provider;
	delete receiver;
}

void demoModel::init(){
	uint32_t rank = repast::RepastProcess::instance()->rank();
	//std::cout << "RANK " << rank << ",TotalAgents: " << totalAgents << std::endl;
	
	for(uint32_t i = 0; i < totalAgents; i++){
		repast::AgentId id(i, rank, 0);
		//std::cout << "RANK " << rank <<  ": " << i << "," << rank << "," << 0 << std::endl;
		id.currentRank(rank);
		Agent* agent = new Agent(id);
		context.addAgent(agent);
	}
}

void demoModel::requestAgents(){
	uint32_t rank = repast::RepastProcess::instance()->rank();
	uint32_t worldSize= repast::RepastProcess::instance()->worldSize();
	
	repast::AgentRequest req(rank);
	for(uint32_t i = 0; i < worldSize; i++){                     // For each process
		if(i != rank){                                      // ... except this one
			std::vector<Agent*> agents;        
			context.selectAgents(5, agents);                // Choose 5 local agents randomly
			for(size_t j = 0; j < agents.size(); j++){
				repast::AgentId local = agents[j]->getId(); // Transform each local agent's id into a matching non-local one
				repast::AgentId other(local.id(), i, 0);
				other.currentRank(i);
				req.addRequest(other);                      // Add it to the agent request
			}
		}
	}
	//std::cout << " BEFORE: RANK " << rank << " has " << context.size() << " agents." << std::endl;
	repast::RepastProcess::instance()->requestAgents<Agent, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, req, *provider, *receiver, *receiver);
	//std::cout << " AFTER:  RANK " << rank << " has " << context.size() << " agents." << std::endl;
}

void demoModel::cancelRequestAgents(){
	uint32_t rank = repast::RepastProcess::instance()->rank();
	
	if(rank == 0) {
		std::cout << "CANCELING AGENT REQUESTS" << std::endl;
	}
	
	repast::AgentRequest req(rank);
	
	std::vector<Agent*> nonLocalAgents;
	
	nonLocalAgents.clear();
	context.selectAgents(repast::SharedContext<Agent>::NON_LOCAL, nonLocalAgents, true);
	
	for(auto nlAgent : nonLocalAgents){
		req.addCancellation( nlAgent->getId() );
	}
	
	/*
	repast::SharedContext<Agent>::const_state_aware_iterator non_local_agents_iter  = context.begin(repast::SharedContext<Agent>::NON_LOCAL);
	repast::SharedContext<Agent>::const_state_aware_iterator non_local_agents_end   = context.end(repast::SharedContext<Agent>::NON_LOCAL);
	while(non_local_agents_iter != non_local_agents_end){
		req.addCancellation((*non_local_agents_iter)->getId());
		non_local_agents_iter++;
	}
	*/
	
	repast::RepastProcess::instance()->requestAgents<Agent, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, req, *provider, *receiver, *receiver);
	
	std::vector<repast::AgentId> cancellations = req.cancellations();
	
	for(auto idToRemove : cancellations){
		context.importedAgentRemoved(idToRemove);
	}
	
	/*
	std::vector<repast::AgentId> cancellations = req.cancellations();
	std::vector<repast::AgentId>::iterator idToRemove = cancellations.begin();
	while(idToRemove != cancellations.end()){
		context.importedAgentRemoved(*idToRemove);
		idToRemove++;
	}
	*/
}

void demoModel::removeLocalAgents(){
	uint32_t rank = repast::RepastProcess::instance()->rank();
	
	if(rank == 0) {
		std::cout << "REMOVING LOCAL AGENTS" << std::endl;
	}
	
	for(int i = 0; i < 5; i++){
		repast::AgentId id(i, rank, 0);
		repast::RepastProcess::instance()->agentRemoved(id);
		context.removeAgent(id);
	}
	
	repast::RepastProcess::instance()->synchronizeAgentStatus<Agent, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, *provider, *receiver, *receiver);
}

void demoModel::doSomething(){
	double currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	
	if(myRank == 0) {
		std::cout << "Time:"<< currTick << std::endl;
	}
	
	//double rndNumber = repast::Random::instance()->nextDouble();
	//std::cout << "Time:"<< currTick << " Rank:" << myRank << " is doing something. " << rndNumber << std::endl;
	
	
	this->viewState(" BEFORE PLAY: ", 0);
	
	std::vector<Agent*> agents;
	uint32_t totalSelectedAgents = 5; //totalAgents
	
	//Seleccionar agentes locales al proceso
	context.selectAgents(repast::SharedContext<Agent>::LOCAL,totalSelectedAgents, agents);
	
	for(auto fooAgent : agents){
		//std::cout << currTick << ": " << fooAgent->getId() <<  " ";
		fooAgent->play(&context);
	}
	//std::cout << std::endl;
	
	/*std::vector<Agent*>::iterator it = agents.begin();
	while(it != agents.end()){
		//std::cout << currTick << ": " <<(*it)->getId() <<  " ";
		(*it)->play(&context);
		it++;
	}
	//std::cout << std::endl;*/
	
	this->viewState(" AFTER PLAY, BEFORE SYNC: ", 0);
	repast::RepastProcess::instance()->synchronizeAgentStates<AgentPackage, AgentPackageProvider, AgentPackageReceiver>(*provider, *receiver);
	this->viewState(" AFTER SYNC: ", 0);
	
}

void demoModel::initSchedule(repast::ScheduleRunner& runner){
	auto action01 = new repast::MethodFunctor<demoModel> (this, &demoModel::doSomething);
	auto action02 = new repast::MethodFunctor<demoModel> (this, &demoModel::recordResults);
	
	auto actionRequest = new repast::MethodFunctor<demoModel> (this, &demoModel::requestAgents);
	auto actionCancelRequestAgent = new repast::MethodFunctor<demoModel> (this, &demoModel::cancelRequestAgents);
	auto actionRemoveLocalAgents = new repast::MethodFunctor<demoModel> (this, &demoModel::removeLocalAgents);
	
	
	runner.scheduleEvent(1, repast::Schedule::FunctorPtr( actionRequest ));
	runner.scheduleEvent(2, 1, repast::Schedule::FunctorPtr( action01 ));
	//runner.scheduleEvent(4, repast::Schedule::FunctorPtr( actionCancelRequestAgent ));
	runner.scheduleEvent(4, repast::Schedule::FunctorPtr( actionRemoveLocalAgents ));
	
	runner.scheduleEndEvent(repast::Schedule::FunctorPtr( action02 ));
	runner.scheduleStop(stopAt);
}

void demoModel::recordResults(){
	if(repast::RepastProcess::instance()->rank() == 0){
		props->putProperty("Result","Passed");
		std::vector<std::string> keyOrder;
		keyOrder.push_back("RunNumber");
		keyOrder.push_back("stop.at");
		keyOrder.push_back("Result");
		props->writeToSVFile("./output/results.csv", keyOrder);
    }
}

void demoModel::viewState(const std::string& label, const uint32_t& rankNumber){
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	std::set<Agent*> agents;
	if(myRank == rankNumber) {
		std::cout << label << std::endl;
		
		agents.clear();
		context.selectAgents(repast::SharedContext<Agent>::LOCAL, agents, true);
		
		for(auto localAgent : agents){
			std::cout << localAgent->getId() << " " << localAgent->getC() << " " << localAgent->getTotal() << std::endl;
		}
		
		agents.clear();
		context.selectAgents(repast::SharedContext<Agent>::NON_LOCAL, agents, true);
		
		for(auto nonLocalAgent : agents){
			std::cout << nonLocalAgent->getId() << " " << nonLocalAgent->getC() << " " << nonLocalAgent->getTotal() << std::endl;
		}
		
	}
	
	
	/*if(myRank == rankNumber) {
		std::cout << label << std::endl;
		auto local_agents_iter = context.localBegin();
		auto local_agents_end  = context.localEnd();
		while(local_agents_iter != local_agents_end){
			Agent* fooAgent = (&**local_agents_iter);
			std::cout << fooAgent->getId() << " " << fooAgent->getC() << " " << fooAgent->getTotal() << std::endl;
			local_agents_iter++;
		}
		
		auto non_local_agents_iter = context.begin(repast::SharedContext<Agent>::NON_LOCAL);
		auto non_local_agents_end  = context.end(repast::SharedContext<Agent>::NON_LOCAL);
		while(non_local_agents_iter != non_local_agents_end){
			Agent* fooAgent = (&**non_local_agents_iter);
			std::cout << fooAgent->getId() << " " << fooAgent->getC() << " " << fooAgent->getTotal() << std::endl;
			non_local_agents_iter++;
		}
	}*/
}

