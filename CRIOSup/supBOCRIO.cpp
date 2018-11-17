
#include <devSup.h>
#include <boRecord.h>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <epicsExport.h>
#include "errlog.h"

#include <CrioLinux.h>

static long crio_bo_init_rec(boRecord *BaseRecord);
static long crio_bo_write(boRecord *BaseRecord);

typedef struct {
    long count;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN getIointInfo;
    DEVSUPFUN read;
} BODeviceSupport;

BODeviceSupport devBOCRIO = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN) crio_bo_init_rec,
    NULL,
    (DEVSUPFUN) crio_bo_write,
};

extern "C" { epicsExportAddress(dset, devBOCRIO); }

extern struct crio_context *ctx;


static long crio_bo_init_rec(boRecord *BaseRecord) {
    if (ctx == NULL)
    {
        errlogPrintf("%s ctx global was also null.\n", __func__);
        return -1;
    }
    BaseRecord->dpvt = (void *) ctx;
    return 0;
}

static long crio_bo_write(boRecord *BaseRecord) {
    struct crio_context *ctx;

    ctx = (struct crio_context *)BaseRecord->dpvt;
    auto name = BaseRecord->out.value.instio.string;

    try{
       crioSetBOItem(ctx, name, BaseRecord->val);
    }
    catch (CrioLibException &e) {
        errlogPrintf("Error on read - %s \n", e.error_text);
        return -1;
    }

    return 0;
}


