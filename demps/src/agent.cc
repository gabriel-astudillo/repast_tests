#include <agent.hh>
#include <environment.hh>

Agent::Agent(repast::AgentId id, double min_speed, double max_speed, 
       //Point2D  targetPos,
	   Point2D  position, 
	   Vector2D direction, 
	   double disiredSpeed, double  maxDisiredSpeed,
	   Vector2D currVelocity, 
	   std::list<Point2D> route,
	   double timeRelax, double sigma, double strengthSocialRepulsiveForceAgents,
	   double cosPhi){
	   	
	_id        = id;
	_min_speed = min_speed;
	_max_speed = max_speed;
	//_targetPos = targetPos; 
	_position  = position;
	_direction = direction;
	_disiredSpeed    = disiredSpeed;
	_maxDisiredSpeed = maxDisiredSpeed;
	_currVelocity    = currVelocity;
	_route = route;
	_timeRelax = timeRelax;
	_sigma     = sigma;    
	_strengthSocialRepulsiveForceAgents = strengthSocialRepulsiveForceAgents;
	_cosPhi = cosPhi; 
}

void Agent::set(uint32_t currentRank, double min_speed, double max_speed, 
       //Point2D  targetPos,
       Point2D  position, 
	   Vector2D direction, 
	   double disiredSpeed, double  maxDisiredSpeed,
	   Vector2D currVelocity, 
	    std::list<Point2D> route,
	   double timeRelax, double sigma, double strengthSocialRepulsiveForceAgents,
	   double cosPhi){
	   	
	_id.currentRank(currentRank);
	_min_speed = min_speed;
	_max_speed = max_speed;
	//_targetPos = targetPos; 
	_position  = position;
	_direction = direction;
	_disiredSpeed    = disiredSpeed;
	_maxDisiredSpeed = maxDisiredSpeed;
	_currVelocity    = currVelocity;
	_route = route;
	_timeRelax = timeRelax;
	_sigma     = sigma;    
	_strengthSocialRepulsiveForceAgents = strengthSocialRepulsiveForceAgents;
	_cosPhi = cosPhi; 
}


Agent::Agent(repast::AgentId id, const Point2D &position, const double &min_speed, const double &max_speed, const json& SocialForceModel){ 

	_id        = id;
	_min_speed = min_speed;
	_max_speed = max_speed;
	_position  = position;
	_direction = Vector2D(0.0,0.0);
	
	
	//Establecer la velocidad del agente
	static thread_local std::random_device device;
	static thread_local std::mt19937 rng(device());

	std::uniform_real_distribution<double> speed(_min_speed, _max_speed);

	_disiredSpeed = speed(rng);
	_maxDisiredSpeed = 1.3 * this->_disiredSpeed;
	
	_currVelocity = Vector2D(0.0,0.0);
	
	_timeRelax =  SocialForceModel["timeRelax"].get<double>();//0.5; //[s]
	
	_sigma                              = SocialForceModel["sigma"].get<double>();//0.6;//[m]
	_strengthSocialRepulsiveForceAgents = SocialForceModel["repulsiveForceAgents"].get<double>();//2.1; //[m^2/s^-2]
	_cosPhi                             = SocialForceModel["cosphi"].get<double>();//-0.93969 ; //cos(200º)
	
}

Agent::~Agent(){ }

void Agent::setTargetPos(const Point2D& tposition){
	_targetPos = tposition;
}

const Point2D Agent::getTargetPos(void) const {
    return(_targetPos);
}

const Point2D Agent::position(void) const {
    return(_position);
}

void Agent::update(Environment* context,
					repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,
					repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace){
	
					
	//#pragma omp critical
	//std::cout << "Vecinos de "<< _id << " ==> " << 	closeNeighbors.size() << std::endl;		   			   
			
	// El agente debe avanzar según su
	// modelo de movilidad				   				  
	switch( this->getId().agentType() ) {
		case ShortestPath: {
		    this->shortestPath(context, discreteSpace, continuousSpace);
			break;
		}
		case RandomWalkway: {
			this->randomWalkway(context, discreteSpace, continuousSpace);
			break;
		}
		case FollowTheCrowd: {
		    //Agent::Neighbors neighbors = _env.neighbors_of(this->_agents[i],g_attractionRadius,SHORTESTPATH);
			//
		    //if(neighbors.empty()){
		    //  if(this->_routes[this->_agents[i].id()].empty()){    
		    //     auto response = _router.route(this->_agents[i].position(),g_randomWalkwayRadius);
		    //     this->_routes[this->_agents[i].id()] = response.path();
		    //   }
		    //  this->_agents[i].random_walkway(this->_routes[this->_agents[i].id()]);
		    //}
		    //else
		    //   this->_agents[i].follow_the_crowd(neighbors);  
			break;
		}
	}
	
	//El agenta salva su posición en el projector
	/*
	std::vector<double> agentLoc;
	continuousSpace->getLocation(_id, agentLoc);*/
	
	/*
	std::vector<double> agentNewLocContinuous;
	
	agentNewLocContinuous.push_back(this->_position[0]);
	agentNewLocContinuous.push_back(this->_position[1]);
	
	#pragma omp critical
	continuousSpace->moveTo(_id, agentNewLocContinuous);
	*/
	
	

}

void Agent::shortestPath(Environment* context,\
						repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,
						repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace){
	//El agente ya tiene la ruta más corta asignada desde el inicio
	//de la simulación. Solo debe seguir dicha ruta.
	this->followPath(context, discreteSpace, continuousSpace);
	//this->followPath(context, continuousSpace);
    
	/*
	if(_route.empty()) return;
   
    while(!_route.empty()) {
        Point2D dst = _route.front();
        double dist = sqrt(CGAL::squared_distance(this->_position, dst));
		
        Transformation scale(CGAL::SCALING, 1.0, dist);
        Vector2D direction(this->_position, dst);
		
		this->_direction = scale(direction);

        if(dist < g_closeEnough) {
            _route.pop_front();
            continue;
        }
		
		this->_currVelocity = _disiredSpeed * this->_direction;
			
		Transformation translate(CGAL::TRANSLATION, this->_currVelocity);
	    this->_position = translate(this->_position);
		
        break;
    }
	*/
	
}

void Agent::randomWalkway(Environment* context,\
						repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,
						repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace) {
	
	if(_route.empty()){  
		auto response = context->getRouter()->route(this->position(), Global::randomWalkwayRadius);
		_route = response.path();
	}
    this->followPath(context, discreteSpace, continuousSpace);
	//this->followPath(context, continuousSpace);
}

void Agent::followPath(Environment* context,\
					repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,
					repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace){
	
    
	/*std::vector<double> agentLocContinuous;
	continuousSpace->getLocation(_id, agentLocContinuous);	
	_position = Point2D(agentLocContinuous[0], agentLocContinuous[1]);*/
						
		
	if(_route.empty()){	 
		return;
	}
	
	double agentFactor = 1.0;
   
	
    while(!_route.empty()) {
        Point2D dst = _route.front();
		
		std::vector<double> agentLocContinuous;
		continuousSpace->getLocation(_id, agentLocContinuous);	
		_position = Point2D(agentLocContinuous[0], agentLocContinuous[1]);
		
		
        double dist = sqrt(CGAL::squared_distance(_position, dst));
		
        Transformation scale(CGAL::SCALING, 1.0, dist);
        Vector2D direction(_position, dst);
		
		_direction = scale(direction);
		
		Vector2D DrivingForce = Vector2D(0.0,0.0);
		double   deltaT       = 1.0;//[s]
		
		//Eq (2)
		//Helbing, D., & Molnar, P. (1998).
		//Social Force Model for Pedestrian Dynamics. Physical Review E, 51(5), 4282–4286.
		DrivingForce = (_disiredSpeed * _direction - _currVelocity) / _timeRelax;
		
        if(dist < Global::closeEnough) {
            _route.pop_front();
            continue;
        }
		
		// Determinar vecinos del agente
		std::vector<Agent*> closeNeighbors;
		uint32_t privateAreaRadius = Global::settings["privateAreaRadius"].get<uint32_t>(); //en [m]
		std::vector<int> agentLocDiscrete;
		discreteSpace->getLocation(_id, agentLocDiscrete);
	    repast::Point<int> centerAgent(agentLocDiscrete);
		repast::Moore2DGridQuery<Agent> moore2DQuery(discreteSpace);
	    moore2DQuery.query(centerAgent, privateAreaRadius, false, closeNeighbors);
		
		
		_currVelocity += agentFactor * DrivingForce * deltaT;	

		if( closeNeighbors.size() == 0 ){
			// No hay agentes cercanos 
			// Sólo actúa la fuerza DrivingForce    
	
			_currVelocity += agentFactor * DrivingForce * deltaT;		
		}
		else{
			
			Vector2D totalRepulsiveEfect = Vector2D(0.0,0.0);
	
			
			// Determinar el efecto repulsivo por cada
			// vecino cercano, y agregarlo al total
			for(Agent* fooAgent : closeNeighbors) {
				if(fooAgent == NULL || fooAgent == this){ 
					//¿Por qué se produce que uno de los vecinos sea NULL?
					//#pragma omp critical
					//std::cout << _id << " fooAgent NULL" << " => " << closeNeighbors.size() << std::endl;
					continue;
				}
				
				Vector2D repulsiveEfect = Vector2D(0.0,0.0);
				
				
				std::vector<double> fooAgentLocContinuous;
				continuousSpace->getLocation(fooAgent->getId(), fooAgentLocContinuous);	
				Point2D fooAgentPosition = Point2D(fooAgentLocContinuous[0], fooAgentLocContinuous[1]);
				
				Vector2D directionAgents(fooAgentPosition, this->_position);
				
				directionAgents /= sqrt(CGAL::scalar_product(directionAgents, directionAgents));
				
				//Calcular la distancia entre el agente "central" y el vecino fooAgent
				repast::Point<double> centerPoint(agentLocContinuous[0], agentLocContinuous[1]);
				repast::Point<double> fooPoint(fooAgentLocContinuous[0], fooAgentLocContinuous[1]);
				double distance = continuousSpace->getDistance(centerPoint, fooPoint);
				
		
				double strengthRepulsiveEfect = _strengthSocialRepulsiveForceAgents * exp(-distance/_sigma);
				repulsiveEfect = strengthRepulsiveEfect * directionAgents;
				
		
				// Determinar directionDependentWeight
				uint8_t directionDependentWeight = 1;
		
				if(CGAL::scalar_product(_direction, repulsiveEfect) >= strengthRepulsiveEfect*_cosPhi){
					directionDependentWeight = 1;
				}
				else{
					directionDependentWeight = 0.5;
				}
		
				//Eq (8)
				//Helbing, D., & Molnar, P. (1998). 
				//Social Force Model for Pedestrian Dynamics. Physical Review E, 51(5), 4282–4286. 
				
				
				totalRepulsiveEfect += repulsiveEfect * directionDependentWeight;
			}
			
	
			_currVelocity += agentFactor * (DrivingForce * deltaT + totalRepulsiveEfect * deltaT);
			
		}
		
		//Se limita la velocidad según Eq (11) y (12)
		//Helbing, D., & Molnar, P. (1998). 
		//Social Force Model for Pedestrian Dynamics. Physical Review E, 51(5), 4282–4286. 
		// REVISAR Y COMPARAR CON
		//Chen, X., Treiber, M., Kanagaraj, V., & Li, H. (2018). 
		//      Social force models for pedestrian traffic–state of the art. Transport Reviews.
		if( sqrt(CGAL::scalar_product(_currVelocity, _currVelocity)) >= _maxDisiredSpeed ){			
			_currVelocity = _maxDisiredSpeed * _currVelocity / sqrt(CGAL::scalar_product(_currVelocity, _currVelocity));		
		}

		//Finalmente, se actualiza la posición del agente
		_position += _currVelocity * deltaT;
		
		
		/*
		std::vector<double> agentNewLocContinuous;
		agentNewLocContinuous.push_back(_position[0]);
		agentNewLocContinuous.push_back(_position[1]);
	
		#pragma omp critical
		continuousSpace->moveTo(_id, agentNewLocContinuous);
	
		std::vector<int> agentNewLocDiscrete;
		agentNewLocDiscrete.push_back((int)(floor(agentNewLocContinuous[0])));
		agentNewLocDiscrete.push_back((int)(floor(agentNewLocContinuous[1])));
	
		#pragma omp critical
		discreteSpace->moveTo(_id, agentNewLocDiscrete);*/
	

        break;
    }
	


}

void Agent::followTheCrowd(std::vector<Agent*> _neighbors){
   /*
	static thread_local std::random_device device;
   static thread_local std::mt19937 rng(device());

   Vector2D direction(0.0,0.0);

   std::uniform_real_distribution<double> speed(this->_min_speed,this->_max_speed);

   for(auto& neighbor : _neighbors)
      direction+=neighbor->direction();

   Transformation scale(CGAL::SCALING,1.0,sqrt(direction.squared_length()));
   this->_direction=scale(direction);

   Transformation translate(CGAL::TRANSLATION,this->_direction*speed(rng));
   this->_position=translate(this->_position);
	*/
}

void Agent::move(repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,\
	repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace){
																							
	std::vector<double> agentNewLocContinuous;
	agentNewLocContinuous.push_back(_position[0]);
	agentNewLocContinuous.push_back(_position[1]);

	//#pragma omp critical
	continuousSpace->moveTo(_id, agentNewLocContinuous);

	std::vector<int> agentNewLocDiscrete;
	agentNewLocDiscrete.push_back((int)(floor(agentNewLocContinuous[0])));
	agentNewLocDiscrete.push_back((int)(floor(agentNewLocContinuous[1])));

	//#pragma omp critical
	discreteSpace->moveTo(_id, agentNewLocDiscrete);												
}


void Agent::randomWalkwayForAdjustInitialPosition(Environment* context,\
												repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,
												repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace){
	if(_route.empty()){  
		auto response = context->getRouter()->route(this->position(), Global::randomWalkwayRadius);
		_route = response.path();
	}
	
    if(_route.empty()) {
		/*
		std::vector<double> agentNewLocContinuous;
		agentNewLocContinuous.push_back(_position[0]);
		agentNewLocContinuous.push_back(_position[1]);
		
		//#pragma omp critical
		continuousSpace->moveTo(_id, agentNewLocContinuous);
		*/
		return;
	}
   
    while(!_route.empty()) {
        Point2D dst = _route.front();
        double dist = sqrt(CGAL::squared_distance(this->_position, dst));
		
		std::vector<double> agentLocContinuous;
		continuousSpace->getLocation(_id, agentLocContinuous);
		_position = Point2D(agentLocContinuous[0], agentLocContinuous[1]);
		
        Transformation scale(CGAL::SCALING, 1.0, dist);
        Vector2D direction(this->_position, dst);
		
		this->_direction = scale(direction);

        if(dist < Global::closeEnough) {
            _route.pop_front();
            continue;
        }
		
		this->_currVelocity = _disiredSpeed * this->_direction;
			
		//Transformation translate(CGAL::TRANSLATION, this->_currVelocity);
	    //this->_position = translate(this->_position);
		
		this->_position += _currVelocity;
		
        break;
    }
	
	/*std::vector<double> agentNewLocContinuous;
	agentNewLocContinuous.push_back(_position[0]);
	agentNewLocContinuous.push_back(_position[1]);
	continuousSpace->moveTo(_id, agentNewLocContinuous);
	
	std::vector<int> agentNewLocDiscrete;
	agentNewLocDiscrete.push_back((int)(floor(agentNewLocContinuous[0])));
	agentNewLocDiscrete.push_back((int)(floor(agentNewLocContinuous[1])));
	discreteSpace->moveTo(_id, agentNewLocDiscrete);*/
}





double Agent::distanceTo(Agent* agent,\
					repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace) const{

	std::vector<double> agentLocOrigin;
	continuousSpace->getLocation(_id, agentLocOrigin);
	
	std::vector<double> agentLocEnd;
	continuousSpace->getLocation(agent->getId(), agentLocEnd);
					
	return(sqrt(CGAL::squared_distance( Point2D(agentLocOrigin[0], agentLocOrigin[1]),  Point2D(agentLocEnd[0], agentLocEnd[1]) )));
}

// ###############################
//
// Serializable Agent Package Data 
//
// ###############################

AgentPackage::AgentPackage(){ }

AgentPackage::AgentPackage(uint32_t _id, uint32_t _rank, uint32_t _type, uint32_t _currentRank, 
			double   min_speed, double   max_speed, 
			//Point2D  targetPos,
			//Point2D  position , 
			double positionX, double  positionY,
			//Vector2D direction, 
			double directionX, double directionY,
			double   disiredSpeed,
			double   maxDisiredSpeed, 
			//Vector2D currVelocity,
			double currVelocityX, double currVelocityY,
			std::string route,
			double   timeRelax, double sigma,
			double   strengthSocialRepulsiveForceAgents,
			double   cosPhi){
				
	id   = _id;
	rank = _rank;
	type = _type;
	currentRank = _currentRank;
	
	_min_speed = min_speed;
	_max_speed = max_speed;
	//_targetPos = targetPos; 
	
	//_position  = position;
	_positionX = positionX;
	_positionY = positionY;
	
	//_direction = direction;
	_directionX = directionX;
	_directionY = directionY;
	
	_disiredSpeed    = disiredSpeed;
	_maxDisiredSpeed = maxDisiredSpeed;
	
	//_currVelocity = currVelocity;
	_currVelocityX = currVelocityX;
	_currVelocityY = currVelocityY;
	
	_route = route;
	
	_timeRelax    = timeRelax;
	_sigma        = sigma;    
	_strengthSocialRepulsiveForceAgents = strengthSocialRepulsiveForceAgents;
	_cosPhi = cosPhi; 			

}


