/*
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

#include <iostream>
#include <pthread.h>
#include <thread>
#include <string.h>
#include <sys/stat.h>

#include <blockingCircularFifo.h>
#include <msgBuffer.h>
#include "err_codes.h"
#include <interfaces/mmeIpcInterface.h>
#include <mmeStates/stateFactory.h>
#include "message_queues.h"
#include "mme_app.h"
#include "msgType.h"
#include "stateMachineEngine.h"
#include <sys/types.h>
#include "mmeThreads.h"

extern "C"
{
 	#include "log.h"
	#include "json_data.h"
}

extern void* RunServer(void * data);
using namespace std;
using namespace mme;

/*********************************************************
 *
 * Circular FIFOs for sender IPC and Reader IPC threads
 *
 **********************************************************/
cmn::utils::BlockingCircularFifo<cmn::utils::MsgBuffer, fifoQSize_c> mmeIpcIngressFifo_g;
cmn::utils::BlockingCircularFifo<cmn::utils::MsgBuffer, fifoQSize_c> mmeIpcEgressFifo_g;

/*********************************************************
 *
 * Externs
 *
 **********************************************************/
extern char processName[255];
extern int pid;

mme_config g_mme_cfg;
pthread_t stage_tid[5];

MmeIpcInterface* mmeIpcIf_g = NULL;
void
init_parser(char *path)
{
	load_json(path);
}

int
parse_mme_conf()
{
	/*mme own information*/
	g_mme_cfg.mme_name = get_string_scalar("mme.name");
	if(NULL == g_mme_cfg.mme_name) return E_PARSING_FAILED;

	g_mme_cfg.mme_ip_addr = get_ip_scalar("mme.ip_addr");
	if(E_PARSING_FAILED == g_mme_cfg.mme_ip_addr) return E_PARSING_FAILED;

	g_mme_cfg.mcc_dig1 = get_int_scalar("mme.mcc.dig1");
	if(E_PARSING_FAILED == g_mme_cfg.mcc_dig1) return E_PARSING_FAILED;
	g_mme_cfg.mcc_dig2 = get_int_scalar("mme.mcc.dig2");
	if(E_PARSING_FAILED == g_mme_cfg.mcc_dig1) return E_PARSING_FAILED;
	g_mme_cfg.mcc_dig3 = get_int_scalar("mme.mcc.dig3");
	if(E_PARSING_FAILED == g_mme_cfg.mcc_dig1) return E_PARSING_FAILED;
	g_mme_cfg.mcc_dig1 = get_int_scalar("mme.mnc.dig1");
	if(E_PARSING_FAILED == g_mme_cfg.mcc_dig1) return E_PARSING_FAILED;
	g_mme_cfg.mnc_dig2 = get_int_scalar("mme.mnc.dig2");
	if(E_PARSING_FAILED == g_mme_cfg.mcc_dig1) return E_PARSING_FAILED;
	g_mme_cfg.mnc_dig3 = get_int_scalar("mme.mnc.dig3");
	if(E_PARSING_FAILED == g_mme_cfg.mcc_dig1) return E_PARSING_FAILED;

	return SUCCESS;
}

void setThreadName(std::thread* thread, const char* threadName)
{
   	auto handle = thread->native_handle();
	pthread_setname_np(handle,threadName);
}

int main(int argc, char *argv[])
{
	memcpy (processName, argv[0], strlen(argv[0]));
	pid = getpid();

	StateFactory::Instance()->initialize();

	mmeIpcIf_g = new MmeIpcInterface();
	mmeIpcIf_g->setup();

	init_parser("conf/mme.json");
	parse_mme_conf();

	MmeIngressIpcProducerThread ipcReader;
	std::thread t1(ipcReader);
	setThreadName(&t1, "IpcReader");
	t1.detach();

	MmeIngressIpcConsumerThread msgHandlerThread;
	std::thread t2(msgHandlerThread);
	setThreadName(&t2, "MMEMsgHandlerThread");
	t2.detach();

	MmeEgressIpcConsumerThread ipcWriter;
	std::thread t3(ipcWriter);
	setThreadName(&t3, "IpcWriter");
	t3.detach();

	// start gRPC server
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&stage_tid[0], &attr, &RunServer, NULL);
	pthread_attr_destroy(&attr);

	while(1)
	{
		SM::StateMachineEngine::Instance()->run();
	}

	return 0;
}
