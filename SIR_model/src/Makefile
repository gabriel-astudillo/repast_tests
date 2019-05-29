CC=gcc
CXX=g++
RM=rm -f

TARGET=../SIR

#HOME_BASE=/home/gvcosta/gastudil
HOME_BASE=/home/gabriel

CXXFLAGS=-std=c++14 -O2
# -fsanitize=address -g

#######DEFINICIONES PARA REPAST#########
MPICXX=/usr/bin/mpicxx

#######
BOOST_INCLUDE=-I/usr/include/
BOOST_LIB_DIR=-L/usr/lib/x86_64-linux-gnu
BOOST_LIBS=-lboost_mpi -lboost_serialization -lboost_system -lboost_filesystem
#######
REPAST_HPC_INCLUDE=-I$(HOME_BASE)/sfw/repast_hpc/include
REPAST_HPC_LIB_DIR=-L$(HOME_BASE)/sfw/repast_hpc/lib
REPAST_HPC_LIB=-lrepast_hpc

#######DEFINICIONES PARA EL RESTO#########
SRC_INCLUDE=-I../include 
SRC_LIB_DIR=
SRC_LIBS=-fopenmp 

#######CONSOLIDACIÓN
INCLUDES=$(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) $(SRC_INCLUDE) $(INCLUDES_CLUST)
LDFLAGS=$(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) $(SRC_LIB_DIR) $(LDFLAGS_CLUST)
LDLIBS=$(BOOST_LIBS) $(REPAST_HPC_LIB) $(SRC_LIBS)

CXX=$(MPICXX)

DIR_OBJ=./obj

SRCS=$(wildcard *.cc)

OBJS=$(patsubst %.cc,$(DIR_OBJ)/%.o,$(SRCS))


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS) $(LDFLAGS) 

$(DIR_OBJ)/%.o: %.cc
	@mkdir -p $(DIR_OBJ)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(INCLUDES)

clean:
	$(RM) $(OBJS)
	$(RM) *~

distclean: clean
	$(RM) $(TARGET)

.PHONY: all clean distclean
	