#!/bin/bash
BASEDIR=$(readlink -f $0)
BASEDIR=$(dirname $BASEDIR)

#### CONFIGURACION EJECUTABLE####
PROJECT_BIN="./SIR"
NUM_PROC=4 #Deben ser coherentes con las dimensiones de la grilla: 'processDims' em model.cc

CONFIG_FILE="./props/config.props"
MODEL_FILE="./props/model.props"

#### CONFIGURACION REPAST####
export OMPI_MCA_btl_base_warn_component_unused=0
export OMPI_MCA_orte_base_help_aggregate=0
export LD_LIBRARY_PATH=/home/gabriel/sfw/repast_hpc/lib



#### CODIGO DE AYUDA PARA LA CARGA DEL SIMULADOR ####
MPICXX="$(which mpirun)"
if [[ ! -e $MPICXX ]]; then
	echo "mpirun no está instalado en $PATH."
	exit
fi


PROJECT_PATH=$BASEDIR/$PROJECT_BIN 

EXEC_FILE="$PROJECT_PATH -c $CONFIG_FILE -m $MODEL_FILE -s $CONFIG_FILE"
MPI_EXEC="$MPICXX -n $NUM_PROC $EXEC_FILE $*"

#### MAIN ####

#echo "Ejecutando con $NUM_PROC threads..."
export OMP_NUM_THREADS=$THREADS
$MPI_EXEC

