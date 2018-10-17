#ifndef _MODEL_H_
#define _MODEL_H_

#include <global.h>
#include <agent.h>

/* Agent Package Provider */
class AgentPackageProvider {
private:
    repast::SharedContext<Agent>* agents;
	
public:	
    AgentPackageProvider(repast::SharedContext<Agent>* agentPtr);
    void providePackage(Agent * agent, std::vector<AgentPackage>& out);
    void provideContent(repast::AgentRequest req, std::vector<AgentPackage>& out);
	
};

/* Agent Package Receiver */
class AgentPackageReceiver {
private:
    repast::SharedContext<Agent>* agents;
	
public:
    AgentPackageReceiver(repast::SharedContext<Agent>* agentPtr);
    Agent * createAgent(AgentPackage package);
    void updateAgent(AgentPackage package);
	
};


class demoModel{
	uint32_t stopAt;
	uint32_t totalAgents;
	
	repast::Properties* props;
	repast::SharedContext<Agent> context;
	
	AgentPackageProvider* provider;
	AgentPackageReceiver* receiver;
	
public:
	demoModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm);
	~demoModel();
	void init();
	void requestAgents();
	void cancelRequestAgents();
	void removeLocalAgents();
	void doSomething();
	void initSchedule(repast::ScheduleRunner& runner);
	void recordResults();
	
	void viewState(const std::string& label, const uint32_t& rankNumber);
};

#endif
