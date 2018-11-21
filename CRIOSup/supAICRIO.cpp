
#include <devSup.h>
#include <aiRecord.h>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <epicsExport.h>
#include "errlog.h"
#include <CrioLinux.h>


static long crio_ai_init_rec(aiRecord *BaseRecord);
static long crio_ai_read(aiRecord *BaseRecord);
static long crio_ai_init_dev(int param);


typedef struct {
    long count;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_ai;
    DEVSUPFUN special_linconv;

} AIDeviceSupport;

AIDeviceSupport devAICRIO = {
    6,
    (DEVSUPFUN) NULL,
    (DEVSUPFUN) crio_ai_init_dev,
    (DEVSUPFUN) crio_ai_init_rec,
    (DEVSUPFUN) NULL,
    (DEVSUPFUN) crio_ai_read,
    (DEVSUPFUN) NULL
};

extern "C" { epicsExportAddress(dset, devAICRIO); }

struct crio_context * ctx = NULL;

static long crio_ai_init_dev(int parm)
{
    return 0;
}


static long crio_ai_init_rec(aiRecord *BaseRecord) {

    if (ctx == NULL)
    {
        errlogPrintf("%s called and CRIO ctx was not initialized.\n", __func__);
        return -1;
    }
    BaseRecord->dpvt = ctx;
    return 0;
}

static long crio_ai_read(aiRecord *BaseRecord) {
    double item;
    struct crio_context *ctx;
    ctx = (struct crio_context *)BaseRecord->dpvt;
    auto name = BaseRecord->inp.value.instio.string;

    try{
       crioGetAIItem(ctx, name, item);
    }
    catch (CrioLibException &e) {
        errlogPrintf("Error on read - %s \n", e.error_text);
        return -1;
    }

    BaseRecord->udf = FALSE;
    BaseRecord->val = item;

    return 2;
}


