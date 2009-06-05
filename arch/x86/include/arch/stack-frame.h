#ifndef __X86_FRAME_H
#define __X86_FRAME_H

#include <jit/stack-slot.h>
#include <stdbool.h>

struct methodblock;
struct expression;
struct compilation_unit;

struct jit_stack_frame {
       struct jit_stack_frame *prev;
       unsigned long old_ebx;
       unsigned long old_esi;
       unsigned long old_edi;
       unsigned long return_address;
       unsigned long args[0];
} __attribute__((packed));

unsigned long frame_local_offset(struct methodblock *, struct expression *);
unsigned long slot_offset(struct stack_slot *slot);
unsigned long frame_locals_size(struct stack_frame *frame);
unsigned long cu_frame_locals_offset(struct compilation_unit *cu);

/* Replaces C function's return address. */
#define __override_return_address(ret)			\
	({						\
		volatile unsigned long *frame;		\
							\
		frame = __builtin_frame_address(0);	\
		frame[1] = (unsigned long)(ret);	\
	})

/*
 * Removes current C function's arguments from frame.
 *
 *                          STACK
 *
 *                before              after
 *              +---------+        +---------+
 *              | arg0    |        | RET     |
 *              +---------+        +---------+
 *              | RET     |        | OLD EBP |
 *              +---------+        +---------+ <- new EBP
 *              | OLD EBP |        | local0  |
 * EBP -------> +---------+        +---------+
 *              + local0  +        + local1  +
 *              +---------+        +---------+ <- new ESP
 *              + local1  +        +         +
 * ESP -------> +---------+        +---------+
 *
 */
#ifdef CONFIG_X86_32
#define __cleanup_args(args_size)		\
	({								\
	__asm__ volatile (						\
	     "movl %%ebp, %%esi \n"					\
	     "addl %1, %%esi \n"					\
	     "decl %%esi \n"						\
									\
	     "movl %%esi, %%edi \n"					\
	     "addl %%ebx, %%edi \n"					\
									\
	     "movl %%ebp, %%ecx \n"					\
	     "subl %%esp, %%ecx \n"					\
	     "addl %1, %%ecx \n"					\
									\
	     "1: movb (%%esi), %%al \n"					\
	     "movb %%al, (%%edi) \n"					\
	     "decl %%esi \n"						\
	     "decl %%edi \n"						\
	     "decl %%ecx \n"						\
	     "jnz 1b \n"						\
									\
	     "addl %%ebx, %%esp \n"					\
	     "addl %%ebx, %%ebp \n"					\
	     :								\
	     : "b" (args_size), "n"(2*sizeof(unsigned long))		\
	     : "%eax", "%edi", "%esi", "%ecx"				\
									); \
	})
#else
 #error NOT IMPLEMENTED
#endif

#endif
