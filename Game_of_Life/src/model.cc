#include <model.hh>
#include <agent.hh>

//##############################
GolAgentPackageProvider::GolAgentPackageProvider(repast::SharedContext<GolAgent>* agentPtr): agents(agentPtr){ }

void GolAgentPackageProvider::providePackage(GolAgent* agent, std::vector<GolAgentPackage>& out){
    repast::AgentId id = agent->getId();
    GolAgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), agent->getState(), agent->getProbLive());
    out.push_back(package);
}

void GolAgentPackageProvider::provideContent(repast::AgentRequest req, std::vector<GolAgentPackage>& out){
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for(size_t i = 0; i < ids.size(); i++){
        providePackage(agents->getAgent(ids[i]), out);
    }
}

GolAgentPackageReceiver::GolAgentPackageReceiver(repast::SharedContext<GolAgent>* agentPtr): agents(agentPtr){}

GolAgent * GolAgentPackageReceiver::createAgent(GolAgentPackage package){
    repast::AgentId id(package._id, package._rank, package._type, package._currentRank);
    return new GolAgent(id, package._probLiveAgent, package._agentState);
}

void GolAgentPackageReceiver::updateAgent(GolAgentPackage package){
    repast::AgentId id(package._id, package._rank, package._type);
    GolAgent* agent = agents->getAgent(id);
    agent->set(package._currentRank, package._probLiveAgent, package._agentState);
}
//##############################


GolModel::GolModel(std::string& propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm){
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	props = new repast::Properties(propsFile, argc, argv, comm);
	duration = repast::strToInt(props->getProperty("duration"));
	
	totalGolAgents = repast::strToInt(props->getProperty("totalGolAgents"));
	probLiveAgent  = repast::strToDouble(props->getProperty("probLiveAgent"));
	
	initializeRandom(*props, comm);
	
	
	provider = new GolAgentPackageProvider(&context);
	receiver = new GolAgentPackageReceiver(&context);
	
	// Establecer las dimensiones del Universo y agregarlo al SharedContext BEGIN
	repast::Point<double> origin(-10,-10);
	repast::Point<double> extent(20, 20);
	
	repast::GridDimensions gd(origin, extent);
	
	//Las dimensiones de la grilla deben 
	//ser coherentes con las contidad de procesos MPI
	std::vector<int> processDims;
	processDims.push_back(2);
	processDims.push_back(2);
	
	discreteSpace = new repast::SharedDiscreteSpace<GolAgent,\
													repast::WrapAroundBorders,\
													repast::SimpleAdder<GolAgent> \
													>("AgentDiscreteSpace", gd, processDims, 1, comm);
													
	std::cout << "RANK " << myRank << " BOUNDS GLOBAL: " << discreteSpace->bounds() << std::endl;
	std::cout << "RANK " << myRank << " BOUNDS LOCAL : " << discreteSpace->dimensions() << std::endl;
    context.addProjection(discreteSpace);										
	// Establecer las dimensiones del Universo y agregarlo al SharedContext END
	
}

GolModel::~GolModel(){
	delete props;
	delete provider;
	delete receiver;
}

void GolModel::init(){
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	int32_t localOriginX = (int32_t)discreteSpace->dimensions().origin().getX();
	int32_t localOriginY = (int32_t)discreteSpace->dimensions().origin().getY();
	
	int32_t widthLocalGrid  = (int32_t)discreteSpace->dimensions().extents().getX() ;
	int32_t heightLocalGrid = (int32_t)discreteSpace->dimensions().extents().getY() ;
	
	std::cout << "RANK " << myRank << ": Origin:(" <<  localOriginX << "," << localOriginY << ")" << \
		" Dimensions: (" << widthLocalGrid << "," << heightLocalGrid << ")" << \
		std::endl;
	
	for(size_t y = 0; y < heightLocalGrid; y++){
		for(size_t x = 0; x < widthLocalGrid; x++){
			uint32_t idCorr = x + y*widthLocalGrid;
			repast::Point<int> initialLocation( (int)discreteSpace->dimensions().origin().getX() + x,\
												(int)discreteSpace->dimensions().origin().getY() + y );
			
			repast::AgentId id(idCorr, myRank, 0);
			id.currentRank(myRank);
			GolAgent* agent = new GolAgent(id, probLiveAgent);
			context.addAgent(agent);
			discreteSpace->moveTo(id, initialLocation);
		}
	}
	
}

void GolModel::synchronizeBorders(){
	
	discreteSpace->balance();
	
	repast::RepastProcess::instance()->synchronizeAgentStatus<GolAgent, \
		GolAgentPackage, \
		GolAgentPackageProvider, \
		GolAgentPackageReceiver>(context, *provider, *receiver, *receiver);
	
	repast::RepastProcess::instance()->synchronizeProjectionInfo<GolAgent, \
		GolAgentPackage, \
		GolAgentPackageProvider, \
		GolAgentPackageReceiver>(context, *provider, *receiver, *receiver);
    
	repast::RepastProcess::instance()->synchronizeAgentStates<GolAgentPackage, \
		GolAgentPackageProvider, \
		GolAgentPackageReceiver>(*provider, *receiver);
}

void GolModel::initSchedule(repast::ScheduleRunner& runner){
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	auto action_checkStatus        = new repast::MethodFunctor<GolModel> (this, &GolModel::checkStatus);
	auto action_synchronizeBorders = new repast::MethodFunctor<GolModel> (this, &GolModel::synchronizeBorders);
	
	// Primero, sincronize los border de todo el universo
	runner.scheduleEvent(0, repast::Schedule::FunctorPtr( action_synchronizeBorders ));
	// A partir de t=1, repita cada dt=1, la función action_checkStatus
	runner.scheduleEvent(1, 1, repast::Schedule::FunctorPtr( action_checkStatus ));

	//Detener la simulación en el tiempo especificado
	runner.scheduleStop(duration);
}

void GolModel::checkStatus(){
	uint32_t myRank;
	double currTick;
	
	myRank   = repast::RepastProcess::instance()->rank();
	currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
	
	std::cout << currTick << "\tRank:" << myRank << " is checking status ..." << std::endl;
	
	
	std::vector<GolAgent*> agents;
	
	if(context.size() > 0){
		context.selectAgents(repast::SharedContext<GolAgent>::LOCAL, agents, true);
	}
	
	for(auto fooAgent : agents){
		fooAgent->applyRules(&context, discreteSpace);
		
		//std::vector<int> agentLoc;
		//discreteSpace->getLocation(fooAgent->getId(), agentLoc);
		//repast::Point<int> agentLocation(agentLoc);
		//std::cout  << currTick << " Location: myRank:" << myRank << ": "<< fooAgent->getId() << " @ " << agentLocation << std::endl;
		
	}
	
	for(auto fooAgent : agents){
		fooAgent->updateState();
	}
	
	synchronizeBorders();
	
}








