#include <global.hh>
#include <checkArgs.hpp>
#include <model.hh>

// Intento de implementar el Juego de la Vida en RepastHPC
// Cada celda del universo es un Agente.

int main(int argc, char** argv){
	
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
	
	std::cout << "Hello, world! I'm rank " << world.rank()<< " of " << world.size() << std::endl;
	

	repast::RepastProcess::init(configFile);
	
	GolModel* golModel = new GolModel(modelFile, argc, argv, &world);
	
	auto& runner = repast::RepastProcess::instance()->getScheduleRunner();
	
	golModel->init();
	golModel->initSchedule(runner);
	
	runner.run();
	
	delete golModel;

	
	
	repast::RepastProcess::instance()->done();
		
	return(EXIT_SUCCESS);
}