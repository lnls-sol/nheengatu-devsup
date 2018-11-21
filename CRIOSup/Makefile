TOP=..
include $(TOP)/configure/CONFIG
#=======================================

# Build devAB300 as a library for an IOC:
LIBRARY_IOC += devSupCRIO

# Library Source files
devSupCRIO_SRCS += supScalerCRIO.cpp
devSupCRIO_SRCS += supBICRIO.cpp
devSupCRIO_SRCS += supAICRIO.cpp
devSupCRIO_SRCS += supAOCRIO.cpp
devSupCRIO_SRCS += supBOCRIO.cpp
devSupCRIO_SRCS += tools.cpp

# Link with the asyn and base libraries
devSupCRIO_LIBS += asyn
devSupCRIO_LIBS += std
devSupCRIO_LIBS += $(EPICS_BASE_IOC_LIBS)
devSupCRIO_SYS_LIBS += CrioLinux

# Install .dbd and .db files
#DBD += devScalerCRIO.dbd
#DB_INSTALLS += devScalerCRIO.db

#=======================================
include $(TOP)/configure/RULES
