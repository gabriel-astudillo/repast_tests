#ifndef _MODEL_H_
#define _MODEL_H_

#include <global.hh>
#include <agent.hh>


/* Agent Package Provider */
class SIRAgentPackageProvider {

private:
	repast::SharedContext<SIRAgent>* agents;

public:

	SIRAgentPackageProvider(repast::SharedContext<SIRAgent>* agentPtr);

	void providePackage(SIRAgent* agent, std::vector<SIRAgentPackage>& out);

	void provideContent(repast::AgentRequest req, std::vector<SIRAgentPackage>& out);

};

/* Agent Package Receiver */
class SIRAgentPackageReceiver {

private:
	repast::SharedContext<SIRAgent>* agents;

public:

	SIRAgentPackageReceiver(repast::SharedContext<SIRAgent>* agentPtr);

	SIRAgent* createAgent(SIRAgentPackage package);

	void updateAgent(SIRAgentPackage package);

};




class SIRModel {
private:
	/*uint32_t tiempoSimulacion;
	uint32_t totalIndividuos;
	double   probInfeccion;
	uint32_t radioInfeccion;
	double   probInmunidad;
	uint32_t tiempoRecuperacion;*/

	boost::mpi::communicator _comm;

	repast::Properties* props;
	repast::SharedContext<SIRAgent> context;
	repast::SharedDiscreteSpace  <SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* discreteSpace;
	repast::SharedContinuousSpace<SIRAgent, repast::WrapAroundBorders, repast::SimpleAdder<SIRAgent> >* continuousSpace;

	SIRAgentPackageProvider* provider;
	SIRAgentPackageReceiver* receiver;

public:
	SIRModel(std::string& propsFile, int argc, char** argv, boost::mpi::communicator* comm);
	~SIRModel();

	void init();
	void synchronizeBorders();
	void initSchedule(repast::ScheduleRunner& runner);

	void checkStatus();

};


#endif
