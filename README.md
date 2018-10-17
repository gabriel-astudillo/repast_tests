# Pruebas de código en Repast HPC

Para ejecutar estos códigos es necesario instalar RepastHPC [repast_hpc-2.2.0.tgz](https://github.com/Repast/repast.hpc/releases/download/v2.2.0/repast_hpc-2.2.0.tgz).

# Instalación de RepastHPC

1. Antes de instalar RepastHPC, es necesario verificar que las librerías Boost de C++ estén instalada y que exista alguna implementación de MPI. Los códigos utilizan las librerías ```libboost-all-dev``` y ```libblacs-mpi-dev```.

2. Una vez descomprimido, hay que modificar la línea 4088 que archivo ```configure```: agregar "a mano" la versión de boost instalada en el sistema. Para esto, revisar el archivo ```/usr/include/boost/version.hpp```. Por ejemplo, agregar ```boost_major_version=165``` si la versión es 1_65. **IMPORTANTE**: No instalar la versión que viene con Repast.

3. No se logró que compilara con la version de netcdf de los paquetes de Debian. Se instaló las librerías que vienen con repast (dentro del directorio ```MANUAL_INSTALL```), cuya instalacion se detalla en el apartado "Requirement #5" del  archivo INSTALL.txt

```
cd MANUAL_INSTALL
./install.sh netcdf
```

4. Luego se instala REPAST con ```./install.sh rhpc```, dentro del directorio ```MANUAL_INSTALL```.

5. Repast queda instalado en ~/sfw/repast_hpc-2.2.0. En el sub directorio lib/, hay que crear los links simbólicos para las opciones LIBS del makefile respectivo. Antes de ejecutar, hay que configurar la variable de entorno LD_LIBRARY_PATH adecuadamente.
  
```
librelogo-2.2.0.a
librelogo-2.2.0.so
librelogo.a -> librelogo-2.2.0.a           #agregar
librelogo.so -> librelogo-2.2.0.so         #agregar
librepast_hpc-2.2.0.a
librepast_hpc-2.2.0.so
librepast_hpc.a -> librepast_hpc-2.2.0.a   #agregar
librepast_hpc.so -> librepast_hpc-2.2.0.so #agregar
```
# Ejemplo de Makefile

```
CXX=g++
RM=rm -f

TARGET=./example

CXXFLAGS=-std=c++14 -O3

####### DEFINICIONES PARA MPI #########
MPICXX=/usr/bin/mpicxx

####### DEFINICIONES PARA BOOST #########
BOOST_INCLUDE=-I/usr/include/
BOOST_LIB_DIR=-L/usr/lib/x86_64-linux-gnu
BOOST_LIBS=-lboost_mpi -lboost_serialization -lboost_system -lboost_filesystem

####### DEFINICIONES PARA REPAST #########
REPAST_HPC_INCLUDE=-I/home/gabriel/sfw/repast_hpc-2.2.0/include
REPAST_HPC_LIB_DIR=-L/home/gabriel/sfw/repast_hpc-2.2.0/lib
REPAST_HPC_LIB=-lrepast_hpc

#######DEFINICIONES PARA EL PROYECTO #########
SRC_INCLUDES=-I../include
SRC_LIB_DIR=
SRC_LIBS=


INCLUDES=$(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) $(SRC_INCLUDES)
LDFLAGS=$(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) $(SRC_LIB_DIR)
LDLIBS=$(BOOST_LIBS) $(REPAST_HPC_LIB) $(SRC_LIBS)

CXX=$(MPICXX)

DIR_OBJ=./obj

SRCS=$(wildcard *.cc)

OBJS=$(patsubst %.cc,$(DIR_OBJ)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS) $(LDFLAGS) 

$(DIR_OBJ)/%.o: %.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(INCLUDES)

clean:
	$(RM) $(OBJS)
	$(RM) *~

distclean: clean
	$(RM) $(TARGET)

.PHONY: all clean distclean
```

# Ejemplo de script para ejecutar 

```

#!/bin/bash
NUM_PROC=4

CONFIG_FILE="./props/config.props"
MODEL_FILE="./props/model.props"
EXEC_FILE="./example $CONFIG_FILE $MODEL_FILE $*"

export OMPI_MCA_btl_base_warn_component_unused=0
export OMPI_MCA_orte_base_help_aggregate=0

export LD_LIBRARY_PATH=/home/gabriel/sfw/repast_hpc-2.2.0/lib
MPICXX=/usr/bin/mpirun

$MPICXX -n $NUM_PROC $EXEC_FILE 


```


