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
# EPICS BO device support
#
###############################################################################*/

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
    return 2;
}

static long crio_bo_write(boRecord *BaseRecord) {
    struct crio_context *ctx;

    ctx = (struct crio_context *)BaseRecord->dpvt;
    auto name = BaseRecord->out.value.instio.string;

    try{
       crioSetBOItem(ctx, name, BaseRecord->val);
    }
    catch (CrioLibException &e) {
        errlogPrintf("Error on BO write - %s \n", e.error_text);
        return -1;
    }

    return 0;
}


