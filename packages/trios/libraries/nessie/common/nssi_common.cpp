/* ************************************************************************

                   Trios: Trilinos I/O Support
                 Copyright 2011 Sandia Corporation

 Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
 the U.S. Government retains certain rights in this software.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:

 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 3. Neither the name of the Corporation nor the names of the
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Questions? Contact Ron A. Oldfield (raoldfi@sandia.gov)

*************************************************************************/
/*-------------------------------------------------------------------------*/
/**  @file rpc_common.c
 *
 *   @brief  Implementation of the \ref rpc_client_api "RPC Client API".
 *           for the NSSI.
 *
 *   @author Ron Oldfield (raoldfi\@sandia.gov)
 *   $Revision: 1640 $
 *   $Date: 2007-11-28 11:59:53 -0700 (Wed, 28 Nov 2007) $
 *
 */

#include "Trios_config.h"
#include "Trios_nssi_rpc.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#ifdef HAVE_TRIOS_MALLOC_H
#include <malloc.h>
#endif

/* string.h is required on Redsky to get the memcpy() declaration. */
#include <string.h>

#include "Trios_nssi_types.h"
#include "Trios_threads.h"
#include "Trios_timer.h"
#include "Trios_logger.h"
#include "Trios_signal.h"
#include "Trios_nssi_xdr.h"

#include "nssi_debug.h"

#include "nnti.h"



/* --------------------- Private methods ------------------- */

NNTI_transport_t transports[NSSI_RPC_COUNT];

static bool       rpc_initialized = FALSE;
static nssi_rpc_encode encoding        = NSSI_DEFAULT_ENCODE;


void *memdup(void *src, int size)
{
    log_debug(rpc_debug_level, "enter");

    void *new_buf = malloc(size*sizeof(char));
    memcpy(new_buf, src, size);

    return(new_buf);
}

/**
 * @brief Initialize the NSSI RPC mechanism.
 *
 * This implementation of \b nssi_rpc_init initializes the
 * Portals library and interface so that we can use Portals
 * as the underlying communication protocol.
 *
 * @param pid  @input The ID to use for this process.
 */
int nssi_rpc_init(
    const nssi_rpc_transport rpc_transport,
    const nssi_rpc_encode    rpc_encode,
    const char              *url)
{
    int rc;
    static bool initialized = false;

    NNTI_transport_id_t transport_id;

    log_debug(rpc_debug_level, "************************************************************ entered");

    if (initialized) return NSSI_OK;

    /* check to see if logging is enabled */
    if (logger_not_initialized()) {
        logger_set_file(stderr);
    }

    /* make sure the timer works properly */
//    rc = nssi_timer_test();
//    if (rc != NSSI_OK) {
//        log_fatal(rpc_debug_level, "nssi_timer_test() failed");
//        return rc;
//    }

    /* stash the transport and encoding types so we know how to cleanup on exit */
//    transport = rpc_transport;
    encoding  = rpc_encode;

    /* initialize the transport mechanism */
    switch (rpc_transport) {
        case NSSI_RPC_PTL:
            transport_id=NNTI_TRANSPORT_PORTALS;
            break;
        case NSSI_RPC_IB:
            transport_id=NNTI_TRANSPORT_IB;
            break;
        case NSSI_RPC_LUC:
            transport_id=NNTI_TRANSPORT_LUC;
            break;
        case NSSI_RPC_GEMINI:
            transport_id=NNTI_TRANSPORT_GEMINI;
            break;
        default:
            rc = NSSI_ENOENT;
            log_error(rpc_debug_level, "the transport scheme %d does not exist", rpc_transport);
            return rc;
    }

    rc = NNTI_init(
            transport_id,
            url,
            &transports[rpc_transport]);
    if (rc != NNTI_OK) {
        log_fatal(rpc_debug_level,"failed");
        return rc;
    }

    /* initialize the xdr-encoding mechanism */
    switch (rpc_encode) {
        case NSSI_RPC_XDR:
            rc = nssi_xdr_init();
            if (rc != NSSI_OK) {
                log_fatal(rpc_debug_level,"failed, %d", rc);
                return rc;
            }
            break;

        default:
            rc = NSSI_ENOENT;
            log_error(rpc_debug_level, "the transport scheme "
            "does not exist");
            return rc;
    }

    initialized = TRUE;
    rpc_initialized = TRUE;

    if (logging_debug(rpc_debug_level)) {
        fprint_NNTI_peer(logger_get_file(), "transports[rpc_transport].me",
                "end of nssi_rpc_init", &transports[rpc_transport].me);
    }

    log_debug(rpc_debug_level, "************************************************************  exit");

    return NSSI_OK;
}


/**
 * @brief Get the process ID of this process.
 *
 * @ingroup rpc_funcs
 *
 * The <tt>\ref nssi_get_my_pid</tt> method gets the
 * \ref nssi_remote_pid "process id" of the calling process.
 *
 * @param id @output If successful, points to the \ref nssi_remote_pid "process ID"
 *                   of the calling process. Undefined otherwise.
 *
 * @return \ref NSSI_OK Indicates sucess.
 * @return \ref NSSI_ERR_RPC Indicates failure in the NSSI RPC library.
 *
 */
int nssi_get_url(
        const nssi_rpc_transport rpc_transport,
        char                    *url,
        uint32_t                 maxlen)
{
    NNTI_result_t rc=NNTI_OK;

    if (!rpc_initialized) {
        log_error(rpc_debug_level, "RPC not initialized");
        return NSSI_ENOTINIT;
    }

    rc=NNTI_get_url(
            &transports[rpc_transport],
            url,
            maxlen);

    return(rc);
}


/**
 * @brief Finalize the RPC mechanism.
 *
 * The \b nssi_rpc_finalize method performs any cleanup
 * required by the underlying communication protocol
 * before exiting the client application.
 */
int nssi_rpc_fini(const nssi_rpc_transport rpc_transport)
{
    int rc;

    /* initialize the transport mechanism */
    rc = NNTI_fini(&transports[rpc_transport]);
    if (rc != NNTI_OK) {
        log_fatal(rpc_debug_level,"failed");
        return rc;
    }

    /* initialize the xdr-encoding mechanism */
    switch (encoding) {
        case NSSI_RPC_XDR:
            rc = nssi_xdr_fini();
            if (rc != NSSI_OK) {
                log_fatal(rpc_debug_level,"failed, %d", rc);
                return rc;
            }
            break;

        default:
            rc = NSSI_ENOENT;
            log_error(rpc_debug_level, "the transport scheme "
                    "does not exist");
            return rc;
    }

    return NSSI_OK;
}
