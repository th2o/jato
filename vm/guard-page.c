/*
 * Copyright (c) 2009 Tomasz Grabiec
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

#include "vm/guard-page.h"
#include "vm/alloc.h"
#include "vm/die.h"

#include <sys/mman.h>
#include <unistd.h>

void *alloc_guard_page(void)
{
	void *p = alloc_page();

	if (p == NULL)
		return NULL;

	if (hide_guard_page(p))
		return NULL;

	return p;
}

int hide_guard_page(void *page_ptr)
{
	return mprotect(page_ptr, getpagesize(), PROT_NONE);
}

int unhide_guard_page(void *page_ptr)
{
	return mprotect(page_ptr, getpagesize(), PROT_READ|PROT_WRITE);
}
