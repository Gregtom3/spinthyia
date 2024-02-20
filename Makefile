include Makefile.inc

GFORTRAN=/usr/local/Cellar/gcc/10.2.0_4/lib/gcc/10
PYTHIAXMLDIR=$(PYTHIADIR)/share/Pythia8/xmldoc
PROG_DIR=$(PWD)/pythia_programs
BIN_DIR=$(PWD)/bin

CXX=g++
CXXFLAGS=-g -O2 -std=c++11 $(shell root-config --cflags)
INCLUDEDIR=$(PYTHIADIR)/include
LIBDIR=$(PYTHIADIR)/lib
ROOTLIBS=$(shell root-config --libs)
FC=gfortran
FFLAGS=-O0 -g -frecord-marker=8 -fbounds-check

# Adjust the paths for mc3P0.o and def.o to include STRINGSPINNERDIR
OBJECTS := $(STRINGSPINNERDIR)/mc3P0.o $(STRINGSPINNERDIR)/def.o

# Automatically list all .cc files in the pythia_programs directory
SOURCES := $(wildcard $(PROG_DIR)/*.cc)
# Generate corresponding executable names in the bin directory
EXECUTABLES := $(patsubst $(PROG_DIR)/%.cc,$(BIN_DIR)/%,$(SOURCES))

# Dependency targets
CLASDIS_DIR=./deps/clasdis

# Default target
all: $(BIN_DIR) $(OBJECTS) $(EXECUTABLES) clasdis

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Rule to compile .cc files into executables, now includes correct paths for OBJECTS
$(BIN_DIR)/%: $(PROG_DIR)/%.cc $(OBJECTS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -I$(STRINGSPINNERDIR) -o $@ $< $(OBJECTS) -L$(GFORTRAN) -lgfortran -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) -lpythia8 -ldl $(ROOTLIBS)

# Compile specific program
# Usage: make prog PROG=name_of_program_without_extension
prog: $(BIN_DIR)/$(PROG)

$(BIN_DIR)/$(PROG): $(PROG_DIR)/$(PROG).cc $(OBJECTS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -I$(STRINGSPINNERDIR) -o $@ $< $(OBJECTS) -L$(GFORTRAN) -lgfortran -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) -lpythia8 -ldl $(ROOTLIBS)

# Rule to build clasdis
clasdis:
	$(MAKE) -C $(CLASDIS_DIR)

# Rules for compiling mc3P0.o and def.o, ensuring they're built before any .cc files
$(STRINGSPINNERDIR)/def.o: $(STRINGSPINNERDIR)/definitions.f90
	$(FC) -c $< -o $@

$(STRINGSPINNERDIR)/mc3P0.o: $(STRINGSPINNERDIR)/mc3P0.f90 $(STRINGSPINNERDIR)/def.o
	$(FC) -c $(FFLAGS) $< -o $@

clean:
	rm -rf $(BIN_DIR)/* $(OBJECTS) *.mod *~ 
	cd $(CLASDIS_DIR) && $(MAKE) clean