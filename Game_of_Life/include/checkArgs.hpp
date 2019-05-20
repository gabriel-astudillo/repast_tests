#ifndef _CHECKARGS_H_
#define _CHECKARGS_H_

#include <unistd.h>

extern char *optarg;
extern int optind, opterr, optopt;

class checkArgs {
private:
	// 1) Modificar esta sección
	const std::string optString = "c:m:s:h";
	
	const std::string opciones = "-c config_file_REPAST -m model_file_REPAST -s config_file_DEMPS [-h]";

	const std::string descripcion  = "Descripción:\n"
		                             "\t-c   Archivo de configuración REPAST\n"
									 "\t-m   Archivo de propiedades del modelo REPAST\n"
									 "\t-s   Archivo de propiedades de DEMPS\n"
									 "\t-h   Muestra esta ayuda y termina\n";
	
	typedef struct args_t{
		std::string configFile;
		std::string modelFile;
		std::string configSpecificFile;
	} args_t;
	
	// 2) Modificar constructor
	// 3) Modificar ciclo "getopt" en método checkArgs::getArgs()
	// 4) Recuerde que para compilar nuevamente, y por tratarse
	//    de un archivo header, debe hacer primero un make clean
	
	args_t  parametros;
	
	int argc;
	char **argv;

	
public:
	checkArgs(int _argc , char **_argv);
	~checkArgs();
	args_t getArgs();;
	
private:
	void printUsage();
	
	
};

checkArgs::checkArgs(int _argc , char **_argv){
	parametros.configFile = "";
	parametros.modelFile  = "";
	parametros.configSpecificFile = "";
	
	argc = _argc;
	argv = _argv;
	
}

checkArgs::~checkArgs(){
	
}

checkArgs::args_t checkArgs::getArgs(){
	int opcion;
	
	while ((opcion = getopt (argc, argv, optString.c_str())) != -1){
		switch (opcion) {
			case 'c':
					parametros.configFile = optarg;
					break;
			case 'm':
					parametros.modelFile = optarg;
					break;
			case 's':
					parametros.configSpecificFile = optarg;
					break;
			case 'h':
			default:
					printUsage();
					exit(EXIT_FAILURE);
		}
	}

	if ( parametros.configFile.empty() ||
	     parametros.modelFile.empty()  ||
	     parametros.configSpecificFile.empty()){
		printUsage();
		exit(EXIT_FAILURE);
	}
	
	return(parametros);
}

void checkArgs::printUsage(){
	printf("Uso: %s %s\n%s\n", argv[0], opciones.c_str(), descripcion.c_str());
}



#endif
