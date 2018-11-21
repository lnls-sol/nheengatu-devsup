/* File: smtools.c                     */

/* Common library to access PXI shared memory */
/*
 *      Original Authors: Márcio Paduan Donadio
 *                        Diego de Oliveira Omitto
 *      Contributor: Marco Raulik
 *
 *      Date: 5/Apr/2012
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ellLib.h>
#include <initHooks.h>
#include <epicsExit.h>
#include <epicsExport.h>
#include <iocsh.h>
#include <errlog.h>
#include <CrioLinux.h>

int iocBootComplete = 0;
int debugVar = 0;

extern struct crio_context * ctx;

epicsExportAddress(int, debugVar);

static void inithooks(initHookState state);


static void crioSupSetup(char *path_to_cfgfile, int print_version, const iocshArgBuf params) {


    /* Prevents someone calling crioSupSetup after ioc boot */
    if (!iocBootComplete) {
        if (print_version == 1)
            printLibVersion();
    } else
        printf("Setup cannot be done after IOC boot completion. Call this function before IOCINIT...\n");
}

static void crioCleaup(void *param) {
    crioCleanup((struct crio_context*) ctx);
}

static void inithooks(initHookState state) {
    iocBootComplete = (state == initHookAfterIocRunning);
    if (iocBootComplete)
        epicsAtExit(crioCleaup, NULL);
}

/* Shell command to create shared memory access */
static const iocshArg crioSupSetupArg0 = { "Path to configuration file", iocshArgString };
static const iocshArg crioSupSetupArg1 = { "Print libCrioLinux version", iocshArgInt };
static const iocshArg crioSupSetupArg2 = { "Parameters", iocshArgArgv };

static const iocshArg * const crioSupSetupArgs[3] = { &crioSupSetupArg0, &crioSupSetupArg1, &crioSupSetupArg2 };

static const iocshFuncDef crioSupSetupFuncDef = { "crioSupSetup", 3, crioSupSetupArgs };

static void crioSupSetupCallFunc(const iocshArgBuf *args) {
    crioSupSetup(args[0].sval, args[1].ival, args[2]);
}

static void CRIOreg(void) {
        initHookRegister(&inithooks);
        iocshRegister(&crioSupSetupFuncDef, crioSupSetupCallFunc);
}
epicsExportRegistrar(CRIOreg);
