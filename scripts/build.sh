#!/bin/bash

# Compile msh and place the executable in bin directory
gcc -I../include/ -o ../bin/msh ../src/*.c