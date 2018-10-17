CC=gcc
CXX=g++
RM=rm -f

TARGET=../example

CXXFLAGS=-std=c++14 -O3
# -fsanitize=address -g

#######DEFINICIONES PARA REPAST DEBIAN 9#########
MPICXX=/usr/bin/mpicxx

BOOST_INCLUDE=-I/usr/include/
BOOST_LIB_DIR=-L/usr/lib/x86_64-linux-gnu
BOOST_LIBS=-lboost_mpi -lboost_serialization -lboost_system -lboost_filesystem

REPAST_HPC_INCLUDE=-I/home/gabriel/sfw/repast_hpc-2.2.0/include
REPAST_HPC_LIB_DIR=-L/home/gabriel/sfw/repast_hpc-2.2.0/lib
REPAST_HPC_LIB=-lrepast_hpc
#######
SRC_INCLUDES=-I../include
SRC_LIB_DIR=
SRC_LIBS=
#INCLUDES=-I/usr/local/include -I/usr/local/include/osrm -I../include
#LDFLAGS=-L/usr/local/lib
#LDLIBS=-lGeographic -lCGAL -losrm -lboost_system -lboost_filesystem -lboost_iostreams -lboost_thread -lrt -ltbb -lpthread -fopenmp -lgmp



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
	