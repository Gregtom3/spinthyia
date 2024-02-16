# spinthyia

A wrapper for a PYTHIA8.3 plugin [StringSpinner](https://arxiv.org/abs/2105.09730) that integrates quark spin dependence into the hadronization mechanism. 

# Installation

1. Install and Make the 8.3.10 version of PYTHIA on your own machine. The instructions for installing PYTHIA8 can be found [here](https://pythia.org/). This repository was built and tested using the following [tar](https://pythia.org/download/pythia83/pythia8310.tgz).

2. Ensure the latest version of `ROOT` is installed on your machine. This repository was built and tested using `root6.28.04` with `gcc9.2.0`

3. Clone this repository and its dependencies (StringSpinner) by executing
 
```
git clone --recurse-submodules https://github.com/Gregtom3/spinthyia.git
```

4. Setup the path to your PYTHIA installation by executing

```
./configure.sh /path/to/pythia/installation
```

5. The scripts responsible for running the PYTHIA MC Eventgen are located in `./spinthyia/pythia_programs` and have a `.cc` extension. The Makefile included in this repository compiles these scripts using `ROOT`, `PYTHIA8`, and `StringSpinner` dependencies. To compile all files...

```
make
```

To compile a single program and save time (ex: `./spinthyia/pythia_programs/dis.cc`) ...

```
make prog PROG=dis
```

