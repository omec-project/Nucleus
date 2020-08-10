/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
#include <thread>
#include <mutex> 
#include <queue>
#include <condition_variable> 
#include <chrono> 
#include <sys/types.h>
#include "s11Threads.h"
#include <string.h>
#include <sys/stat.h>
#include <blockingCircularFifo.h>
#include <msgBuffer.h>
#include "err_codes.h"
#include <interfaces/s11IpcInterface.h>
#include "s11.h"
#include "msgType.h"
#include "log.h"
#include "json_data.h"
#include "timeoutManager.h"
#include <utils/s11TimerUtils.h>
#include "eventMessage.h"
#include "s11_config.h"
#include "msgHandlers/gtpIncomingMsgHandler.h"

using namespace std;

/*********************************************************
 *
 * Circular FIFOs for sender IPC and Reader IPC threads
 *
 **********************************************************/
cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> fromMmeIpcIngressFifo_g;
cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> toMmeIpcIngressFifo_g;

local_endpoint init_gtpv2(uint32_t local_ip, uint16_t local_port);

/* Thread functions */
void s11_reader(void);
void condition_notify(void);
void gtp_msg_processing(uint16_t id);
void handle_s11_message(MsgBuffer *msg); 

/*********************************************************
 *
 * Externs
 *
 **********************************************************/
struct GtpV2Stack* gtpStack_gp = NULL;
extern char processName[255];
extern int pid;
struct thread_pool *g_tpool;
s11_config_t *s11_cfg = NULL;
s11IpcInterface* s11IpcIf_g = NULL;
TimeoutManager* timeoutMgr_g = NULL;
local_endpoint le;


using namespace std::placeholders;

int main(int argc, char *argv[])
{
	memcpy (processName, argv[0], strlen(argv[0]));
	pid = getpid();

	char *hp = getenv("MMERUNENV");
	if (hp && (strcmp(hp, "container") == 0)) {
		init_logging((char*)("container"), NULL);
	}
	else { 
		init_logging((char*)("hostbased"), (char*)("/tmp/mmelogs.txt"));
	}
	init_backtrace(argv[0]);
	srand(time(0));

    auto cb = std::bind(&s11TimerUtils::onTimeout, _1);
    timeoutMgr_g = new TimeoutManager(cb);

	s11IpcIf_g = new s11IpcInterface();
	s11IpcIf_g->setup();

    s11_cfg = new (s11_config_t);
    assert(s11_cfg != NULL);
    s11Config::s11_parse_config(s11_cfg);
	s11Config::register_config_updates();

	// init stack
	gtpStack_gp = createGtpV2Stack();
	if (gtpStack_gp == NULL)
	{
		log_msg(LOG_ERROR, "Error in initializing ipc.\n");
        assert(0);
		return -1;
	}
    log_msg(LOG_INFO, "GTPv2 stack initialized \n");

	le = init_gtpv2(s11_cfg->local_egtp_ip, s11_cfg->egtp_def_port); 

	MmeIpcProducerThread ipcReader;
	std::thread t1(ipcReader);
	t1.detach();
    log_msg(LOG_INFO, "mme-ipc IpcReader started \n");

	MmeIpcConsumerThread msgHandlerThread;
	std::thread t2(msgHandlerThread);
	t2.detach();
    log_msg(LOG_INFO, "mme-ipc consumer started \n");

	GtpMsgConsumerThread ipcWriter;
	std::thread t3(ipcWriter);
	t3.detach();
    log_msg(LOG_INFO, "gtp packets dispath thread started \n");

    std::thread gtp_reader(s11_reader);
	gtp_reader.detach();

    std::thread c_notify(condition_notify);
	c_notify.detach();


    for(int pool=0; pool<2;pool++) {
        log_msg(LOG_INFO, "gtp message processing thread  started \n");
        std::thread gtp_processing(gtp_msg_processing, pool+1);
        gtp_processing.detach();
    }

	while(1)
	{
        std::this_thread::sleep_for (std::chrono::seconds(1));
	}
}

/*Initialize sctp socket connection for eNB*/
local_endpoint
init_gtpv2(uint32_t local_ip, uint16_t local_port)
{
    local_endpoint le;

	/*Create UDP socket*/
	le.s11_fd = socket(PF_INET, SOCK_DGRAM, 0);
	le.local_addr.sin_family = AF_INET;
	le.local_addr.sin_addr.s_addr = htonl(local_ip);
	le.local_addr.sin_port = htons(local_port);
	if(bind(le.s11_fd, (struct sockaddr *)&(le.local_addr), sizeof(struct sockaddr_in)) != 0) {
        log_msg(LOG_ERROR, "failed to bind s11 socker %s",strerror(errno));
        assert(0);
    }
    log_msg(LOG_INFO, "local port opened for gtp packets %s %d \n", inet_ntoa(le.local_addr.sin_addr), local_port);
	return le;
}
/**
  Read incoming S11 messages and pass to threadpool
  for processing.
*/
std::condition_variable cv;
std::mutex gtp_msg_mtx;
std::queue<MsgBuffer*> incoming_gtp_msg_q;

void s11_reader(void)
{
	unsigned char buffer[S11_GTPV2C_BUF_LEN];
	int len;
    socklen_t g_s11_serv_size = sizeof(struct sockaddr_in);
    struct sockaddr_in g_s11_cp_addr = {0};
    log_msg(LOG_INFO, "gtp socket reader started \n");

	while(1) {
		len = recvfrom(le.s11_fd, buffer, S11_GTPV2C_BUF_LEN, 0,
			(struct sockaddr*)&g_s11_cp_addr, &g_s11_serv_size);
		if(len > 0) {
            std::unique_lock<std::mutex> q_lock(gtp_msg_mtx, std::defer_lock);
			log_msg(LOG_INFO, "S11 Received msg len : %d \n",len);
			MsgBuffer* tmp_buf_p = createMsgBuffer(len);
			uint32_t ip = ntohl(g_s11_cp_addr.sin_addr.s_addr);
 			MsgBuffer_writeUint32(tmp_buf_p, ip, true);
			MsgBuffer_writeBytes(tmp_buf_p, buffer, len, true);
			MsgBuffer_rewind(tmp_buf_p);
            q_lock.lock();
            incoming_gtp_msg_q.push(tmp_buf_p);   
            q_lock.unlock();
		}
	}
}

void 
condition_notify(void)
{
    log_msg(LOG_INFO, "notify condition thread started \n");
    while(1) {
        std::unique_lock<std::mutex> lock(gtp_msg_mtx);
        if(!incoming_gtp_msg_q.empty()) {
            lock.unlock();
            cv.notify_one();
        } else {
            lock.unlock();
            std::this_thread::sleep_for (std::chrono::microseconds(100));
        }
    }
}

void 
gtp_msg_processing(uint16_t id)
{
    log_msg(LOG_DEBUG, "Start thread %d for gtp message processing \n", id);
    while(1)
    {
        log_msg(LOG_DEBUG, "thead (%d) - Waiting for condition variable \n",id);
        std::unique_lock<std::mutex> q_lock(gtp_msg_mtx);
        cv.wait(q_lock, [] { return !incoming_gtp_msg_q.empty();}); 
        MsgBuffer *msg = incoming_gtp_msg_q.front();
        incoming_gtp_msg_q.pop();
        q_lock.unlock();
        log_msg(LOG_DEBUG, "thread (%d) - processing gtp message job\n", id);
        gtpIncomingMsgHandler::handle_s11_message(msg);
    }
}
