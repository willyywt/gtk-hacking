#!/bin/sh
ARG1="$1"
if [ -z $ARG1 ] ; then
	echo "Usage: ./make.sh <hwnumber> | pch" >&2
	exit 1
fi
# Precompile header
if [ $ARG1 == "pch" ] ; then
	g++ `pkg-config --cflags gtk4` -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter framework.h
	exit 0
fi
HW="hw$ARG1"
g++ `pkg-config --cflags gtk4` -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter gtk4-$HW.cpp sap84data.cpp hw_utils.cpp `pkg-config --libs gtk4` -o gtk4-$HW
