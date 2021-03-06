/*###############################################################################
#
# This software is distributed under the following ISC license:
#
# Copyright © 2019 BRAZILIAN SYNCHROTRON LIGHT SOURCE <sol@lnls.br>
#   Dawood Alnajjar <dawood.alnajjar@lnls.br>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# Description:
# Support function to setup CRIO environment
#
###############################################################################*/


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
int CRIODebug = 0;

extern struct crio_context * ctx;

epicsExportAddress(int, CRIODebug);

static void inithooks(initHookState state);


static void crioSupSetup(char *path_to_cfgfile, int print_version) {


    /* Prevents someone calling crioSupSetup after ioc boot */
    if (!iocBootComplete) {
        if (print_version == 1)
            printLibVersion();
        ctx = new struct crio_context;

        try {
            crioSetup(ctx, path_to_cfgfile);
        }
            catch(CrioLibException &e) {
            errlogPrintf("Error on initialization - %s \n", e.error_text);
            epicsExit(-1);
        }
    } else
        printf("Setup cannot be done after IOC boot completion. Call this function before IOCINIT...\n");
}

static void crioCleanupCaller(void *param) {
    crioCleanup((struct crio_context*) ctx);
}

static void inithooks(initHookState state) {
    iocBootComplete = (state == initHookAfterIocRunning);
    if (iocBootComplete)
        epicsAtExit(crioCleanupCaller, NULL);
}

/* Shell command to create shared memory access */
static const iocshArg crioSupSetupArg0 = { "Path to configuration file", iocshArgString };
static const iocshArg crioSupSetupArg1 = { "Print libCrioLinux version", iocshArgInt };

static const iocshArg * const crioSupSetupArgs[2] = { &crioSupSetupArg0, &crioSupSetupArg1 };

static const iocshFuncDef crioSupSetupFuncDef = { "crioSupSetup", 2, crioSupSetupArgs };

static void crioSupSetupCallFunc(const iocshArgBuf *args) {
    crioSupSetup(args[0].sval, args[1].ival);
}

static void CRIOreg(void) {
        initHookRegister(&inithooks);
        iocshRegister(&crioSupSetupFuncDef, crioSupSetupCallFunc);
}
epicsExportRegistrar(CRIOreg);


static const iocshArg crioDebugArg0 = { "Enable Debug", iocshArgInt };
static const iocshArg * const crioDebugArgs[1] = { &crioDebugArg0 };
static const iocshFuncDef crioDebugFuncDef = { "crioDebug", 1, crioDebugArgs };
static void crioDebugCallFunc(const iocshArgBuf *args) {
    debug(ctx, args[0].ival);
}

static void CRIODebugReg(void) {
        initHookRegister(&inithooks);
        iocshRegister(&crioDebugFuncDef, crioDebugCallFunc);
}
epicsExportRegistrar(CRIODebugReg);

