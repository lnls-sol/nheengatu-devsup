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
# EPICS STRINGOUT device support
#
###############################################################################*/

#include <devSup.h>
#include <stringoutRecord.h>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <epicsExport.h>
#include "errlog.h"

#include <CrioLinux.h>



static long crio_stringout_init_rec(stringoutRecord *BaseRecord);
static long crio_stringout_write(stringoutRecord *BaseRecord);


typedef struct {
    long	number;
    DEVSUPFUN	dev_report;
    DEVSUPFUN	init;
    DEVSUPFUN	init_record; /*returns: (-1,0)=>(failure,success)*/
    DEVSUPFUN	get_ioint_info;
    DEVSUPFUN	write_stringout;/*(-1,0)=>(failure,success)*/

} STRINGOUTDeviceSupport;

STRINGOUTDeviceSupport devSTRINGOUTCRIO = {
    6,
    (DEVSUPFUN) NULL,
    (DEVSUPFUN) NULL,
    (DEVSUPFUN) crio_stringout_init_rec,
    (DEVSUPFUN) NULL,
    (DEVSUPFUN) crio_stringout_write
};

extern "C" { epicsExportAddress(dset, devSTRINGOUTCRIO); }

extern struct crio_context * ctx;



static long crio_stringout_init_rec(stringoutRecord *BaseRecord) {

    if (ctx == NULL)
    {
        errlogPrintf("%s called and CRIO ctx was not initialized.\n", __func__);
        return -1;
    }
    BaseRecord->dpvt = ctx;
    return 0;
}

static long crio_stringout_write(stringoutRecord *BaseRecord) {
    struct crio_context *ctx;

    ctx = (struct crio_context *)BaseRecord->dpvt;
    auto name = BaseRecord->out.value.instio.string;

    try{
       crioSetSTRINGOUTItem(ctx, name, BaseRecord->val);
    }
    catch (CrioLibException &e) {
        errlogPrintf("Error on STRINGOUT write - %s \n", e.error_text);
        return -1;
    }

    return 0;
}


