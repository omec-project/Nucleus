/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */


class MonitorSubscriber
{
public:
    static void handle_monitor_processing(void *message);

private:
    MonitorSubscriber();
    ~MonitorSubscriber();

    static void handle_monitor_imsi_req(struct monitor_imsi_req *mir, int sock_fd);
    static void handle_imsi_list_req(struct monitor_imsi_req *mir, int sock_fd);
    static void handle_imsi_flush_req();
};


