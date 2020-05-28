//test
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
#include "mme_app.h"
#include "msgType.h"
#include "stateMachineEngine.h"
#include <sys/types.h>
#include "mmeThreads.h"
#include "log.h"
#include "json_data.h"
#include "monitorSubscriber.h"
#include "timeoutManager.h"
#include <utils/mmeTimerUtils.h>

using namespace std;
using namespace mme;


/*********************************************************
 *
 * Circular FIFOs for sender IPC and Reader IPC threads
 *
 **********************************************************/
cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> mmeIpcIngressFifo_g;
cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> mmeIpcEgressFifo_g;

/*********************************************************
 *
 * Externs
 *
 **********************************************************/
extern "C"
{
#include "thread_pool.h"
int init_sock();
}

extern JobFunction monitorConfigFunc_fpg;
extern void init_parser(char *path);
extern int parse_mme_conf(mme_config *config);
extern void* RunServer(void * data);

extern char processName[255];
extern int pid;
int g_unix_fd = 0;
struct thread_pool *g_tpool;
pthread_t acceptUnix_t;

mme_config g_mme_cfg;
pthread_t stage_tid[5];

MmeIpcInterface* mmeIpcIf_g = NULL;
TimeoutManager* timeoutMgr_g = NULL;

using namespace std::placeholders;

void setThreadName(std::thread* thread, const char* threadName)
{
   	auto handle = thread->native_handle();
	pthread_setname_np(handle,threadName);
}

void mme_parse_config(mme_config *config)
{
    /*Read MME configurations*/
    init_parser((char *)("conf/mme.json"));
    parse_mme_conf(config);
    /* Lets apply logging setting */
    set_logging_level(config->logging);
}

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

    auto cb = std::bind(&MmeTimerUtils::onTimeout, _1);
    timeoutMgr_g = new TimeoutManager(cb);

	StateFactory::Instance()->initialize();

	mmeIpcIf_g = new MmeIpcInterface();
	mmeIpcIf_g->setup();

	mme_parse_config(&g_mme_cfg);

	register_config_updates();

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

	monitorConfigFunc_fpg = &(MonitorSubscriber::handle_monitor_processing);

    if (init_sock() != SUCCESS)
    {
        log_msg(LOG_ERROR, "Error in initializing unix domain socket server.\n");
        return -E_FAIL_INIT;
    }

	while(1)
	{
		SM::StateMachineEngine::Instance()->run();
	}

	return 0;
}
