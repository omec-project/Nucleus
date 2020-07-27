#include <iostream>
#include <pthread.h>
#include <thread>
#include "s11Threads.h"
#include <string.h>
#include <sys/stat.h>
#include <blockingCircularFifo.h>
#include <msgBuffer.h>
#include "err_codes.h"
#include <interfaces/s11IpcInterface.h>
#include "s11.h"
#include "msgType.h"
#include <sys/types.h>
#include "log.h"
#include "json_data.h"
#include "timeoutManager.h"
#include <utils/s11TimerUtils.h>
#include "eventMessage.h"
#include "s11_config.h"

using namespace std;
using namespace s11;

/*********************************************************
 *
 * Circular FIFOs for sender IPC and Reader IPC threads
 *
 **********************************************************/
cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> s11IpcIngressFifo_g;
cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> s11IpcEgressFifo_g;

int init_gtpv2();
void s11_reader();
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

struct GtpV2Stack* gtpStack_gp = NULL;

extern char processName[255];
extern int pid;
int g_unix_fd = 0;
struct thread_pool *g_tpool;
pthread_t acceptUnix_t;

int g_s11_fd;
struct sockaddr_in g_s11_cp_addr;
struct sockaddr_in g_client_addr;
socklen_t g_client_addr_size;
socklen_t g_s11_serv_size;

s11_config_t *s11_cfg = NULL;

s11IpcInterface* s11IpcIf_g = NULL;
TimeoutManager* timeoutMgr_g = NULL;

using namespace std::placeholders;

void setThreadName(std::thread* thread, const char* threadName)
{
   	auto handle = thread->native_handle();
	pthread_setname_np(handle,threadName);
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
		return -1;
	}

	if (init_gtpv2() != 0)
		return -1;


	MmeIngressIpcProducerThread ipcReader;
	std::thread t1(ipcReader);
	setThreadName(&t1, "IpcReader");
	t1.detach();

	MmeIngressIpcConsumerThread msgHandlerThread;
	std::thread t2(msgHandlerThread);
	setThreadName(&t2, "s11MsgHandlerThread");
	t2.detach();

	MmeEgressIpcConsumerThread ipcWriter;
	std::thread t3(ipcWriter);
	setThreadName(&t3, "IpcWriter");
	t3.detach();

    if (init_sock() != SUCCESS)
    {
        log_msg(LOG_ERROR, "Error in initializing unix domain socket server.\n");
        return -E_FAIL_INIT;
    }

    std::thread gtp_reader(s11_reader);
	setThreadName(&gtp_reader, "gtpMsgHandlerThread");
	gtp_reader.detach();

	while(1)
	{
        sleep(1);
	}
}

/*Initialize sctp socket connection for eNB*/
int
init_gtpv2()
{
	/*Create UDP socket*/
	g_s11_fd = socket(PF_INET, SOCK_DGRAM, 0);

	g_client_addr.sin_family = AF_INET;
	//g_client_addr.sin_addr.s_addr = htonl(s11_cfg->local_egtp_ip);
	struct in_addr mme_local_addr = {s11_cfg->local_egtp_ip};
	fprintf(stderr, "....................local egtp %s\n", inet_ntoa(mme_local_addr));
	g_client_addr.sin_addr.s_addr = s11_cfg->local_egtp_ip;
	g_client_addr.sin_port = s11_cfg->egtp_def_port;

	bind(g_s11_fd, (struct sockaddr *)&g_client_addr, sizeof(g_client_addr));
	g_client_addr_size = sizeof(g_client_addr);

	/*Configure settings in address struct*/
	g_s11_cp_addr.sin_family = AF_INET;
	//g_s11_cp_addr.sin_port = htons(s11_cfg->egtp_def_port);
	fprintf(stderr, ".................... egtp def port %d\n", s11_cfg->egtp_def_port);
	g_s11_cp_addr.sin_port = htons(s11_cfg->egtp_def_port);
	//g_s11_cp_addr.sin_addr.s_addr = htonl(s11_cfg->sgw_ip);
	struct in_addr sgw_addr = {s11_cfg->sgw_ip};
	fprintf(stderr, "....................sgw ip %s\n", inet_ntoa(sgw_addr));
	g_s11_cp_addr.sin_addr.s_addr = s11_cfg->sgw_ip;
	memset(g_s11_cp_addr.sin_zero, '\0', sizeof(g_s11_cp_addr.sin_zero));

	g_s11_serv_size = sizeof(g_s11_cp_addr);

	return SUCCESS;
}
/**
  Read incoming S11 messages and pass to threadpool
  for processing.
*/
void s11_reader()
{
	unsigned char buffer[S11_GTPV2C_BUF_LEN];
	int len;

	while(1) {
		//len = recvfrom(g_s11_fd, buffer, S11_GTPV2C_BUF_LEN, 0,
		//	&g_client_addr, &g_client_addr_size);
		len = recvfrom(g_s11_fd, buffer, S11_GTPV2C_BUF_LEN, 0,
			(struct sockaddr*)&g_s11_cp_addr, &g_s11_serv_size);

		if(len > 0) {
			MsgBuffer* tmp_buf_p = createMsgBuffer(len);
			MsgBuffer_writeBytes(tmp_buf_p, buffer, len, true);
			MsgBuffer_rewind(tmp_buf_p);

			log_msg(LOG_INFO, "S11 Received msg len : %d \n",len);

			insert_job(g_tpool, handle_s11_message, tmp_buf_p);
		}
	}
}

