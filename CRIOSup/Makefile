TOP=..
include $(TOP)/configure/CONFIG
#=======================================

# Build devAB300 as a library for an IOC:
LIBRARY_IOC += devScalerCRIO

# Library Source files
devScalerCRIO_SRCS += devScalerCRIO.cpp

# Link with the asyn and base libraries
devScalerCRIO_LIBS += asyn
devScalerCRIO_LIBS += std
devScalerCRIO_LIBS += $(EPICS_BASE_IOC_LIBS)
devScalerCRIO_SYS_LIBS += CrioLinux

# Install .dbd and .db files
DBD += devScalerCRIO.dbd
#DB_INSTALLS += devScalerCRIO.db

#=======================================
include $(TOP)/configure/RULES
