#include "host_service.h"
#include "host_node.h"
#include "host_log.h"
#include "host_dispatcher.h"
#include "node_type.h"
#include "user_message.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct _array {
    int cap;
    int size;
    uint16_t* p;
};

struct _center {
    struct _array subs[NODE_TYPE_MAX];
};

static void
_add_subscribe(struct _array* arr, uint16_t tid) {
    int tmp;
    int i;
    for (i=0; i<arr->size; ++i) {
        tmp = arr->p[i];
        if (tid == tmp)
            return; // already sub
    }
    int idx = arr->size;
    int cap = arr->cap;
    if (idx >= cap) {
        cap *= 2;
        if (cap <= 0)
            cap = 1;
        arr->p = realloc(arr->p, sizeof(arr->p[0]) * cap);
        memset(arr->p + idx, 0, sizeof(arr->p[0] * (cap-idx)));
    }
    arr->p[idx] = tid;
}

struct _center*
center_create() {
    struct _center* self = malloc(sizeof(*self));
    memset(self, 0, sizeof(*self));
    return self;
}

void
center_free(struct _center* self) {
    if (self == NULL)
        return;
    
    struct _array* arr;
    int i;
    for (i=0; i<NODE_TYPE_MAX; ++i) {
        arr = &self->subs[i];
        free(arr->p);
    }
    free(self);
}

int
center_init(struct service* s) {
    SUBSCRIBE_MSG(s->serviceid, UMID_NODE_SUB);
    return 0;
}

static inline bool
_isvalid_tid(uint16_t tid) {
    return tid < NODE_TYPE_MAX;
}

static void
_notify(int id, struct host_node* node) {
    UM_DEFFIX(UM_node_notify, notify, UMID_NODE_NOTIFY);
    notify.tnodeid = node->id;
    notify.addr = node->addr;
    notify.port = node->port;
    UM_SEND(id, &notify, sizeof(notify));
}

static int
_subscribecb(struct host_node* node, void* ud) {
    int id = (int)(intptr_t)ud;
    _notify(id, node);
    return 0;
}

static void
_subscribe(struct _center* self, int id, struct user_message* um) {
    UM_CAST(UM_node_subs, req, um);
    uint16_t src_tid = HNODE_TID(req->nodeid);
    uint16_t tid;
    struct _array* arr;
    int i;
    for (i=0; i<req->n; ++i) {
        tid = req->subs[i];
        if (!_isvalid_tid(tid)) {
            host_error("subscribe fail: invalid tid:%d", tid);
            continue;
        }
        arr = &self->subs[tid];
        _add_subscribe(arr, src_tid);
        host_node_foreach(tid, _subscribecb, (void*)(intptr_t)id);
    }
}

static int
_onregcb(struct host_node* node, void* ud) {
    struct host_node* tnode = ud;
    _notify(node->connid, tnode);
    return 0;
}

static void
_onreg(struct _center* self, struct host_node* node) {
    struct host_node* tnode = node;
    uint16_t tid = HNODE_TID(node->id);
    assert(_isvalid_tid(tid));

    struct _array* arr = &self->subs[tid];
    uint16_t sub;
    int i;
    for (i=0; i<arr->size; ++i) {
        sub = arr->p[i];
        host_node_foreach(sub, _onregcb, tnode); 
    }
}

void
center_service(struct service* s, struct service_message* sm) {
    struct _center* self = SERVICE_SELF;
    struct host_node* regn = sm->msg;
    _onreg(self, regn);
}

void
center_usermsg(struct service* s, int id, void* msg, int sz) {
    struct _center* self = SERVICE_SELF;
    struct user_message* um = msg;
    switch (um->msgid) {
    case UMID_NODE_SUB:
        _subscribe(self, id, um);
        break;
    }
}
