#!/bin/sh
ARG1="$1"
if [ -z $ARG1 ] ; then
	echo "Usage: ./make.sh <hwnumber> | pch" >&2
	exit 1
fi
# Precompile header
if [ $ARG1 == "pch" ] ; then
	g++ `pkg-config --cflags gtk+-3.0` -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter framework.h
	exit 0
fi
HW="hw$ARG1"
g++ `pkg-config --cflags gtk+-3.0` -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter $HW.cpp sap84data-$ARG1.cpp hw_utils.cpp `pkg-config --libs gtk+-3.0` -o $HW
