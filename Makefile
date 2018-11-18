CXX          	= g++
COMPILE_FLAGS	= -std=c++11 -Wall -Wextra -g
CXXFLAGS     	= $(COMPILE_FLAGS)
LIBS         	= 
DEL_FILE     	= rm -f
MKDIR_P			= mkdir -p

.PHONY: directories

OBJECTS_1_SAVINGS				= 	build/o/1-savings.o \
									build/o/instance.o \
									build/o/aux.o \
									build/o/savings.o

OBJECTS_2_GOLOSA				= 	build/o/2-golosa.o \
									build/o/instance.o \
									build/o/aux.o

OBJECTS_3_SWEEP_ALGORITHM		= 	build/o/3-sweep-algorithm.o \
									build/o/instance.o \
									build/o/aux.o \
									build/o/tsp-solvers.o \
									build/o/clusterization-methods.o

OBJECTS_4_OTRA_CLUSTERIZACION	= 	build/o/4-otra-clusterizacion.o \
									build/o/instance.o \
									build/o/aux.o \
									build/o/tsp-solvers.o \
									build/o/clusterization-methods.o

OBJECTS_5_SIMULATED_ANNEALING	= 	build/o/5-simulated-annealing.o \
									build/o/instance.o \
									build/o/aux.o \
									build/o/savings.o


TARGET_1_SAVINGS				= 	1-savings
TARGET_2_GOLOSA					= 	2-golosa
TARGET_3_SWEEP_ALGORITHM		= 	3-sweep-algorithm
TARGET_4_OTRA_CLUSTERIZACION	= 	4-otra-clusterizacion
TARGET_5_SIMULATED_ANNEALING	= 	5-simulated-annealing

all: directories $(TARGET_1_SAVINGS) $(TARGET_2_GOLOSA) $(TARGET_3_SWEEP_ALGORITHM) $(TARGET_4_OTRA_CLUSTERIZACION) $(TARGET_5_SIMULATED_ANNEALING)

directories:
	$(MKDIR_P) build/bin
	$(MKDIR_P) build/o

$(TARGET_1_SAVINGS):  directories $(OBJECTS_1_SAVINGS)  
	$(CXX) -o build/bin/$(TARGET_1_SAVINGS) $(OBJECTS_1_SAVINGS) $(OBJCOMP) $(LIBS)

$(TARGET_2_GOLOSA): directories $(OBJECTS_2_GOLOSA)  
	$(CXX) -o build/bin/$(TARGET_2_GOLOSA) $(OBJECTS_2_GOLOSA) $(OBJCOMP) $(LIBS)

$(TARGET_3_SWEEP_ALGORITHM): directories $(OBJECTS_3_SWEEP_ALGORITHM)  
	$(CXX) -o build/bin/$(TARGET_3_SWEEP_ALGORITHM) $(OBJECTS_3_SWEEP_ALGORITHM) $(OBJCOMP) $(LIBS)

$(TARGET_4_OTRA_CLUSTERIZACION): directories $(OBJECTS_4_OTRA_CLUSTERIZACION)  
	$(CXX) -o build/bin/$(TARGET_4_OTRA_CLUSTERIZACION) $(OBJECTS_4_OTRA_CLUSTERIZACION) $(OBJCOMP) $(LIBS)

$(TARGET_5_SIMULATED_ANNEALING): directories $(OBJECTS_5_SIMULATED_ANNEALING)  
	$(CXX) -o build/bin/$(TARGET_5_SIMULATED_ANNEALING) $(OBJECTS_5_SIMULATED_ANNEALING) $(OBJCOMP) $(LIBS)

clean: 
	-$(DEL_FILE) $(OBJECTS_1_SAVINGS)
	-$(DEL_FILE) $(OBJECTS_2_GOLOSA)
	-$(DEL_FILE) $(OBJECTS_3_SWEEP_ALGORITHM)
	-$(DEL_FILE) $(OBJECTS_4_OTRA_CLUSTERIZACION)
	-$(DEL_FILE) $(OBJECTS_5_SIMULATED_ANNEALING)
	-$(DEL_FILE) *~ core *.core
	-$(DEL_FILE) build/bin/$(TARGET_1_SAVINGS)
	-$(DEL_FILE) build/bin/$(TARGET_2_GOLOSA)
	-$(DEL_FILE) build/bin/$(TARGET_3_SWEEP_ALGORITHM)
	-$(DEL_FILE) build/bin/$(TARGET_4_OTRA_CLUSTERIZACION)
	-$(DEL_FILE) build/bin/$(TARGET_5_SIMULATED_ANNEALING)


build/o/$(TARGET_1_SAVINGS).o: directories src/$(TARGET_1_SAVINGS).cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/$(TARGET_1_SAVINGS).o src/$(TARGET_1_SAVINGS).cpp

build/o/$(TARGET_2_GOLOSA).o: directories src/$(TARGET_2_GOLOSA).cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/$(TARGET_2_GOLOSA).o src/$(TARGET_2_GOLOSA).cpp

build/o/$(TARGET_3_SWEEP_ALGORITHM).o: directories src/$(TARGET_3_SWEEP_ALGORITHM).cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/$(TARGET_3_SWEEP_ALGORITHM).o src/$(TARGET_3_SWEEP_ALGORITHM).cpp

build/o/$(TARGET_4_OTRA_CLUSTERIZACION).o: directories src/$(TARGET_4_OTRA_CLUSTERIZACION).cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/$(TARGET_4_OTRA_CLUSTERIZACION).o src/$(TARGET_4_OTRA_CLUSTERIZACION).cpp

build/o/$(TARGET_5_SIMULATED_ANNEALING).o: directories src/$(TARGET_5_SIMULATED_ANNEALING).cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/$(TARGET_5_SIMULATED_ANNEALING).o src/$(TARGET_5_SIMULATED_ANNEALING).cpp

build/o/instance.o: directories src/tsplib-helper/instance.cpp src/tsplib-helper/instance.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/instance.o src/tsplib-helper/instance.cpp

build/o/aux.o: directories src/aux.cpp src/aux.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/aux.o src/aux.cpp

build/o/savings.o: directories src/savings/savings.cpp src/savings/savings.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/savings.o src/savings/savings.cpp

build/o/tsp-solvers.o: directories src/tsp-solvers/tsp-solvers.cpp src/tsp-solvers/tsp-solvers.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/tsp-solvers.o src/tsp-solvers/tsp-solvers.cpp

build/o/clusterization-methods.o: directories src/clusterization/clusterization-methods.cpp src/clusterization/clusterization-methods.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/o/clusterization-methods.o src/clusterization/clusterization-methods.cpp	