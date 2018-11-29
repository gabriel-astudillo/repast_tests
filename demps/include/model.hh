#ifndef _MODEL_H_
#define _MODEL_H_

#include <global.hh>
#include <environment.hh>
#include <agent.hh>


/* Agent Package Provider */
class AgentPackageProvider {
private:
    Environment* agents;
	
public:	
    //AgentPackageProvider(repast::SharedContext<Agent>* agentPtr);
	AgentPackageProvider(Environment* agentPtr);
    void providePackage(Agent * agent, std::vector<AgentPackage>& out);
    void provideContent(repast::AgentRequest req, std::vector<AgentPackage>& out);
	
};

/* Agent Package Receiver */
class AgentPackageReceiver {
private:
    //repast::SharedContext<Agent>* agents;
	Environment* agents;
	
public:
    //AgentPackageReceiver(repast::SharedContext<Agent>* agentPtr);
	AgentPackageReceiver(Environment* agentPtr);
    Agent * createAgent(AgentPackage package);
    void updateAgent(AgentPackage package);
	
};



class Model{
	float stopAt;
	//uint32_t totalAgents;
	
	repast::Properties* props;
	//repast::SharedContext<Agent> context;
	Environment context;
	
	
	boost::mpi::communicator* _comm;
	
	AgentPackageProvider* provider;
	AgentPackageReceiver* receiver;
	
	repast::SharedDiscreteSpace<  Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* discreteSpace;
	repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >* continuousSpace;
	
	
public:
	Model(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm);
	~Model();
	void init();
	
	void calibrate();
	void simulate();
	void stats();
	
	void initSchedule(repast::ScheduleRunner& runner);
	void adjustAgentsInitialPosition(const uint32_t& calibrationTime);
	void adjustAgentsRules();
	
	void viewState(const std::string& label, const uint32_t& rankNumber);
	
	void saveStateToFile();
	void saveStatsToFile();
	void showTimeExec();
	
private:
	void progress();
};

#endif
