/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ERROR_CODES_H_
#define ERROR_CODES_H_

/***
Common error codes across MME
*/

enum ERROR_CODES{
	SUCCESS = 0,
	SUCCESS_1 = 1, 
	/*Generic error codes 101-200*/
	E_FAIL = 101,
	E_FAIL_INIT, /*Failed in initialization*/
	E_ALLOC_FAILED,
	E_PARSING_FAILED,
	E_MAPPING_FAILED,
	E_SKIP,

	/*S1AP error codes 201-300 */
	S1AP_AUTH_FAILED = 201,
	S1AP_SECMODE_FAILED,
	S1AP_IDENTITY_FAILED,
	S1AP_HOREQACK_FAILED,
	S1AP_ERABSURESP_FAILED,

	/*S6a error codes 301-500*/
	S6A_AIA_FAILED = 301,
	S6A_FD_ERROR,
	S6A_FD_CORE_INIT_FAILED,
	S6A_FD_CORE_PARSECONF_FAILED,
	S6A_FD_CORE_START_FAILED,
	S6A_FD_GET_DICTVAL_FAILED,
	S6A_FD_DICTSRCH_FAILED,

	/*S11 error codes 501-700 */
	S11_MODIFY_BEARER_RESP_FAILURE = 501,

	/*MME error codes 701-900 */
	UE_CONTEXT_NOT_FOUND = 701,
	NETWORK_TIMEOUT,
	ABORT_DUE_TO_ATTACH_COLLISION,
	ABORT_DUE_TO_DETACH_COLLISION,
	ABORT_DUE_TO_S1_REL_COLLISION,
	SESSION_CONTAINER_EMPTY,
	SESSION_CONTEXT_NOT_FOUND,
	BEARER_CONTAINER_EMPTY,
	BEARER_CONTEXT_NOT_FOUND,
	INVALID_MSG_BUFFER,
	INVALID_DATA_BUFFER,
	S1AP_FAILURE_IND,
	NAS_ESM_FAILURE_IND,
	PAGING_FAILED,
	DETACH_FAILED,
	MME_AUTH_VALIDATION_FAILURE,
	MAC_MISMATCH
};

#define VERIFY(cond, result, msg, ...) \
    if (!(cond)) { \
        log_msg(LOG_ERROR, msg, ## __VA_ARGS__); \
        result; \
    }

#define VERIFY_UE(cb, ue, msg, ...) \
    if (!(ue)) { \
        log_msg(LOG_ERROR, msg, ## __VA_ARGS__); \
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex()); \
        return ActStatus::HALT; \
    }

#endif /* ERROR_CODES_H__*/
