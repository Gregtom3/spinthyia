include Makefile.inc

GFORTRAN=/usr/local/Cellar/gcc/10.2.0_4/lib/gcc/10
PYTHIAXMLDIR=$(PYTHIADIR)/share/Pythia8/xmldoc
PROG_DIR=$(PWD)/pythia_programs
LIB_DIR=$(PWD)/lib
BIN_DIR=$(PWD)/bin
SRC_DIR=$(PWD)/src
OBJ_DIR=$(BIN_DIR)/obj

CXX=g++
CXXFLAGS=-g -O2 -std=c++11 -fPIC $(shell root-config --cflags)
INCLUDEDIR=$(PYTHIADIR)/include
LIBDIR=$(PYTHIADIR)/lib
ROOTLIBS=$(shell root-config --libs)
FC=gfortran
FFLAGS=-O0 -g -frecord-marker=8 -fbounds-check -fPIC

# Adjust the paths for mc3P0.o and def.o to include STRINGSPINNERDIR
OBJECTS := $(STRINGSPINNERDIR)/mc3P0.o $(STRINGSPINNERDIR)/def.o

# Automatically list all .cc files in the pythia_programs directory
SOURCES := $(wildcard $(PROG_DIR)/*.cc)
# Generate corresponding executable names in the bin directory
EXECUTABLES := $(patsubst $(PROG_DIR)/%.cc,$(BIN_DIR)/%,$(SOURCES))

# Automatically list all .cc files in the src directory
SRC_SOURCES := $(wildcard $(SRC_DIR)/*.cc)
# Generate corresponding object file names in the obj directory
SRC_OBJECTS := $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(SRC_SOURCES))

# Dependency targets
CLASDIS_DIR=./deps/clasdis

# Name of the library
LIB_NAME=libSpinthyia

# Path to the shared library
SHARED_LIB=$(LIB_DIR)/$(LIB_NAME).so

# Default target
all: $(BIN_DIR) $(OBJ_DIR) $(OBJECTS) $(SRC_OBJECTS) $(EXECUTABLES) $(SHARED_LIB) clasdis

# Ensure bin and obj directories exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Rule to compile .cc files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Rule for compiling .cc files
$(BIN_DIR)/%: $(PROG_DIR)/%.cc $(OBJECTS) $(SRC_OBJECTS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -I$(STRINGSPINNERDIR) -o $@ $< $(OBJECTS) $(SRC_OBJECTS) -L$(GFORTRAN) -lgfortran -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) -lpythia8 -ldl $(ROOTLIBS)


# Rule to create the shared library
$(SHARED_LIB): $(SRC_OBJECTS) $(OBJECTS)
	$(CXX) -shared -o $@ $^ $(LDFLAGS) -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) $(ROOTLIBS)
    
# Compile specific program
# Usage: make prog PROG=name_of_program_without_extension
prog: $(BIN_DIR)/$(PROG)

$(BIN_DIR)/$(PROG): $(PROG_DIR)/$(PROG).cc $(OBJECTS) $(SRC_OBJECTS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -I$(STRINGSPINNERDIR) -o $@ $< $(OBJECTS) $(SRC_OBJECTS) -L$(GFORTRAN) -lgfortran -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) -lpythia8 -ldl $(ROOTLIBS)

# Rule to build clasdis
clasdis:
	$(MAKE) -C $(CLASDIS_DIR)

# Rules for compiling mc3P0.o and def.o, ensuring they're built before any .cc files
$(STRINGSPINNERDIR)/def.o: $(STRINGSPINNERDIR)/definitions.f90
	$(FC) $(FFLAGS) -c $< -o $@

$(STRINGSPINNERDIR)/mc3P0.o: $(STRINGSPINNERDIR)/mc3P0.f90 $(STRINGSPINNERDIR)/def.o
	$(FC) $(FFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)/* $(SHARED_LIB) $(OBJECTS) *.mod *~ 
	cd $(CLASDIS_DIR) && $(MAKE) clean
