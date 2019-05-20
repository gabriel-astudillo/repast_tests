#ifndef _MODEL_H_
#define _MODEL_H_

#include <global.hh>
#include <agent.hh>


/* Agent Package Provider */
class GolAgentPackageProvider {
	
private:
    repast::SharedContext<GolAgent>* agents;
	
public:
	
    GolAgentPackageProvider(repast::SharedContext<GolAgent>* agentPtr);
	
    void providePackage(GolAgent* agent, std::vector<GolAgentPackage>& out);
	
    void provideContent(repast::AgentRequest req, std::vector<GolAgentPackage>& out);
	
};

/* Agent Package Receiver */
class GolAgentPackageReceiver {
	
private:
    repast::SharedContext<GolAgent>* agents;
	
public:
	
    GolAgentPackageReceiver(repast::SharedContext<GolAgent>* agentPtr);
	
    GolAgent* createAgent(GolAgentPackage package);
	
    void updateAgent(GolAgentPackage package);
	
};




class GolModel{
private:
	uint32_t duration;
	uint32_t totalGolAgents;
	double   probLiveAgent;
	
	repast::Properties* props;
	repast::SharedContext<GolAgent> context;
	repast::SharedDiscreteSpace<GolAgent, repast::WrapAroundBorders, repast::SimpleAdder<GolAgent> >* discreteSpace;
	
	GolAgentPackageProvider* provider;
	GolAgentPackageReceiver* receiver;
	
public:
	GolModel(std::string& propsFile, int argc, char** argv, boost::mpi::communicator* comm);
	~GolModel();
	
	void init();
	void synchronizeBorders();
	void initSchedule(repast::ScheduleRunner& runner);
	
	void checkStatus();

};


#endif
