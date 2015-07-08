#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/opencl_factory.o \
	${OBJECTDIR}/profile_timer.o \
	${OBJECTDIR}/runtime_exception.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/opt/AMDAPPSDK-2.9-1/lib/x86_64 -lOpenCL

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk opencl_locality

opencl_locality: ${OBJECTFILES}
	${LINK.cc} -o opencl_locality ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/local/cuda-7.0/include -I/opt/AMDAPPSDK-2.9-1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/opencl_factory.o: opencl_factory.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/local/cuda-7.0/include -I/opt/AMDAPPSDK-2.9-1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/opencl_factory.o opencl_factory.cpp

${OBJECTDIR}/profile_timer.o: profile_timer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/local/cuda-7.0/include -I/opt/AMDAPPSDK-2.9-1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/profile_timer.o profile_timer.cpp

${OBJECTDIR}/runtime_exception.o: runtime_exception.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/local/cuda-7.0/include -I/opt/AMDAPPSDK-2.9-1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/runtime_exception.o runtime_exception.cpp

${OBJECTDIR}/runtime_exception.hpp.gch: runtime_exception.hpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/local/cuda-7.0/include -I/opt/AMDAPPSDK-2.9-1/include -std=c++11 -MMD -MP -MF "$@.d" -o "$@" runtime_exception.hpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} opencl_locality

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
