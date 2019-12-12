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
# EPICS MBBI device support
#
###############################################################################*/

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

    BaseRecord->rval = item;

    return 0;
}


