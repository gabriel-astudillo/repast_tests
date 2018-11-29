#include <agent.hh>
#include <model.hh>
#include <global.hh>
#include <checkArgs.hh>
#include <json.hpp>

void loadDataFrom(std::string fileIn, json& dataOut){
	
	std::ifstream ifs;
	
	ifs.open(fileIn,std::ifstream::in);
	if(ifs.fail()) {
	    std::cerr << "Open error in file:"<<  fileIn << std::endl;
	    exit(EXIT_FAILURE);
	}

	ifs >> dataOut;
	ifs.close();
	
	if(dataOut.empty()){
	    std::cerr << "Can't load data from:"<<  fileIn << std::endl;
	    exit(EXIT_FAILURE);
	}
	
}


int main(int argc, char** argv){
	
	checkArgs* argumentos = new checkArgs(argc, argv);
	
	std::string configFile = argumentos->getArgs().configFile;  // The name of the configuration file 
	std::string modelFile  = argumentos->getArgs().modelFile;   // The name of the model properties file 
	std::string configSpecificFile  = argumentos->getArgs().configSpecificFile; 

    std::ifstream ifs;
    ifs.open(configSpecificFile,std::ifstream::in);
    ifs >> Global::settings;
    ifs.close();
	
	std::string area_zone_file;
	std::string initial_zones_file;
	std::string reference_zones_file;
	//std::string reference_point_file;

	try {
		Global::map_osrm     = Global::settings["input"]["map"].get<std::string>();
		area_zone_file       = Global::settings["input"]["area"].get<std::string>();
		initial_zones_file   = Global::settings["input"]["initial_zones"].get<std::string>();
		reference_zones_file = Global::settings["input"]["reference_zones"].get<std::string>();
		//reference_point_file = Global::settings["input"]["reference_point"].get<std::string>();
	}catch (json::exception &e){
		std::cerr << "Error in get action from 'input' section in <config.json>:" << std::endl;
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	
	ifs.open(Global::map_osrm,std::ifstream::in);
	if(ifs.fail()) {
	    std::cerr << "Error in file:"<<  Global::map_osrm << std::endl;
	    exit(EXIT_FAILURE);
	}
	ifs.close();
	
	loadDataFrom(area_zone_file      , Global::area_zone);
	loadDataFrom(initial_zones_file  , Global::initial_zones);
	loadDataFrom(reference_zones_file, Global::reference_zones);	
	//loadDataFrom(reference_point_file, Global::reference_point);
	
	/*
		Los datos para la simulación están disponible para todo el proyecto:
		Global::settings
		Global::map_osrm
		Global::area_zone
		Global::reference_zones
		Global::initial_zones
	*/
	
	Global::showProgressBar     = Global::settings["output"]["progressBar"].get<bool>();
	Global::closeEnough         = Global::settings["closeEnough"].get<float>();
	Global::randomWalkwayRadius = Global::settings["randomWalkwayRadius"].get<float>();
	Global::attractionRadius    = Global::settings["attractionRadius"].get<float>();
	
	//Reset counters
	Counters::timeExecMakeAgents = 0;
	Counters::timeExecCalibrate  = 0;
	Counters::timeExecSimulate   = 0;

	
	boost::mpi::environment env(argc, argv);
	boost::mpi::communicator world;

	repast::RepastProcess::init(configFile);
	
	
	Model* model = new Model(modelFile, argc, argv, &world);
	repast::ScheduleRunner& runner = repast::RepastProcess::instance()->getScheduleRunner();
	uint32_t myRank   = repast::RepastProcess::instance()->rank();
	
	model->init();
	model->calibrate();
	
	model->initSchedule(runner);
	
	std::cout << myRank << ": Simulando..." << std::endl;
	runner.run();
	
	if(Global::showProgressBar){std::cout << std::endl;}
	model->showTimeExec();
	
	delete model;
	
	repast::RepastProcess::instance()->done();
	  
	
	
		
}