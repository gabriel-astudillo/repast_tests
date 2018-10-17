#!/bin/bash
NUM_PROC=2

CONFIG_FILE="./props/config.props"
MODEL_FILE="./props/model.props"
EXEC_FILE="./example $CONFIG_FILE $MODEL_FILE $*"

export OMPI_MCA_btl_base_warn_component_unused=0
export OMPI_MCA_orte_base_help_aggregate=0

export LD_LIBRARY_PATH=/home/gabriel/sfw/repast_hpc-2.2.0/lib
MPICXX=/usr/bin/mpirun

$MPICXX -n $NUM_PROC $EXEC_FILE 
