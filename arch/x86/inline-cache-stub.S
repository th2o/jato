.text

.global inline_cache_stub
.type inline_cache_stub, @function
.func inline_cache_stub
inline_cache_stub:
	push	%eax
	call	inline_cache_resolve
	addl	$0x4, %esp
	test	%eax, %eax
	je	exception_occured
	jmp	*%eax
exception_occured:
	jmp	unwind
.endfunc
