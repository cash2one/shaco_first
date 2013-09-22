#ifndef __host_net_h__
#define __host_net_h__

#include <stdint.h>
#include <stdbool.h>
#include "net.h"

int host_net_init(int max);
void host_net_fini();
int host_net_listen(const char* addr, uint16_t port, int serviceid, int ut);
int host_net_connect(const char* addr, uint16_t port, bool block, int serviceid, int ut);
void host_net_poll(int timeout);
void* host_net_read(int id, int sz, int skip);
void host_net_dropread(int id, int skip);
int host_net_send(int id, void* data, int sz);
void host_net_close_socket(int id);
int host_net_max_socket();
const char* host_net_error();
int host_net_subscribe(int id, bool read, bool write);
int host_net_socket_address(int id, uint32_t* addr, uint16_t* port);
int host_net_socket_isclosed(int id);

#endif
