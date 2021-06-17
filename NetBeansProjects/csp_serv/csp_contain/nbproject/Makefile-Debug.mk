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
	${OBJECTDIR}/_ext/f534dc3f/access_log.o \
	${OBJECTDIR}/_ext/f534dc3f/logger.o \
	${OBJECTDIR}/_ext/f534dc3f/path_hmap.o \
	${OBJECTDIR}/_ext/f534dc3f/stri_utili.o \
	${OBJECTDIR}/_ext/f534dc3f/tiny_pool.o \
	${OBJECTDIR}/base_pool.o \
	${OBJECTDIR}/csp_contain.o \
	${OBJECTDIR}/file_utili.o \
	${OBJECTDIR}/map_path.o \
	${OBJECTDIR}/parse_conf.o \
	${OBJECTDIR}/virtu_path.o


# C Compiler Flags
CFLAGS=-Wall -D_DEBUG -D_LARGEFILE64_SOURCE

# CC Compiler Flags
CCFLAGS=-Wall -D_DEBUG -D_LARGEFILE64_SOURCE
CXXFLAGS=-Wall -D_DEBUG -D_LARGEFILE64_SOURCE

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/csp_contain

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/csp_contain: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/csp_contain ${OBJECTFILES} ${LDLIBSOPTIONS} -lfcgi -ldl -lxml2 -lqdecoder -lmysqlclient -L/usr/lib64/mysql

${OBJECTDIR}/_ext/f534dc3f/access_log.o: ../third_party/access_log.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/access_log.o ../third_party/access_log.c

${OBJECTDIR}/_ext/f534dc3f/logger.o: ../third_party/logger.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/logger.o ../third_party/logger.c

${OBJECTDIR}/_ext/f534dc3f/path_hmap.o: ../third_party/path_hmap.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/path_hmap.o ../third_party/path_hmap.c

${OBJECTDIR}/_ext/f534dc3f/stri_utili.o: ../third_party/stri_utili.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/stri_utili.o ../third_party/stri_utili.c

${OBJECTDIR}/_ext/f534dc3f/tiny_pool.o: ../third_party/tiny_pool.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f534dc3f
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f534dc3f/tiny_pool.o ../third_party/tiny_pool.c

${OBJECTDIR}/base_pool.o: base_pool.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base_pool.o base_pool.c

${OBJECTDIR}/csp_contain.o: csp_contain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/csp_contain.o csp_contain.cpp

${OBJECTDIR}/file_utili.o: file_utili.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/file_utili.o file_utili.c

${OBJECTDIR}/map_path.o: map_path.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/map_path.o map_path.c

${OBJECTDIR}/parse_conf.o: parse_conf.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/parse_conf.o parse_conf.c

${OBJECTDIR}/virtu_path.o: virtu_path.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I../third_party -I/usr/include/mysql -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/virtu_path.o virtu_path.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/csp_contain

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
