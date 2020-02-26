/*
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2019, Infosys Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MESSAGE_QUEUES_H_
#define __MESSAGE_QUEUES_H_

#define S1AP_Q_DIR "/tmp/s1ap"
#define MME_APP_Q_DIR "/tmp/mme-app"
#define S6A_Q_DIR "/tmp/s6a"
#define S11_Q_DIR "/tmp/s11"

#define MME_TOTAL_HANDLERS 7

/**
Message queues used across MME, S1ap, S11, S6a
**/


/********** S1AP READ/WRITE QUEUE *************/
#define S1AP_READ_QUEUE "/tmp/s1ap/s1ap_read_Q"
#define S1AP_WRITE_QUEUE "/tmp/s1ap/s1ap_write_Q"

/********** GTP READ/WRITE QUEUE *************/
#define GTP_READ_QUEUE "/tmp/s11/gtp_read_Q"
#define GTP_WRITE_QUEUE "/tmp/s11/gtp_write_Q"

/********** S6 READ/WRITE QUEUE *************/
#define S6_READ_QUEUE "/tmp/s6a/s6_read_Q"
#define S6_WRITE_QUEUE "/tmp/s6a/s6_write_Q"

#define S6A_REQ_STAGE1_QUEUE "/tmp/s6a/req_Q"
#define S1AP_AUTHREQ_STAGE2_QUEUE "/tmp/s1ap/authq_stage2_Q"
#define S1AP_SECREQ_STAGE3_QUEUE "/tmp/s1ap/secreq_stage3_Q"
#define S1AP_ESMREQ_STAGE4_QUEUE "/tmp/s1ap/esmreq_stage4_Q"
#define S11_CSREQ_STAGE5_QUEUE "/tmp/s11/CSreq_stage5_Q"
#define S1AP_ICSREQ_STAGE6_QUEUE "/tmp/s1ap/icsreq_stage6_Q"
#define S11_MBREQ_STAGE7_QUEUE "/tmp/s11/MBreq_stage7_Q"
#define S11_DTCHREQ_STAGE1_QUEUE   "/tmp/mme-app/s11_dtchreq_stage1_Q"
#define S6A_DTCHREQ_STAGE1_QUEUE  "/tmp/mme-app/s6a_dtchreq_stage1_Q"
#define S1AP_DTCHACCEPT_STAGE2_QUEUE "/tmp/mme-app/s1ap_dtchaccept_stage2_Q"
#define S6A_PURGE_STAGE2_QUEUE "/tmp/s6a/PURGE_Q"

#endif /*__MESSAGE_QUEUES_H*/
