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
CND_PLATFORM=GNU-Linux
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
	${OBJECTDIR}/astyle.o \
	${OBJECTDIR}/builder.o \
	${OBJECTDIR}/compile.o \
	${OBJECTDIR}/conv.o \
	${OBJECTDIR}/inte.o \
	${OBJECTDIR}/invoke.o \
	${OBJECTDIR}/line_type.o \
	${OBJECTDIR}/logger.o \
	${OBJECTDIR}/map_key.o \
	${OBJECTDIR}/middle.o \
	${OBJECTDIR}/parser.o \
	${OBJECTDIR}/qcgi_inte.o \
	${OBJECTDIR}/qmap.o \
	${OBJECTDIR}/spc.o \
	${OBJECTDIR}/utility.o


# C Compiler Flags
CFLAGS=-Wall -D_LARGEFILE64_SOURCE -D_FILEOFFSET_BITS==64

# CC Compiler Flags
CCFLAGS=-Wall -D_LARGEFILE64_SOURCE -D_FILEOFFSET_BITS==64
CXXFLAGS=-Wall -D_LARGEFILE64_SOURCE -D_FILEOFFSET_BITS==64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spc

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spc: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spc ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/astyle.o: astyle.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/astyle.o astyle.c

${OBJECTDIR}/builder.o: builder.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/builder.o builder.c

${OBJECTDIR}/compile.o: compile.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/compile.o compile.c

${OBJECTDIR}/conv.o: conv.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/conv.o conv.c

${OBJECTDIR}/inte.o: inte.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inte.o inte.c

${OBJECTDIR}/invoke.o: invoke.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/invoke.o invoke.c

${OBJECTDIR}/line_type.o: line_type.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/line_type.o line_type.c

${OBJECTDIR}/logger.o: logger.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/logger.o logger.c

${OBJECTDIR}/map_key.o: map_key.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/map_key.o map_key.c

${OBJECTDIR}/middle.o: middle.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/middle.o middle.c

${OBJECTDIR}/parser.o: parser.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/parser.o parser.c

${OBJECTDIR}/qcgi_inte.o: qcgi_inte.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/qcgi_inte.o qcgi_inte.c

${OBJECTDIR}/qmap.o: qmap.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/qmap.o qmap.c

${OBJECTDIR}/spc.o: spc.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/spc.o spc.c

${OBJECTDIR}/utility.o: utility.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utility.o utility.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spc

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
