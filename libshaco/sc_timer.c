#include "sc_timer.h"
#include "sc_init.h"
#include "sc_service.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define INIT_EVENTS 1

struct _event {
    int serviceid;
    int interval;
    uint64_t next_time;
};

struct _event_holder {
    struct _event* p;
    int sz;
    int cap;
};

static void
_event_holder_init(struct _event_holder* eh) {
    eh->cap = INIT_EVENTS;
    eh->sz = 0;
    eh->p = malloc(sizeof(struct _event) * INIT_EVENTS);
}

static void
_event_holder_fini(struct _event_holder* eh) {
    free(eh->p);
    eh->sz = 0;
    eh->cap = 0;
}

static void
_event_holder_grow(struct _event_holder* eh) {
    int old_cap = eh->cap;
    eh->cap *= 2;
    eh->p = realloc(eh->p, sizeof(struct _event) * eh->cap);
    memset(eh->p + old_cap, 0, sizeof(struct _event) * (eh->cap - old_cap));
}

struct sc_timer {
    uint64_t start_time;
    uint64_t elapsed_time;
    bool dirty;
    struct _event_holder eh;
};

static struct sc_timer* T = NULL;

static uint64_t
_now() {
    struct timespec ti;
    clock_gettime(CLOCK_REALTIME, &ti);
    return ti.tv_sec * 1000 + ti.tv_nsec / 1000000;
}

static uint64_t
_elapsed() {
    struct timespec ti;
    clock_gettime(CLOCK_MONOTONIC, &ti);
    return ti.tv_sec * 1000 + ti.tv_nsec / 1000000;
}

static void
_elapsed_time() {
    if (T->dirty) {
        T->dirty = false;
        T->elapsed_time = _elapsed();
    }
}

uint64_t 
sc_timer_now() {
    _elapsed_time();
    return T->start_time + T->elapsed_time;
}

uint64_t 
sc_timer_elapsed() {
    _elapsed_time();
    return T->elapsed_time;
}

uint64_t 
sc_timer_elapsed_real() {
    return _elapsed();
}

static uint64_t
_closest_time() {
    struct _event_holder* eh = &T->eh;
    struct _event* e;
    int i;
    uint64_t min = -1;
    for (i=0; i<eh->sz; ++i) {
        e = &eh->p[i];
        if (e->next_time < min) {
            min = e->next_time;
        }
    }
    return min;
}

int
sc_timer_max_timeout() {
    T->elapsed_time = _elapsed(); 
    T->dirty = true;
    uint64_t next_time = _closest_time(T->elapsed_time);
    int timeout = -1;
    if (next_time != -1) {
        timeout = next_time > T->elapsed_time ?
            next_time - T->elapsed_time : 0;
    } 
    return timeout;
}

void
sc_timer_dispatch_timeout() {
    //T->dirty = true;
    _elapsed_time();
    struct _event_holder* eh = &T->eh;
    struct _event* e;
    int i;
    for (i=0; i<eh->sz; ++i) {
        e = &eh->p[i];
        if (e->next_time <= T->elapsed_time) {
            service_notify_time(e->serviceid);
            e->next_time += e->interval;
        }
    }
}

void
sc_timer_register(int serviceid, int interval) {
    struct _event_holder* eh = &T->eh;
    if (eh->sz >= eh->cap)  {
       _event_holder_grow(eh);
    }
    struct _event* e = &eh->p[eh->sz];
    e->serviceid = serviceid;
    e->interval = interval;
    e->next_time = T->elapsed_time + interval;
    eh->sz += 1;
}

static void
sc_timer_init() {
    T = malloc(sizeof(*T));
    memset(T, 0, sizeof(*T));
    T->dirty = true;
    T->elapsed_time = _elapsed();
    T->start_time = _now() - T->elapsed_time;
    _event_holder_init(&T->eh);
}

static void 
sc_timer_fini() {
    if (T == NULL) 
        return;
    _event_holder_fini(&T->eh);
    free(T);
    T = NULL;
}

SC_LIBRARY_INIT_PRIO(sc_timer_init, sc_timer_fini, 10)
