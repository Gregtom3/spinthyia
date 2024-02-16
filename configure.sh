#!/bin/bash

echo_blue() {
    echo -e "\033[94m$1\033[0m"
}

# Check if the path to the Pythia installation directory was passed as an argument
if [ "$#" -ne 1 ]; then
    echo "This script requires the path to the Pythia installation directory as an argument."
    read -p "Please enter the path to the Pythia installation directory: " pythia_path
else
    pythia_path="$1"
fi

# Verify that the path is not empty
if [ -z "$pythia_path" ]; then
    echo "The path to the Pythia installation directory was not provided. Exiting."
    exit 1
fi

# Verify that the directory exists
if [ ! -d "$pythia_path" ]; then
    echo "The provided path does not exist or is not a directory: $pythia_path"
    exit 1
fi

# Run the ./configure command in the dependencies of StringSpinner
echo_blue "Configuring StringSpinner..."
./deps/stringspinner/configure $pythia_path

# Add the StringSpinner PATH to the Makefile.inc
echo_blue "Storing \$STRINGSPINNERDIR=${PWD}/deps/stringspinner in Makefile.inc"
echo "STRINGSPINNERDIR=${PWD}/deps/stringspinner" >> Makefile.inc

# Exit
echo_blue "All done."
