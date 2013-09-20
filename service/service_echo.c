#include "host_service.h"
#include "host_net.h"
#include "host_log.h"
#include "host_dispatcher.h"
#include "host.h"
#include "hashid.h"
#include "freeid.h"
#include "client_type.h"
#include "user_message.h"

struct client {
    int connid;
    bool connected;
};

struct echo {
    //struct hashid hash;
    int max;
    struct freeid fi;
    struct client* clients;    
};

struct echo*
echo_create() {
    struct echo* self = malloc(sizeof(*self));
    memset(self, 0, sizeof(*self));
    return self;
}

void
echo_free(struct echo* self) {
    if (self == NULL)
        return;

    //hashid_fini(&self->hash);
    freeid_fini(&self->fi);
    free(self->clients);
    free(self);
}

static int
_listen(struct service* s) {
    const char* addr = host_getstr("echo_ip", "");
    int port = host_getint("echo_port", 0);
    if (addr[0] != '\0' &&
        host_net_listen(addr, port, s->serviceid, CLI_GAME)) {
        host_error("listen echo fail");
        return 1;
    }
    return 0;
}

int
echo_init(struct service* s) {
    struct echo* self = SERVICE_SELF;
    if (_listen(s))
        return 1;
    int cmax = host_getint("echoc_max", 0);
    int hmax = host_getint("host_connmax", cmax);
    self->max = cmax;
    self->clients = malloc(sizeof(struct client) * cmax);
    memset(self->clients, 0, sizeof(struct client) * cmax);
    freeid_init(&self->fi, cmax, hmax);
    
    host_info("echoc_max = %d", cmax);
    return 0;
}

static inline struct client*
_getclient(struct echo* self, int id) {
    if (id >= 0 && id < self->max) {
        struct client* c = &self->clients[id];
        if (c->connected)
            return c;
    }
    return NULL;
}

void
echo_usermsg(struct service* s, int id, void* msg, int sz) {
    struct echo* self = SERVICE_SELF;
    int hash = freeid_find(&self->fi, id);
    struct client* c = _getclient(self, hash);
    assert(c);

    UM_CAST(UM_base, um, msg);
    UM_SENDTOCLI(id, um, um->msgsz);
}

static void
_accept(struct echo* self, int connid) {
    int id = freeid_alloc(&self->fi, connid);
    if (id == -1) {
        return;
    }
    assert(id >= 0 && id < self->max);
    struct client* c = &self->clients[id];
    assert(!c->connected);
    c->connected = true;
    c->connid = connid;
    //c->active_time = host_timer_now();
    
    host_net_subscribe(connid, true, false);
}

static void
_ondisconnect(struct echo* self, int connid) {
    int id = freeid_free(&self->fi, connid);
    if (id == -1) {
        return;
    }
    assert(id >= 0 && id < self->max);
    struct client* c = &self->clients[id];
    c->connected = false;
}
/*
static void
_disconnect(struct echo* self, int id, const char* error) {
    assert(id >= 0 && id < self->max);
    struct client* c = &self->clients[id]; 
    assert(c->connected);
    assert(c->connid >= 0);
    int tmp = freeid_free(&self->fi, c->connid);
    assert(tmp == id);
    host_net_close_socket(c->connid);
    c->connected = false;
}
*/
void
echo_net(struct service* s, struct net_message* nm) {
    struct echo* self = SERVICE_SELF;
    switch (nm->type) {
    case NETE_ACCEPT:
        _accept(self, nm->connid);
        break;
    case NETE_SOCKERR:
        _ondisconnect(self, nm->connid);
        break;
    }
}
