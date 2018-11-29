#include <model.hh>
#include <environment.hh>


AgentPackageProvider::AgentPackageProvider(Environment* agentPtr): agents(agentPtr){
	
}

void AgentPackageProvider::providePackage(Agent * agent, std::vector<AgentPackage>& out){
    repast::AgentId id = agent->getId();
	
    AgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), 
	                     agent->getMinSpeed(),
						 agent->getMaxSpeed(),
	                     //agent->getTargetPos(),
						 agent->getPosition()[0], agent->getPosition()[1],
	                     agent->getDirection()[0], agent->getDirection()[1],
						 agent->getDisiredSpeed(),
	                     agent->getMaxDisiredSpeed(),
						 agent->getCurrVelocity()[0], agent->getCurrVelocity()[1],
						 agent->getRoute(),
	                     agent->getTimeRelax(),
						 agent->getSigma(),
	                     agent->getStrengthSocialRepulsiveForceAgents(),
						 agent->getCosPhi()
						);
    out.push_back(package);
}


void AgentPackageProvider::provideContent(repast::AgentRequest req, std::vector<AgentPackage>& out){
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for(size_t i = 0; i < ids.size(); i++){
        providePackage(agents->getAgent(ids[i]), out);
    }
}


AgentPackageReceiver::AgentPackageReceiver(Environment* agentPtr): agents(agentPtr){
	
}

Agent * AgentPackageReceiver::createAgent(AgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type, package.currentRank);
	Point2D position = Point2D(package._positionX, package._positionY);
	Vector2D direction = Vector2D(package._directionX , package._directionY);
	Vector2D currVelocity = Vector2D(package._currVelocityX , package._currVelocityY);
	
	std::list<Point2D> route;
	
	std::string separador = ";";	
	std::string routeStr  = package._route;
	size_t index = routeStr.find(separador);
	while(index != std::string::npos){
		std::string  campo = routeStr.substr(0, index);
		size_t idx = campo.find(",");
		
		Point2D fooP(std::stof(campo.substr(0,idx)) , std::stof(campo.substr(idx+1, campo.length() )));
		route.push_back(fooP);
		
		routeStr.erase(0, index + separador.length());
		
		index = routeStr.find(separador);
	}
	
    return new Agent(id,
		package._min_speed,
		package._max_speed,
		//package._targetPos, 
		//package._position,
		position,
		//package._direction,
		direction,
		package._disiredSpeed,
		package._maxDisiredSpeed,
		//package._currVelocity,
		currVelocity,
		route,
		package._timeRelax,
		package._sigma,    
		package._strengthSocialRepulsiveForceAgents,
		package._cosPhi
		);
}

void AgentPackageReceiver::updateAgent(AgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type);
	Point2D position = Point2D(package._positionX, package._positionY);
	Vector2D direction = Vector2D(package._directionX , package._directionY);
	Vector2D currVelocity = Vector2D(package._currVelocityX , package._currVelocityY);
	
	std::list<Point2D> route;
	
	std::string separador = ";";	
	std::string routeStr  = package._route;
	size_t index = routeStr.find(separador);
	while(index != std::string::npos){
		std::string  campo = routeStr.substr(0, index);
		size_t idx = campo.find(",");
		
		Point2D fooP(std::stof(campo.substr(0,idx)) , std::stof(campo.substr(idx+1, campo.length() )));
		route.push_back(fooP);
		
		routeStr.erase(0, index + separador.length());
		
		index = routeStr.find(separador);
	}
	
    Agent * agent = agents->getAgent(id);
    agent->set(package.currentRank,
		package._min_speed,
		package._max_speed,
		//package._targetPos, 
		//package._position,
		position,
		//package._direction,
		direction,
		package._disiredSpeed,
		package._maxDisiredSpeed,
		//package._currVelocity,
		currVelocity,
		route,
		package._timeRelax,
		package._sigma,    
		package._strengthSocialRepulsiveForceAgents,
		package._cosPhi
	);

}


Model::Model(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm){
	uint32_t myRank    = repast::RepastProcess::instance()->rank();
	
	props       = new repast::Properties(propsFile, argc, argv, comm);
	//stopAt      = repast::strToInt(props->getProperty("stopAt"));
	stopAt      = Global::settings["duration"];
	stopAt     += 0.9;
	
	//totalAgents = repast::strToInt(props->getProperty("totalAgents"));
	
	initializeRandom(*props, comm);
	
	_comm = comm;
	
	//Agregar propiedades al context
	
	//context.setReferencePoint(Global::reference_point);
	context.setReferencePoint(Global::area_zone);
	context.setRouter(Global::map_osrm);
	context.setProjector();
	context.setReferenceZones(Global::reference_zones);
	context.setInitialZones(Global::initial_zones);

	context.setGrid(Global::area_zone);
	
	if(myRank == 0){
		context.showGrid();
	}
	
	if(repast::RepastProcess::instance()->rank() == 0) props->writeToSVFile("./output/record.csv");
	
	provider = new AgentPackageProvider(&context);
	receiver = new AgentPackageReceiver(&context);
	
	//Código para spatial projection 
	repast::Point<double> origin(context.getGrid()._xMin, context.getGrid()._yMin);
    repast::Point<double> extent(context.getGrid()._mapWidth, context.getGrid()._mapHeight);

    repast::GridDimensions gd(origin, extent);

    std::vector<int> processDims;
    processDims.push_back(Global::settings["processDims"]["x"].get<uint32_t>());
    processDims.push_back(Global::settings["processDims"]["y"].get<uint32_t>());

	uint32_t borders = Global::settings["processDims"]["borders"].get<uint32_t>();

    discreteSpace   = new repast::SharedDiscreteSpace<  Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >("AgentDiscreteSpace", gd, processDims, borders, comm);
	continuousSpace = new repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >("AgentContinuousSpace", gd, processDims, borders, comm);
	
    std::cout << "RANK " << repast::RepastProcess::instance()->rank() << 
		        " BOUNDS: " << continuousSpace->bounds().origin() << " " << 
				continuousSpace->bounds().extents() << 
				std::endl;
    
   	context.addProjection(discreteSpace);
	context.addProjection(continuousSpace);
	//Fin Código para spatial projection 
	
}

Model::~Model(){
	delete props;
}

void Model::init(){
	static thread_local std::random_device device;
	static thread_local std::mt19937 rng(device());
	
	uint32_t myRank    = repast::RepastProcess::instance()->rank();
	uint32_t worldSize = repast::RepastProcess::instance()->worldSize();
	//std::cout << "RANK " << myRank << ",TotalAgents: " << totalAgents << std::endl;
	
	std::cout << "Rank:" << myRank << ", wSize:" << worldSize << ": Creando agentes..." << std::endl;	
	
	auto start = std::chrono::system_clock::now(); //Measure Time
	
	std::uniform_int_distribution<uint32_t> zone(0, context.getInitialZones().size()-1);
	ProgressBar pg;
	uint32_t idNumber = 0;
	for(auto& fagent : Global::settings["agents"]) {
		uint32_t totalAgents  = fagent["number"].get<uint32_t>();
		
		totalAgents /= worldSize;
		
		std::string modelName = fagent["model"].get<std::string>();
		uint32_t modelID      = model_t[modelName];
		
		std::cout << "Rank:" << myRank << ": Creando agentes " << totalAgents << " " <<  modelName << std::endl;	
		
		json SocialForceModel = fagent["SFM"];
		
		pg.start(totalAgents);
		for(uint32_t i = 0; i < totalAgents; i++, idNumber++) {
			if(Global::showProgressBar){pg.update(i);}
			
			repast::AgentId id(totalAgents*myRank + idNumber, myRank, modelID);
			id.currentRank(myRank);
			
			Point2D position = context.getInitialZone(zone(rng)).generate();
			
			Agent* agent = new Agent(id, \
				position,\
				fagent["speed"]["min"].get<double>(),\
				fagent["speed"]["max"].get<double>(),\
				SocialForceModel);
			
			context.addAgent(agent);
			
			repast::Point<int> initialLocationDiscrete(\
				(int)position[0],\
				(int)position[1]\
			);
		
			repast::Point<double> initialLocationContinuous(\
				(double)position[0],\
				(double)position[1]\
			);
			
			discreteSpace->moveTo(id, initialLocationDiscrete);
			continuousSpace->moveTo(id, initialLocationContinuous);
			
			
		}
	}
	
	auto end = std::chrono::system_clock::now(); //Measure Time
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);//Measure Time
	Counters::timeExecMakeAgents += elapsed.count();//Measure Time
	
	if(Global::showProgressBar){std::cout << std::endl;}

}

void Model::calibrate(){
	uint32_t myRank    = repast::RepastProcess::instance()->rank();
	
	std::cout << "Rank:" << myRank  << ": Ajustando posición inicial de los agentes..." << std::endl;
	uint32_t calibrationTime = Global::settings["calibration"].get<uint32_t>();
	
	auto start = std::chrono::system_clock::now(); //Measure Time
	adjustAgentsInitialPosition(calibrationTime);
	
	std::cout << "Rank:" << myRank << ": Ajustando reglas de los agentes... " <<  std::endl;
	adjustAgentsRules();
	
	auto end = std::chrono::system_clock::now(); //Measure Time
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);//Measure Time
	Counters::timeExecCalibrate += elapsed.count();//Measure Time
}

void Model::simulate(){
	double currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	
	std::vector<Agent*> agents;
	agents.clear();
	
	
	/////
	/*
	std::cout << "NON LOCAL AGENTS:" << std::endl;
	agents.clear();
	context.selectAgents(repast::SharedContext<Agent>::NON_LOCAL, agents, true);
	
	for(auto nonLocalAgent : agents){
		std::vector<double> agentLoc;
        continuousSpace->getLocation(nonLocalAgent->getId(), agentLoc);
        repast::Point<double> agentLocation(agentLoc);
        std::cout << currTick << " > NL:" << myRank << " ==> " << nonLocalAgent->getId() << "@" << agentLocation << std::endl;
	}
	
	*/
	////
	
	if(context.size() > 0){
		//context.selectAgents(repast::SharedContext<Agent>::LOCAL, agents, true);
		context.selectAgents(repast::SharedContext<Agent>::LOCAL, agents, true);
	}
	
	uint32_t totalAgents = agents.size();
	

	std::cout << "Rank:" << myRank << ", Time:"<< currTick << ", " << totalAgents << " / " << context.size() \
		<< " , "<< context.size() - totalAgents << std::endl;

	
	auto start = std::chrono::system_clock::now(); //Measure Time
	
	for(auto fooAgent : agents){
		//std::cout << currTick << ": " << fooAgent->getId() <<  " ";
		fooAgent->update(&context, discreteSpace, continuousSpace);	
	}
	
	for(auto fooAgent : agents){
		//std::cout << currTick << ": " << fooAgent->getId() <<  " ";
		fooAgent->move(discreteSpace, continuousSpace);
	}

	
	auto end = std::chrono::system_clock::now(); //Measure Time
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);//Measure Time

	Counters::timeExecSimulate += elapsed.count(); //Measure Time
	

	discreteSpace->balance(); //??? ????
	
	//continuousSpace->balance();
	repast::RepastProcess::instance()->synchronizeAgentStatus<Agent, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, *provider, *receiver, *receiver);				 
	repast::RepastProcess::instance()->synchronizeProjectionInfo<Agent, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, *provider, *receiver, *receiver);
    repast::RepastProcess::instance()->synchronizeAgentStates<AgentPackage, AgentPackageProvider, AgentPackageReceiver>(*provider, *receiver);
    
	
	/*
	
	Calls the 'balance' method on the space. 
			The balance method marks the agents that have moved into the buffer zones to be moved 
			to the adjacent processes.
	Calls 'synchronizeAgentStatus', 
			which performs the move; agents that had moved out of the local boundaries of one process are moved to 
			the appropriate adjacent process.
	Calls 'synchronizeProjectionInfo',
		 	which will copy the agents within the local boundaries but inside some other process's buffer zone so that 
			these agents are visible on the other processes.
	Calls 'synchronizeAgentStates',
		 	which performs a final update of all non-local agents so that they have the correct, current state of the 
			local originals
	*/
	
	
}

void Model::stats(){
	double currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	std::vector<Agent*> agents;
	agents.clear();
	if(context.size() > 0){
		context.selectAgents(repast::SharedContext<Agent>::LOCAL, agents, true);
	}
	
	uint32_t totalAgents = agents.size();
	
	//if(myRank == 0) {
	//	std::cout << "Rank:" << myRank << ", Time:"<< currTick << " Gen. stats" <<std::endl;
	//}
	
	for(uint32_t i = 0; i < totalAgents; i++){	
		//std::cout << i << std::flush << std::endl;
		Agent* agent = agents[i];	
		
		for(auto& reference_zone : context.getReferenceZones() ) { 
			bool isInside;
			isInside = reference_zone.pointIsInside(agent->position());
	
			if(isInside){
				//#pragma omp critical
				{
				reference_zone.addAgent(agent->getId());				
				reference_zone.updateAgentsDensity();	
				}	
			}
			else{
				if(reference_zone.getAgentDensity() > 0){
					//#pragma omp critical
					{
					reference_zone.deleteAgent(agent->getId());				
					reference_zone.updateAgentsDensity();	
					}
				}	
			}	
		}
		
	}
	
	
	for(auto& reference_zone : context.getReferenceZones() ) {
		std::string logString;
		logString = reference_zone.getNameID() + ":" +  std::to_string( (uint32_t)currTick) + ":" + std::to_string(reference_zone.getAgentDensity());					
		Stats::logZonesDensity.push_back(logString);
	}

}

void Model::progress(){
	std::cout << "." << std::flush;
}

void Model::initSchedule(repast::ScheduleRunner& runner){
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	auto actionCalibrate       = new repast::MethodFunctor<Model> (this, &Model::calibrate);
	auto actionSimulate        = new repast::MethodFunctor<Model> (this, &Model::simulate);
	auto actionSaveStateToFile = new repast::MethodFunctor<Model> (this, &Model::saveStateToFile);
	auto actionStats           = new repast::MethodFunctor<Model> (this, &Model::stats);
	auto actionProgress        = new repast::MethodFunctor<Model> (this, &Model::progress);
	auto actionSaveStatsToFile = new repast::MethodFunctor<Model> (this, &Model::saveStatsToFile);

	
	runner.scheduleEvent(0, 1, repast::Schedule::FunctorPtr( actionSimulate ));

	
	
	bool     saveToDisk = Global::settings["output"]["filesim-out"].get<bool>();
	bool     statsOut   = Global::settings["output"]["stats-out"].get<bool>();
	uint32_t interval   = Global::settings["output"]["interval"].get<uint32_t>();
	
	if(Global::showProgressBar && (myRank == 0) ){
		runner.scheduleEvent(0, interval, repast::Schedule::FunctorPtr( actionProgress ));
	}
		
	//if( saveToDisk && (myRank == 0) ){	
	if( saveToDisk ){	
		runner.scheduleEvent(0.1, interval, repast::Schedule::FunctorPtr( actionSaveStateToFile ));
	}
	
	if( statsOut && (myRank == 0)){
		uint32_t statsInterval = Global::settings["output"]["stats-interval"].get<uint32_t>();
		runner.scheduleEvent(0.2, statsInterval, repast::Schedule::FunctorPtr( actionStats ));
		runner.scheduleEndEvent(repast::Schedule::FunctorPtr( actionSaveStatsToFile ));
	}
	
	runner.scheduleStop(stopAt);
}

/**
* @brief Ajusta la posición inicial de los agentes del Environment
*
*A cada agente que pertenezca al Environment, se le
*ajusta su posición incial en el mapa. Esto es debido a que
*la posición inicial del agente puede estar en un lugar donde
*no es necesariamente una calle. Esto se realiza en dos pasos:
*
* 1) Se ajusta el modelo de movilidad de todos los agentes a RANDOMWALK
* 2) Los agentes caminan un tiempo determinado para que finalmente
*    queden en las calles
*
*Este método es llamado por
*el método Model::calibrate()
*
* @param void
* @return void
*/
void Model::adjustAgentsInitialPosition(const uint32_t& calibrationTime){
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	std::vector<Agent*> agents;
	agents.clear();
	if(context.size() > 0){
		context.selectAgents(repast::SharedContext<Agent>::LOCAL, agents, true);
	}
	
	uint32_t totalAgents = agents.size();
	
	
	//
	// AJUSTE: PASO 1	
	//
	std::cout << "Rank:" << myRank << ": Ajustando posición inicial de los agentes: total:" << totalAgents <<std::endl;
	std::cout << "Rank:" << myRank << ": Ajustando posición inicial de los agentes...(1/2)" << std::endl;
	
	ProgressBar pg;
	pg.start(totalAgents);
	
	//#pragma omp parallel for 
	for(uint32_t i = 0; i < totalAgents; i++){
		if(Global::showProgressBar){
			pg.update(i);
		}
		
		Agent* agent = agents[i];
	
		auto response = context.getRouter()->route(agent->position(),Global::randomWalkwayRadius);
		agent->_route = response.path();
	}

	if(Global::showProgressBar){
		std::cout << std::endl;
	}

	//
	// AJUSTE: PASO 2
	//
	std::cout << "Rank:" << myRank << ": Ajustando posición inicial de los agentes...(2/2)" << std::endl;
	pg.start(calibrationTime-1);
	for(uint32_t t = 0; t < calibrationTime; t++) {
		if(Global::showProgressBar){
			pg.update(t);
		}	

		//#pragma omp parallel for 
		for(uint32_t i = 0; i < totalAgents; i++){
			Agent* agent = agents[i];
			
			if(agent->_route.empty()){
				auto response = context.getRouter()->route(agent->position(),Global::randomWalkwayRadius);
				agent->_route = response.path();
			}
			agent->randomWalkwayForAdjustInitialPosition(&context, discreteSpace, continuousSpace);
			agent->move(discreteSpace, continuousSpace);	
		}
		
		
		//discreteSpace->balance();
		
		//continuousSpace->balance();
	    repast::RepastProcess::instance()->synchronizeAgentStatus<Agent, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, *provider, *receiver, *receiver);				 
		repast::RepastProcess::instance()->synchronizeProjectionInfo<Agent, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, *provider, *receiver, *receiver);
	    repast::RepastProcess::instance()->synchronizeAgentStates<AgentPackage, AgentPackageProvider, AgentPackageReceiver>(*provider, *receiver);
		
	}

	if(Global::showProgressBar){std::cout << std::endl;}
	
}


/**
* @brief Ajusta las reglas iniciales de los agentes del Environment
*
*A cada agente que pertenezca al Environment, se le
*ajusta sus reglas. Este método es llamado por
*el método Model::calibrate()
*
* @param void
* @return void
*/
void Model::adjustAgentsRules(){
	std::vector<Agent*> agents;
	agents.clear();
	
	if(context.size() > 0){
		context.selectAgents(repast::SharedContext<Agent>::LOCAL, agents, true);
	}
	
	uint32_t totalAgents = agents.size();
	
	ProgressBar pg;
	pg.start(totalAgents-1);
	
	//#pragma omp parallel for
	for(uint32_t i = 0; i < totalAgents; i++){
		if(Global::showProgressBar){
			pg.update(i);
		}
			
		Agent* agent = agents[i];

		switch( agent->getId().agentType() ) {
			case ShortestPath: {
				double distance = DBL_MAX;
				Point2D  fooTarget;
				for(auto &reference_zone : context.getReferenceZones()) { 				
					//VERSION 3
					// se basa en calcular la distancia 
					// entre el agente y una zona de referencia
					// a través de la distancia euclideana. El error cometido es del orden
					// del 18% para el mapa de Iquique, Q1=12.04, Q3=21.62. 
					// Se logra un SpeedUp de 2.8 comparado con la V1
					fooTarget = reference_zone.generate();
					double fooDistance = sqrt(CGAL::squared_distance(agent->position(), fooTarget));
								
					if( fooDistance < distance ){
						distance = fooDistance;
						agent->setTargetPos(fooTarget);
					}
					
				}
				
				//VERSION 3
				auto response = context.getRouter()->route(agent->position(),agent->getTargetPos());
				agent->_route = response.path();
				
				break;
			}
			case FollowTheCrowd:  {					            
				break;
			}
			case RandomWalkway: {
				break;
			}
		}				
	}
	if(Global::showProgressBar){std::cout << std::endl;}
	
	
}


void Model::viewState(const std::string& label, const uint32_t& rankNumber){
	uint32_t myRank = repast::RepastProcess::instance()->rank();
	
	std::set<Agent*> agents;
	if(myRank == rankNumber) {
		std::cout << label << std::endl;
		
		agents.clear();
		
		if(context.size() > 0){
			context.selectAgents(repast::SharedContext<Agent>::LOCAL, agents, true);
		}
		
		for(auto localAgent : agents){
			std::cout << localAgent->getId() << std::endl;
		}
		
		agents.clear();
		context.selectAgents(repast::SharedContext<Agent>::NON_LOCAL, agents, true);
		
		for(auto nonLocalAgent : agents){
			std::cout << nonLocalAgent->getId() << std::endl;
		}
		
	}
	
}

void Model::saveStateToFile(){
	uint32_t currTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
	uint32_t myRank   = repast::RepastProcess::instance()->rank();
	
	std::ostringstream ss;
	ss << std::setw( 10 ) << std::setfill( '0' ) << currTick;
	
	std::ostringstream ssRank;
	ssRank << std::setw( 3 ) << std::setfill( '0' ) << myRank;
	
	std::string filesimPrefix   = Global::settings["output"]["filesim-prefix"].get<std::string>();
	std::string filesimSufix    = Global::settings["output"]["filesim-sufix"].get<std::string>();
	std::string filesimPath     = Global::settings["output"]["filesim-path"].get<std::string>();
		
	std::string nameFile = filesimPrefix + ss.str() + "-" + ssRank.str() + filesimSufix ;
	//std::string nameFile = filesimPrefix + ss.str() + filesimSufix ;
	std::string pathFile = filesimPath + "/" + nameFile ;
	
	
	std::ofstream ofs(pathFile);
	
	std::set<Agent*> agents;
	agents.clear();
	
	if(context.size() > 0){
		//context.selectAgents(repast::SharedContext<Agent>::LOCAL, agents, true);
		context.selectAgents(agents, true);
	}
		
	for( auto agent : agents ) {
		double latitude,longitude,h;
		context.getProjector().Reverse(agent->position()[0],agent->position()[1],0,latitude,longitude,h); 
		ofs << agent->getId().id() << " " 
			<< latitude << " " << longitude << " " 
			<< agent->getId().agentType() 
			<< std::endl;
	}
}

void Model::saveStatsToFile(){
	std::string statsPath = Global::settings["output"]["stats-path"].get<std::string>();
	
	std::string pathFile = statsPath + "/zonesDensity.txt" ;
	std::ofstream ofs(pathFile);
	for(auto& item : Stats::logZonesDensity){
		ofs << item << std::endl;
	}
}

void Model::showTimeExec(){
	uint32_t myRank   = repast::RepastProcess::instance()->rank();
	
	std::cout << myRank << ":" << Global::settings["duration"] << ":"
		<< Global::settings["calibration"] << ":"
		<< Global::settings["agents"][0]["number"] << ":"
		<< Global::settings["agents"][1]["number"] << ":"
		<< Global::settings["agents"][2]["number"] << ":"
		<< Counters::timeExecMakeAgents  << ":"
		<< Counters::timeExecCalibrate << ":"
		<< Counters::timeExecSimulate 
		<< std::endl;

	
}

