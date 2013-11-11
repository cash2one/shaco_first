#ifndef __tplt_h__
#define __tplt_h__

struct tplt;
struct tplt_holder;
struct tplt_visitor;

struct tplt_desc {
    int type; // see TPLT_*
    int size; // sizeof(*_tplt)
    const char* name;
    const struct tplt_visitor_ops* vist;
};

struct tplt* tplt_init(const struct tplt_desc* desc, int sz);
void tplt_fini(struct tplt* self);
const struct tplt_holder* tplt_get_holder(struct tplt* self, int type);
const struct tplt_visitor* tplt_get_visitor(struct tplt* self, int type);

#endif
