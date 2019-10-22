
#include <devSup.h>
#include <mbbiRecord.h>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <epicsExport.h>
#include "errlog.h"
#include <CrioLinux.h>


static long crio_mbbi_init_rec(mbbiRecord *BaseRecord);
static long crio_mbbi_read(mbbiRecord *BaseRecord);
static long crio_mbbi_init_dev(int param);



typedef struct { /* multi bit binary input dset */
    long number;
    DEVSUPFUN dev_report;
    DEVSUPFUN init;
    DEVSUPFUN init_record; /* returns: (-1,0) => (failure, success)*/
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_mbbi;/* (0, 2) => (success, success no convert)*/

} MBBIDeviceSupport;

MBBIDeviceSupport devMBBICRIO = {
    5,
    (DEVSUPFUN) NULL,
    (DEVSUPFUN) crio_mbbi_init_dev,
    (DEVSUPFUN) crio_mbbi_init_rec,
    (DEVSUPFUN) NULL,
    (DEVSUPFUN) crio_mbbi_read
};

extern "C" { epicsExportAddress(dset, devMBBICRIO); }

extern struct crio_context * ctx;

static long crio_mbbi_init_dev(int parm)
{
    return 0;
}


static long crio_mbbi_init_rec(mbbiRecord *BaseRecord) {

    if (ctx == NULL)
    {
        errlogPrintf("%s called and CRIO ctx was not initialized.\n", __func__);
        return -1;
    }
    BaseRecord->dpvt = ctx;
    return 0;
}

static long crio_mbbi_read(mbbiRecord *BaseRecord) {
    double item;
    struct crio_context *ctx;
    ctx = (struct crio_context *)BaseRecord->dpvt;
    auto name = BaseRecord->inp.value.instio.string;

    try{
       crioGetMBBIItem(ctx, name, item);
    }
    catch (CrioLibException &e) {
        errlogPrintf("Error on MBBI read - %s \n", e.error_text);
        return -1;
    }

    BaseRecord->udf = FALSE;
    BaseRecord->val = item;

    return 2;
}


