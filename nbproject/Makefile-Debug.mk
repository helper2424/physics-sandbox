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
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/BallBody.o \
	${OBJECTDIR}/Map.o \
	${OBJECTDIR}/Render.o \
	${OBJECTDIR}/Test.o \
	${OBJECTDIR}/VoidSerializer.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/X11/lib box2d/freeglut/libfreeglut_static.a box2d/glui/libglui.a box2d/Box2D/libBox2D.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/megaphysics

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/megaphysics: box2d/freeglut/libfreeglut_static.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/megaphysics: box2d/glui/libglui.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/megaphysics: box2d/Box2D/libBox2D.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/megaphysics: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/megaphysics ${OBJECTFILES} ${LDLIBSOPTIONS} -Wdeprecated-declarations -lGL -lGLUT -lGLU -lX11

${OBJECTDIR}/BallBody.o: BallBody.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -I/usr/local/include/boost -I/usr/local/include/boost -I/usr/local/include -Ibox2d/Box2D -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BallBody.o BallBody.cpp

${OBJECTDIR}/Map.o: Map.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -I/usr/local/include/boost -I/usr/local/include/boost -I/usr/local/include -Ibox2d/Box2D -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Map.o Map.cpp

${OBJECTDIR}/Render.o: Render.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -I/usr/local/include/boost -I/usr/local/include/boost -I/usr/local/include -Ibox2d/Box2D -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Render.o Render.cpp

${OBJECTDIR}/Test.o: Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -I/usr/local/include/boost -I/usr/local/include/boost -I/usr/local/include -Ibox2d/Box2D -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Test.o Test.cpp

${OBJECTDIR}/VoidSerializer.o: VoidSerializer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -I/usr/local/include/boost -I/usr/local/include/boost -I/usr/local/include -Ibox2d/Box2D -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VoidSerializer.o VoidSerializer.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -I/usr/local/include/boost -I/usr/local/include/boost -I/usr/local/include -Ibox2d/Box2D -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/megaphysics

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
