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
# EPICS Scaler device support
#
###############################################################################*/


#include <dbScan.h>
#include <callback.h>
#include <devSup.h>
#include <alarm.h>
#include <epicsExport.h>
#include "scalerRecord.h"
#include "devScaler.h"
#include <inttypes.h>
// Linux shared memory
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include<string>
#include <CrioLinux.h>
#include "errlog.h"


#define NDEBUG

#ifdef NDEBUG

#define DEBUG(x)

#else

#define DEBUG(x) {x;} while(0)

#endif

/* Functions of the DSET */
static long crio_scaler_report(int level);
static long crio_scaler_init(int after);
static long crio_scaler_init_record(struct scalerRecord *psr, CALLBACK *pcallback);
static long crio_scaler_get_ioint_info(int cmd, struct dbCommon *precord, IOSCANPVT *ppvt);
static long crio_scaler_reset(scalerRecord *psr);
static long crio_scaler_read(scalerRecord *psr, epicsUInt32 *val);
static long crio_scaler_write_preset(scalerRecord *psr, int signal, epicsUInt32 val);
static long crio_scaler_arm(scalerRecord *psr, int val);
static long crio_scaler_done(scalerRecord *psr);

SCALERDSET devScalerCRIO = {
    9,
    (DEVSUPFUN) crio_scaler_report,
    (DEVSUPFUN) crio_scaler_init,
    (DEVSUPFUN) crio_scaler_init_record,
    (DEVSUPFUN) crio_scaler_get_ioint_info,
    (DEVSUPFUN) crio_scaler_reset,
    (DEVSUPFUN) crio_scaler_read,
    (DEVSUPFUN) crio_scaler_write_preset,
    (DEVSUPFUN) crio_scaler_arm,
    (DEVSUPFUN) crio_scaler_done
};

extern "C" { epicsExportAddress(dset, devScalerCRIO); }

extern struct crio_context *ctx;


static long crio_scaler_report(int level)
{
    DEBUG(printf("[%s] called. level is %d.\n", __func__, level));
    return 0;
}

static long crio_scaler_init(int after)
{
    DEBUG(printf("[%s] called. after = %d\n", __func__, after));
    return 0;
}

static long crio_scaler_init_record(struct scalerRecord *psr, CALLBACK *pcallback)
{
    uint16_t nch;
    DEBUG(printf("[%s] called. Name is %s\n", __func__, psr->out.value.instio.string));

    if (ctx == NULL)
    {
        errlogPrintf("%s ctx global was also null. Scaler init failed.\n", __func__);
        return -1;
    }
    psr->dpvt = (void *) ctx;
    struct crio_context *ctx = (struct crio_context *)psr->dpvt;

    try {
        crioGetNumOfCounters(ctx, psr->out.value.instio.string, &nch);
        psr->nch = static_cast<epicsInt16>(nch);
    }
    catch(CrioLibException &e) {
        errlogPrintf("Error on initialization - scaler - %s \n", e.error_text);
        return -1;
    }
    return 0;
}

static long crio_scaler_get_ioint_info(int cmd, struct dbCommon *precord, IOSCANPVT *ppvt)
{
    DEBUG(printf("[%s] called\n", __func__));
    return 0;
}

static long crio_scaler_reset(scalerRecord *psr)
{
    DEBUG(printf("[%s] called name is = %s\n", __func__, psr->name));
    struct crio_context * ctx;
    ctx = (struct crio_context *)psr->dpvt;
    try {
        crioSetScalerReset(ctx, psr->out.value.instio.string);
    }
    catch(CrioLibException &e) {
        errlogPrintf("Error on scaler reset - %s \n", e.error_text);
        return -1;
    }
    return 0;
}

static long crio_scaler_read(scalerRecord *psr, epicsUInt32 *val)
{
    DEBUG(printf("[%s] called\n", __func__));
    struct crio_context * ctx;
    ctx = (struct crio_context *)psr->dpvt;
    try {
        crioGetScalerCounters(ctx, psr->out.value.instio.string, val);
    }
    catch(CrioLibException &e) {
        errlogPrintf("Error on scaler counter read - %s \n", e.error_text);
        return -1;
    }
    return 0;
}

static long crio_scaler_write_preset(scalerRecord *psr, int signal, epicsUInt32 val)
{
    DEBUG(printf("[%s] called. Signal = %d, val = %u\n", __func__, signal, val));
    struct crio_context * ctx;
    ctx = (struct crio_context *)psr->dpvt;
    try {
        crioSetScalerPresetsGates(ctx, psr->out.value.instio.string, signal, val);
    }
    catch(CrioLibException &e) {
        errlogPrintf("Error on scaler preset write - %s \n", e.error_text);
        return -1;
    }
    return 0;
}

static long crio_scaler_arm(scalerRecord *psr, int val)
{
    DEBUG(printf("[%s] called. Val = %d\n", __func__, val));
    struct crio_context * ctx;
    ctx = (struct crio_context *)psr->dpvt;
    try {
        crioSetScalerArm(ctx, psr->out.value.instio.string, val, true);
    }
    catch(CrioLibException &e) {
        errlogPrintf("Error on scaler arm write - %s \n", e.error_text);
        return -1;
    }
    return 0;
}

static long crio_scaler_done(scalerRecord *psr)
{
    DEBUG(printf("[%s] called\n", __func__));
    struct crio_context * ctx;
    ctx = (struct crio_context *)psr->dpvt;
    bool done = false;
    try {
        crioGetScalerDone(ctx, psr->out.value.instio.string, &done);
        return done;
    }
    catch(CrioLibException &e) {
        errlogPrintf("Error on scaler done read - %s \n", e.error_text);
        return false;
    }
    return false;
}
