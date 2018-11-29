#include <environment.hh>

/**
* @brief Crea el Router para los agentes en el mapa
*
*El Router se encarga de determinar las rutas que deben
*seguir los agentes en el mapa asignado.
*Necesita que el atributo _reference_point este inicializado.
*
* @param _map_osrm        : string que representa el archivo con la base de datos OSRM del mapa.
* @return void
*/
void Environment::setRouter(const std::string &_map_osrm){
	_router =  Router(_reference_point,_map_osrm);
}

Router* Environment::getRouter(){
	return(&_router);
}

/**
* @brief Inicializa el punto de referencia del mapa
*
*El Punto de Referencia sirva para realizar la conversión
*entre coordenadas WGS84 y ENU. Marca el origen en este 
*último sistema. Se determina cómo el vértice superior izquierdo
*del rectangulo que determina el mapa de la simulación
*
* @param _freference_point: GeoJson con los datos latitud y longitud del punto de referencia
* @return void
*/

//void Environment::setReferencePoint(const json &_freference_point){
//	this->_reference_point = _freference_point;
//}

void Environment::setReferencePoint(const json &_fmap_zone ) {
	std::list<double> map_x;
	std::list<double> map_y;
	
	double xMin, xMax, yMin, yMax;
	double mapWidth, mapHeight;
	
	for(auto& point : _fmap_zone["features"][0]["geometry"]["coordinates"][0]){
		double x,y,z;
		map_x.push_back( point[1] );
		map_y.push_back( point[0] );
	}
	map_x.sort(); map_y.sort();
	
	//Coordenadas min y max para x e y.
	xMin = map_x.front(); xMax = map_x.back();	
	yMin = map_y.front(); yMax = map_y.back();

	//Ancho y alto del mapa
	mapWidth  = std::abs(xMax - xMin);
	mapHeight = std::abs(yMax - yMin);
	
	// El punto (xMin, yMin) debe ser el punto de referencia
	// ==> hay que convertirlo a JSON
	/*
	{
	  "type": "FeatureCollection",
	  "features": [
	    {
	      "type": "Feature",
	      "properties": {},
	      "geometry": {
	        "type": "Point",
	        "coordinates": [
				 -70.166667,  <== yMin
				 -20.216667   <== xMin
	        ]
	      }
	    }
	  ]
	}
	*/
	
	json geomData = {
		{"type", "Point"},
		{"coordinates", json::array({ yMin, xMin } ) }
	};
	
	this->_reference_point = {
		{"type", "FeatureCollection"},
		{"features", json::array({ json({{"type", "Feature"}, {"properties", {} }, {"geometry", geomData } }) }) }
	};
	
	//std::cout << "xMin:" << xMin << ", yMin:" << yMin << std::endl;
	//std::cout << "xMax:" << xMax << ", yMax:" << yMax << std::endl;
	
}

/**
* @brief Crea el proyector para las coordenadas
*
*El proyector es una estructura que sirve para determinar
*donde se ubica el origen del sistema de coordenadas ENU.
*Necesita que el atributo _reference_point este inicializado.
*
* @param void
* @return void
*/
void Environment::setProjector(){
	this->_projector = LocalCartesian(_reference_point["features"][0]["geometry"]["coordinates"][1],_reference_point["features"][0]["geometry"]["coordinates"][0],0,Geocentric::WGS84());
}

/**
* @brief Crea las zonas de referencia
*
*Las zonas de referencias son las zones seguras donde deben ir
*los agentes durante la simulación.
*
* @param _freference_zones: GeoJson que representa las zonas de referencia.
* @return void
*/
void Environment::setReferenceZones(const json &_freference_zones){
	for(auto& feature : _freference_zones["features"]){
		this->_reference_zones.push_back(Zone(_reference_point, feature));
	}
}

/**
* @brief Crea las zonas iniciales
*
*Las zonas iniciales son las zones donde se van a
*ubicar los agentes al inicio de la simulación
*
* @param _finitial_zones: GeoJson que representa las zonas iniciales.
* @return void
*/
void Environment::setInitialZones(const json &_finitial_zones){
	for(auto& feature : _finitial_zones["features"]){
		this->_initial_zones.push_back(Zone(_reference_point, feature));
	}
}

/**
* @brief Crea los cuadrantes del mapa de la simulación
*
* @param _fmap_zone: GeoJson que representa el mapa.
* @return void
*/
void Environment::setGrid(const json &_fmap_zone){
	std::list<double> map_x;
	std::list<double> map_y;
	
	for(auto& point : _fmap_zone["features"][0]["geometry"]["coordinates"][0]){
		double x,y,z;
		this->getProjector().Forward(point[1],point[0],0,x,y,z);
	
		map_x.push_back( x );
		map_y.push_back( y );
	}
	map_x.sort(); map_y.sort();
	
	//Coordenadas min y max para x e y.
	_grid._xMin = map_x.front(); _grid._xMax = map_x.back();	
	_grid._yMin = map_y.front(); _grid._yMax = map_y.back();

	//Ancho y alto del mapa
	_grid._mapWidth  = std::abs(_grid._xMax - _grid._xMin);
	_grid._mapHeight = std::abs(_grid._yMax - _grid._yMin);
	

}

Environment::grid_t Environment::getGrid(){
	return(_grid);
}

void Environment::showGrid(){
	std::cout <<
		"xMin:" << _grid._xMin  << ", xMax:" << _grid._xMax << "\n" <<
		"yMin:" << _grid._yMin  << ", yMax:" << _grid._yMax << "\n" <<
		"mapWidth: " << _grid._mapWidth << ", mapHeight:" << _grid._mapHeight << "\n" <<
		std::endl;
}

Zone Environment::getInitialZone(uint32_t id){
	return(this->_initial_zones[id]);
}

std::vector<Zone>  Environment::getInitialZones(){
	return(this->_initial_zones);
}

std::vector<Zone>& Environment::getReferenceZones(){
	return(this->_reference_zones);
}

LocalCartesian Environment::getProjector(){
	return(this->_projector);
}

void Environment::setNeighborsOf(Agent* agent,const double& distanceMax){
	/*
	std::vector<repast::AgentId> idsAgents;
	
	//Agent* agent = this->getAgent(idAgent);
	
	omp_set_lock(&lock_agentsInQuad);
	idsAgents = _agentsInQuad[agent->getQuad()];
	omp_unset_lock(&lock_agentsInQuad);
	
	agent->clearCloseNeighbors(); 
	
	auto idAgent = agent->getId();
	
	for(auto& id : idsAgents) {
		if(id != idAgent){
			Agent* neighbor;
			neighbor = this->getAgent(id);
		
			if( this->distance(agent, neighbor) < distanceMax ) {
				agent->addCloseNeighbors(neighbor); //test
			}
		}	
	}
	*/
}




double Environment::distance(Agent* a, Agent* b){
	return(sqrt(CGAL::squared_distance(a->position(), b->position())));
}











