#include <global.hh>
#include <checkArgs.hpp>
#include <model.hh>

// Ejemplo de agentes que se mueven en un espacio continuo.
// Este programa implementa el modelo de contagio SIR.

namespace Global
{
uint32_t tiempoSimulacion;
uint32_t totalIndividuos;
double   probInfeccion;
uint32_t radioInfeccion;
double   probInmunidad;
uint32_t tiempoRecuperacion;
}

int main(int argc, char** argv)
{

	// Adquisición de parámetro de entrada BEGIN
	checkArgs* argumentos = new checkArgs(argc, argv);

	std::string configFile = argumentos->getArgs().configFile;  // The name of the configuration file
	std::string modelFile  = argumentos->getArgs().modelFile;   // The name of the model properties file
	std::string configSpecificFile  = argumentos->getArgs().configSpecificFile;

	delete argumentos;
	// Adquisición de parámetro de entrada END


	boost::mpi::environment env(argc, argv);

	//std::cout << env.processor_name() << std::endl;

	boost::mpi::communicator world;

	//std::cout << "Hello, world! I'm rank " << world.rank()<< " of " << world.size() << std::endl;

	repast::RepastProcess::init(configFile);

	SIRModel* model = new SIRModel(modelFile, argc, argv, &world);

	auto& runner = repast::RepastProcess::instance()->getScheduleRunner();

	model->init();
	model->initSchedule(runner);

	runner.run();

	delete model;

	repast::RepastProcess::instance()->done();

	return(EXIT_SUCCESS);
}
