#ifndef _AGENT_H_
#define _AGENT_H_

#include <global.hh>

class Environment;

/* Agents */
class Agent{
public: 
	std::list<Point2D> _route; 
	
private:
    repast::AgentId   _id; //Correlativo, proceso origen, tipo, proceso actual
	
	double   _min_speed;
	double   _max_speed;
	
	// Para los agentes que conocer donde su meta
	Point2D  _targetPos; 
	
	//std::vector<Agent*> _closeNeighbors;
	//uint32_t _quad;
	
	//Variables para el modelo de Fuerza social
	//Helbing, D., & Molnar, P. (1998). 
	//Social Force Model for Pedestrian Dynamics. Physical Review E, 51(5), 4282–4286. 
	
	Point2D  _position;
	Vector2D _direction;
	
	double   _disiredSpeed;
	double   _maxDisiredSpeed;
	Vector2D _currVelocity;
	
	//Driving force of the taget Point
	double   _timeRelax;
	
	//Interaction force between agents
	double   _sigma;    //[m]
	double   _strengthSocialRepulsiveForceAgents;
	double   _cosPhi; //cos(200º)
	
	
public:
	//Constructor para el envio de agentes en los bordes
	Agent(repast::AgentId id, double min_speed, double max_speed, 
	       //Point2D  targetPos,
			Point2D  position, 
			Vector2D direction, 
		   double disiredSpeed, double  maxDisiredSpeed,
		   Vector2D currVelocity, 
		   std::list<Point2D> route,
		   double timeRelax, double sigma, double strengthSocialRepulsiveForceAgents,
		   double cosPhi);
	
	//setter para el envio de agentes en los bordes
	void set(uint32_t currentRank, double min_speed, double max_speed, 
	       //Point2D  targetPos,
			Point2D  position, 
			Vector2D direction, 
		   double disiredSpeed, double  maxDisiredSpeed,
		   Vector2D currVelocity, 
		   std::list<Point2D> route,
		   double timeRelax, double sigma, double strengthSocialRepulsiveForceAgents,
		   double cosPhi);
	
    Agent(repast::AgentId id, const Point2D& position, const double &min_speed, const double &max_speed, const json& SocialForceModel);
	Agent(){}
    //Agent(repast::AgentId id, double newC, double newTotal);
	
    ~Agent();
	
    /* Required Getters */
    virtual       repast::AgentId& getId(){             return _id;    }
    virtual const repast::AgentId& getId() const {      return _id;    }
			
    //void play(repast::SharedContext<Agent>* context); // Choose three other agents from the given context and see if they cooperate or not

	
	void          setTargetPos(const Point2D& tposition);
	const Point2D getTargetPos(void) const;
	const Point2D position(void) const;
	
	double   getMinSpeed(){return  _min_speed;}
	double   getMaxSpeed(){return  _max_speed;}
	Point2D  getTargetPos(){return _targetPos;}
	Point2D  getPosition(){return _position;}
	Vector2D getDirection(){return _direction;}
	double   getDisiredSpeed(){return _disiredSpeed;}
	double   getMaxDisiredSpeed(){return _maxDisiredSpeed;}
	Vector2D getCurrVelocity(){return _currVelocity;}
	
	std::string getRoute(){
		std::string routeStr = "";
		for(auto& point : _route){
			routeStr += std::to_string( point.x() ) + "," + std::to_string( point.y() ) + ";" ;
		}
		
		return(routeStr);
	}
	
	double   getTimeRelax(){return _timeRelax;}
	double   getSigma(){return _sigma;}
	double   getStrengthSocialRepulsiveForceAgents(){return _strengthSocialRepulsiveForceAgents;}
	double   getCosPhi(){return _cosPhi;}
	
	/*
	uint32_t determineQuad(Environment* contex);
	void     setQuad(Environment* contex);
	void     setQuad(uint32_t idQuad);
	uint32_t getQuad() const;
	void     updateQuad(Environment* contex);
	*/
	
	/*
	void     clearCloseNeighbors();
	void     addCloseNeighbors(Agent* neighbor);
	*/
	
	void     update(Environment* context,\
		repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,
		repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace);
	
	void shortestPath(Environment* context,\
							repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,\
							repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace);
	
	void randomWalkway(Environment* context,\
							repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,\
							repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace);
	
	void followPath(Environment* context,\
						repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,\
						repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace);
	
	void followTheCrowd(std::vector<Agent*> _neighbors);
	
	void move(repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,\
		repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace);
	
	
	void randomWalkwayForAdjustInitialPosition(Environment* context,\
						repast::SharedDiscreteSpace<Agent  , repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace,\
						repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace);
	
	
	double distanceTo(Agent* agent, repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace) const;
	
};

/* Serializable Agent Package */
struct AgentPackage {
	
public:
    uint32_t    id;
    uint32_t    rank;
    uint32_t    type;
    uint32_t    currentRank;
	
	double   _min_speed;
	double   _max_speed;
	//Point2D  _targetPos; 
	double _positionX, _positionY; //Point2D  _position; 
	double _directionX, _directionY; //Vector2D _direction;
	double _disiredSpeed;
	double _maxDisiredSpeed;
	double _currVelocityX, _currVelocityY; //Vector2D _currVelocity;
	std::string _route;
	double _timeRelax;
	double _sigma;    
	double _strengthSocialRepulsiveForceAgents;
	double _cosPhi; 
	
	
    double c;
    double total;
	
    /* Constructors */
    AgentPackage(); // For serialization
    AgentPackage(uint32_t _id, uint32_t _rank, uint32_t _type, uint32_t _currentRank, 
				double   _min_speed,
				double   _max_speed,
				//Point2D  _targetPos,
				double _positionX , double _positionY, //Para Point2D  _position,
				double _directionX, double _directionY, //Para Vector2D _direction,
				double _disiredSpeed,
				double _maxDisiredSpeed,				
				double _currVelocityX, double _currVelocityY, //Para Vector2D _currVelocity,
				std::string _route,
				double _timeRelax,
				double _sigma,
				double _strengthSocialRepulsiveForceAgents,
				double _cosPhi
	);
	
    /* For archive packaging */
    template<class Archive>
    void serialize(Archive &ar, const uint32_t version){
        ar & id;
        ar & rank;
        ar & type;
        ar & currentRank;
		
		
		ar & _min_speed;
		ar & _max_speed;
		//ar & _targetPos; 
		//ar & _position;
		ar &  _positionX;
		ar &  _positionY;
		//ar & _direction;
		ar &  _directionX;
		ar &  _directionY;
		ar & _disiredSpeed;
		ar & _maxDisiredSpeed;
		//ar & _currVelocity;
		ar & _currVelocityX;
		ar & _currVelocityY;
		
		ar & _route;
		
		ar & _timeRelax;
		ar & _sigma;    
		ar & _strengthSocialRepulsiveForceAgents;
		ar & _cosPhi; 
        
    }
	
};


#endif