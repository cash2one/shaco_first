#include "hall_tplt.h"
#include "hall.h"
#include "sc.h"

static int
load_tplt(struct hall *self) {
    tplt_free(self->T);
#define TBLFILE(name) "./res/tbl/"#name".tbl"
    struct tplt_desc desc[] = {
        { TPLT_ROLE, sizeof(struct role_tplt), 1, TBLFILE(role), 0, TPLT_VIST_VEC32},
        { TPLT_RING, sizeof(struct ring_tplt), 1, TBLFILE(ring), 0, TPLT_VIST_VEC32},
        { TPLT_EXP,  sizeof(struct exp_tplt),  1, TBLFILE(exp),  0, TPLT_VIST_INDEX32},
    };
    self->T = tplt_create(desc, sizeof(desc)/sizeof(desc[0]));
    return self->T ? 0 : 1;
}

int
hall_tplt_init(struct hall *self) {
    if (load_tplt(self)) {
        return 1;
    }
    return 0;
}

void
hall_tplt_fini(struct hall *self) {
    if (self->T) {
        tplt_free(self->T);
        self->T = NULL;
    }
}

void
hall_tplt_main(struct module *s, int session, int source, int type, const void *msg, int sz) {
    struct hall *self = MODULE_SELF;
    if (type != MT_TEXT)
        return;

    if (!strncmp("reload", msg, sz)) {
        if (!load_tplt(self)) {
            sh_info("reload tplt ok");
        } else {
            sh_error("reload tplt fail");
        }
    }
}
