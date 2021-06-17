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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/anchor.o \
	${OBJECTDIR}/auth_socks.o \
	${OBJECTDIR}/auth_utili.o \
	${OBJECTDIR}/base64.o \
	${OBJECTDIR}/capacity.o \
	${OBJECTDIR}/dav_cloud.o \
	${OBJECTDIR}/encoding.o \
	${OBJECTDIR}/file_utili.o \
	${OBJECTDIR}/logger.o \
	${OBJECTDIR}/md5.o \
	${OBJECTDIR}/parsing.o \
	${OBJECTDIR}/query_xml.o \
	${OBJECTDIR}/status_xml.o \
	${OBJECTDIR}/stri_utili.o \
	${OBJECTDIR}/versi_utili.o


# C Compiler Flags
CFLAGS=-Wall -D_DEBUG -D_LARGEFILE64_SOURCE -D_FILEOFFSET_BITS==64

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libdav_cloud.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libdav_cloud.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libdav_cloud.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/anchor.o: anchor.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/anchor.o anchor.c

${OBJECTDIR}/auth_socks.o: auth_socks.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/auth_socks.o auth_socks.c

${OBJECTDIR}/auth_utili.o: auth_utili.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/auth_utili.o auth_utili.c

${OBJECTDIR}/base64.o: base64.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base64.o base64.c

${OBJECTDIR}/capacity.o: capacity.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/capacity.o capacity.c

${OBJECTDIR}/dav_cloud.o: dav_cloud.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dav_cloud.o dav_cloud.c

${OBJECTDIR}/encoding.o: encoding.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/encoding.o encoding.c

${OBJECTDIR}/file_utili.o: file_utili.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/file_utili.o file_utili.c

${OBJECTDIR}/logger.o: logger.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/logger.o logger.c

${OBJECTDIR}/md5.o: md5.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/md5.o md5.c

${OBJECTDIR}/parsing.o: parsing.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/parsing.o parsing.c

${OBJECTDIR}/query_xml.o: query_xml.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/query_xml.o query_xml.c

${OBJECTDIR}/status_xml.o: status_xml.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/status_xml.o status_xml.c

${OBJECTDIR}/stri_utili.o: stri_utili.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stri_utili.o stri_utili.c

${OBJECTDIR}/versi_utili.o: versi_utili.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/usr/include/mysql -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/versi_utili.o versi_utili.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libdav_cloud.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
