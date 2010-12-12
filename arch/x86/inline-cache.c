/*
 * Copyright (c) 2010  Pekka Enberg
 *
 * This file is released under the GPL version 2 with the following
 * clarification and special exception:
 *
 *     Linking this library statically or dynamically with other modules is
 *     making a combined work based on this library. Thus, the terms and
 *     conditions of the GNU General Public License cover the whole
 *     combination.
 *
 *     As a special exception, the copyright holders of this library give you
 *     permission to link this library with independent modules to produce an
 *     executable, regardless of the license terms of these independent
 *     modules, and to copy and distribute the resulting executable under terms
 *     of your choice, provided that you also meet, for each linked independent
 *     module, the terms and conditions of the license of that module. An
 *     independent module is a module which is not derived from or based on
 *     this library. If you modify this library, you may extend this exception
 *     to your version of the library, but you are not obligated to do so. If
 *     you do not wish to do so, delete this exception statement from your
 *     version.
 *
 * Please refer to the file LICENSE for details.
 */

#include <jit/inline-cache.h>

#include <jit/vtable.h>

#include <vm/errors.h>
#include <vm/object.h>
#include <vm/class.h>
#include <vm/die.h>

#include <arch/encode.h>

#include <stdio.h>

struct x86_call {
	void			*site;
	unsigned long		*target;
};

static pthread_mutex_t inline_cache_mutex	= PTHREAD_MUTEX_INITIALIZER;

static void x86_decode_return_addr(struct x86_call *call, void *return_address)
{
	call->site		= return_address - X86_CALL_INSN_SIZE;
	call->target		= call->site + 1;
}

static inline unsigned long x86_call_disp(void *call_site, void *target)
{
	return (unsigned long) target - (unsigned long) call_site - X86_CALL_INSN_SIZE;
}

static bool inline_cache_is_stub(void *return_address)
{
	struct x86_call call;
	bool ret;

	x86_decode_return_addr(&call, return_address);

	if (pthread_mutex_lock(&inline_cache_mutex) != 0)
		die("pthread_mutex_lock");

	ret = x86_call_disp(call.site, inline_cache_stub) == *call.target;

	if (pthread_mutex_unlock(&inline_cache_mutex) != 0)
		die("pthread_mutex_unlock");

	return ret;
}

void inline_cache_fixup(void *return_address, struct vm_object *this, struct vm_class *class, void *target)
{
	struct x86_call call;

	x86_decode_return_addr(&call, return_address);

	if (!inline_cache_is_stub(return_address))
		return;

	if (this->class != class)
		return;

	if (pthread_mutex_lock(&inline_cache_mutex) != 0)
		die("pthread_mutex_lock");

	/* Expected class */
	cpu_write_u32(call.site - 4, (unsigned long) class);

	/* Call target */
	cpu_write_u32(call.site + 1, x86_call_disp(call.site, target - INLINE_CACHE_CHECK_SIZE));

	if (pthread_mutex_unlock(&inline_cache_mutex) != 0)
		die("pthread_mutex_unlock");
}

bool inline_cache_is_call_site(unsigned long *return_address)
{
	return inline_cache_is_stub(return_address);
}

static void *do_inline_cache_resolve(unsigned long method_index, void *return_address, struct vm_object *object)
{
	void *target;

	if (!object)
		return throw_npe();

	assert(method_index < object->class->vtable_size);

	target	= object->class->vtable.native_ptr[method_index];

	assert(target != NULL);

	return target;
}

void *inline_cache_resolve(unsigned long method_index, void *return_address, struct vm_object *object)
{
	return do_inline_cache_resolve(method_index, return_address, object);
}

bool inline_cache_supports_method(struct vm_method *vmm)
{
	assert(!vm_class_is_primitive_class(vmm->class));

	if (method_is_synchronized(vmm))
		return false;

	if (vm_method_is_static(vmm))
		return false;

	if (vm_method_is_native(vmm))
		return false;

	if (vm_method_is_special(vmm))
		return false;

	if (vm_class_is_final(vmm->class))
		return false;

	if (vm_class_is_interface(vmm->class))
		return false;

	if (vm_class_is_array_class(vmm->class))
		return false;

	if (is_array(vmm->class->name))
		return false;

	if (vmm->class->nr_interfaces != 0)
		return false;

	return true;
}
