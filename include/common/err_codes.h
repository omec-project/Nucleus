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

	/*S6a error codes 301-500*/
	S6A_AIA_FAILED = 301,
	S6A_FD_ERROR,
	S6A_FD_CORE_INIT_FAILED,
	S6A_FD_CORE_PARSECONF_FAILED,
	S6A_FD_CORE_START_FAILED,
	S6A_FD_GET_DICTVAL_FAILED,
	S6A_FD_DICTSRCH_FAILED,
};

#endif /* ERROR_CODES_H__*/
