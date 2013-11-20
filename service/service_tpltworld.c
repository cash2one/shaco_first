#include "sc_service.h"
#include "tplt_include.h"
#include "tplt_struct.h"

static int
_load() {
#define TBLFILE(name) "./res/tbl/"#name".tbl"
    struct tplt_desc desc[] = {
        { TPLT_ROLE, sizeof(struct role_tplt), 1, TBLFILE(role), 0, TPLT_VIST_VEC32},
    };
    return tplt_init(desc, sizeof(desc)/sizeof(desc[0]));
}

void
tpltworld_free(void* pnull) {
    tplt_fini();
}

int
tpltworld_init(struct service* s) {
    if (_load()) {
        return 1;
    }
    return 0;
}

void
tpltworld_service(struct service* s, struct service_message* sm) {
    tplt_fini();
    _load();
}
