#include <model.hh>
#include <agent.hh>

//##############################
SIRAgentPackageProvider::SIRAgentPackageProvider(repast::SharedContext<SIRAgent>* agentPtr): agents(agentPtr) { }

void SIRAgentPackageProvider::providePackage(SIRAgent* agent, std::vector<SIRAgentPackage>& out)
{
	repast::AgentId id = agent->getId();
	SIRAgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), agent->getState(), agent->getTiempoInfeccion());
	out.push_back(package);
}

void SIRAgentPackageProvider::provideContent(repast::AgentRequest req, std::vector<SIRAgentPackage>& out)
{
	std::vector<repast::AgentId> ids = req.requestedAgents();
	for(size_t i = 0; i < ids.size(); i++) {
		providePackage(agents->getAgent(ids[i]), out);
	}
}

SIRAgentPackageReceiver::SIRAgentPackageReceiver(repast::SharedContext<SIRAgent>* agentPtr): agents(agentPtr) {}

SIRAgent * SIRAgentPackageReceiver::createAgent(SIRAgentPackage package)
{
	repast::AgentId id(package._id, package._rank, package._type, package._currentRank);
	return new SIRAgent(id, package._agentState, package._tiempoInfeccion);
}

void SIRAgentPackageReceiver::updateAgent(SIRAgentPackage package)
{
	repast::AgentId id(package._id, package._rank, package._type);
	SIRAgent* agent = agents->getAgent(id);
	agent->set(package._currentRank, package._agentState, package._tiempoInfeccion);
}
//##############################


SIRModel::SIRModel(std::string& propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm), _comm(*comm)
{
	uint32_t myRank = repast::RepastProcess::instance()->rank();

	props = new repast::Properties(propsFile, argc, argv, comm);

	Global::totalIndividuos  = repast::strToInt(props->getProperty("totalIndividuos"));
	Global::tiempoSimulacion = repast::strToInt(props->getProperty("tiempoSimulacion"));
	Global::probInfeccion    = repast::strToDouble(props->getProperty("probInfeccion"));
	Global::radioInfeccion   = repast::strToInt(props->getProperty("radioInfeccion"));
	Global::probInmunidad    = repast::strToDouble(props->getProperty("probInmunidad"));
	Global::tiempoRecuperacion = repast::strToInt(props->getProperty("tiempoRecuperacion"));

	initializeRandom(*props, comm);


	provider = new SIRAgentPackageProvider(&context);
	receiver = new SIRAgentPackageReceiver(&context);

	// Establecer las dimensiones del Universo y agregarlo al SharedContext BEGIN
	repast::Point<double> origin(-100,-100);
	repast::Point<double> extent(200, 200);

	repast::GridDimensions gd(origin, extent);

	//Las dimensiones de la grilla deben
	//ser coherentes con las contidad de procesos MPI
	std::vector<int> processDims;
	processDims.push_back(2);
	processDims.push_back(2);

	discreteSpace = new repast::SharedDiscreteSpace<SIRAgent,\
	repast::WrapAroundBorders,\
	repast::SimpleAdder<SIRAgent> \
	>("AgentDiscreteSpace", gd, processDims, 2, comm);

	continuousSpace = new repast::SharedContinuousSpace<SIRAgent,\
	repast::WrapAroundBorders,\
	repast::SimpleAdder<SIRAgent> \
	>("AgentContinuousSpace", gd, processDims, 0, comm);

	//std::cout << "RANK " << myRank << " BOUNDS GLOBAL (discrete): " << discreteSpace->bounds() << "\n" \
	        << "RANK " << myRank << " BOUNDS LOCAL  (discrete): " << discreteSpace->dimensions() << "\n" \
	        << "RANK " << myRank << " BOUNDS GLOBAL (continuous): " << continuousSpace->bounds() << "\n" \
	        << "RANK " << myRank << " BOUNDS LOCAL  (continuous): " << continuousSpace->dimensions() << std::endl;

	context.addProjection(continuousSpace);
	context.addProjection(discreteSpace);
	// Establecer las dimensiones del Universo y agregarlo al SharedContext END

}

SIRModel::~SIRModel()
{
	delete props;
	delete provider;
	delete receiver;
}

void SIRModel::init()
{
	uint32_t myRank = repast::RepastProcess::instance()->rank();

	int32_t localOriginX = (int32_t)discreteSpace->dimensions().origin().getX();
	int32_t localOriginY = (int32_t)discreteSpace->dimensions().origin().getY();

	int32_t widthLocalGrid  = (int32_t)discreteSpace->dimensions().extents().getX() ;
	int32_t heightLocalGrid = (int32_t)discreteSpace->dimensions().extents().getY() ;

	//std::cout << "RANK " << myRank << ": Origin:(" <<  localOriginX << "," << localOriginY << ")" \
	        << " Dimensions: (" << widthLocalGrid << "," << heightLocalGrid << ")" \
	        << std::endl;

	for(size_t idCorr = 0; idCorr < Global::totalIndividuos; idCorr++) {
		repast::Point<double> initialLocationC((double)continuousSpace->dimensions().origin().getX() + repast::Random::instance()->nextDouble()*widthLocalGrid,\
		                                       (double)continuousSpace->dimensions().origin().getY() + repast::Random::instance()->nextDouble()*heightLocalGrid);

		repast::Point<int> initialLocationD( (int)(floor(initialLocationC[0])), (int)(floor(initialLocationC[1])) );


		repast::AgentId id(idCorr, myRank, 0);
		id.currentRank(myRank);
		SIRAgent* agent = new SIRAgent(id);
		context.addAgent(agent);
		discreteSpace->moveTo(id, initialLocationD);
		continuousSpace->moveTo(id, initialLocationC);
	}

}

void SIRModel::synchronizeBorders()
{

	discreteSpace->balance();

	repast::RepastProcess::instance()->synchronizeAgentStatus<SIRAgent, \
	SIRAgentPackage, \
	SIRAgentPackageProvider, \
	SIRAgentPackageReceiver>(context, *provider, *receiver, *receiver);

	repast::RepastProcess::instance()->synchronizeProjectionInfo<SIRAgent, \
	SIRAgentPackage, \
	SIRAgentPackageProvider, \
	SIRAgentPackageReceiver>(context, *provider, *receiver, *receiver);

	repast::RepastProcess::instance()->synchronizeAgentStates<SIRAgentPackage, \
	SIRAgentPackageProvider, \
	SIRAgentPackageReceiver>(*provider, *receiver);
}

void SIRModel::initSchedule(repast::ScheduleRunner& runner)
{
	uint32_t myRank = repast::RepastProcess::instance()->rank();

	auto action_checkStatus        = new repast::MethodFunctor<SIRModel> (this, &SIRModel::checkStatus);
	auto action_synchronizeBorders = new repast::MethodFunctor<SIRModel> (this, &SIRModel::synchronizeBorders);

	// Primero, sincronize los border de todo el universo
	runner.scheduleEvent(0, repast::Schedule::FunctorPtr( action_synchronizeBorders ));
	// A partir de t=1, repita cada dt=1, la función action_checkStatus
	runner.scheduleEvent(1, 1, repast::Schedule::FunctorPtr( action_checkStatus ));

	//Detener la simulación en el tiempo especificado
	runner.scheduleStop(Global::tiempoSimulacion);
}

void SIRModel::checkStatus()
{
	uint32_t myRank;
	double currTick;

	myRank   = repast::RepastProcess::instance()->rank();
	currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();

	//std::cout << currTick << "\tRank:" << myRank << " is checking status ..." << std::endl;


	std::vector<SIRAgent*> agents;

	if(context.size() > 0) {
		context.selectAgents(repast::SharedContext<SIRAgent>::LOCAL, agents, true);
	}

	for(auto fooAgent : agents) {
		fooAgent->move(discreteSpace, continuousSpace);
	}

	for(auto fooAgent : agents) {
		if( fooAgent->getState() == I ) {
			fooAgent->applyRules(&context, discreteSpace, continuousSpace);
		}
	}

	uint32_t SusceptiblesParcial = 0;
	uint32_t InfectadosParcial   = 0;
	uint32_t RecuperadosParcial  = 0;
	for(auto fooAgent : agents) {
		switch(fooAgent->getState()) {
		case S:
			SusceptiblesParcial++;
			break;
		case I:
			InfectadosParcial++;
			break;
		case R:
			RecuperadosParcial++;
			break;
		}
	}

	//std::cout  << currTick << ";myRank:" << myRank << ";" \
	        << SusceptiblesParcial << ";" \
	        << InfectadosParcial << ";" \
	        << RecuperadosParcial << std::endl;

	//Enviar cantidad de S,I,R al proceso 0 para que consolide los datos
	uint32_t Susceptibles;
	uint32_t Infectados;
	uint32_t Recuperados;

	bool pararSimulacion = false;

	auto sum = [](const uint32_t& a, const uint32_t& b) {
		return(a+b);
	};

	boost::mpi::reduce(_comm, SusceptiblesParcial, Susceptibles, sum, 0);
	boost::mpi::reduce(_comm, InfectadosParcial, Infectados, sum, 0);
	boost::mpi::reduce(_comm, RecuperadosParcial, Recuperados, sum, 0);

	if(myRank == 0) {
		std::cout << currTick << ":" \
		          << Susceptibles << ":" \
		          << Infectados << ":" \
		          << Recuperados << std::endl;

		//Si no hay infectados, hay que parar la simulación
		pararSimulacion = !Infectados;
	}

	boost::mpi::broadcast(_comm, pararSimulacion, 0);
	//std::cout << currTick << "->" << myRank << " : " << pararSimulacion << '\n';
	if(pararSimulacion) { //Cada proceso finaliza su correspondiente scheduler
		repast::RepastProcess::instance()->getScheduleRunner().stop();
	}


	synchronizeBorders();

}








