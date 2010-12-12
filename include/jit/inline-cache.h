#ifndef JATO__JIT_INLINE_CACHE_H
#define JATO__JIT_INLINE_CACHE_H

#include <stdbool.h>

struct vm_object;
struct vm_method;
struct vm_class;

#define INLINE_CACHE_CHECK_SIZE 12

void inline_cache_stub(void);
void inline_cache_fixup(void *call_site, struct vm_object *this, struct vm_class *class, void *target);
void *inline_cache_resolve(unsigned long method_index, void *return_address, struct vm_object *object);
bool inline_cache_is_call_site(unsigned long *addr);
bool inline_cache_supports_method(struct vm_method *method);

#endif /* JATO__JIT_INLINE_CACHE_H */
