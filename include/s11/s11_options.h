/*
 * Copyright 2019-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdbool.h>

uint8_t create_sock_addr(
                struct sockaddr_in *addr, 
                uint16_t port, uint32_t val);

void parse_args(int argc, char **argv);
