	.file	"add2_I4.c"
	.text
	.p2align 4,,15
.globl add2_I4
	.type	add2_I4, @function
add2_I4:
.LFB11:
	.cfi_startproc
	testq	%rdx, %rdx
	jle	.L11
	cmpq	$6, %rdx
	leaq	16(%rcx), %r8
	jbe	.L3
	testb	$15, %cl
	jne	.L3
	leaq	16(%rdi), %rax
	cmpq	%rax, %rcx
	jbe	.L17
.L4:
	leaq	16(%rsi), %rax
	cmpq	%rax, %rcx
	jbe	.L18
.L12:
	movq	%rdx, %r10
	shrq	$2, %r10
	leaq	0(,%r10,4), %r9
	testq	%r9, %r9
	je	.L7
	xorl	%eax, %eax
	xorl	%r8d, %r8d
	.p2align 4,,10
	.p2align 3
.L8:
	movdqu	(%rsi,%rax), %xmm1
	addq	$1, %r8
	movdqu	(%rdi,%rax), %xmm0
	paddd	%xmm1, %xmm0
	movdqa	%xmm0, (%rcx,%rax)
	addq	$16, %rax
	cmpq	%r8, %r10
	ja	.L8
	leaq	0(,%r9,4), %rax
	addq	%rax, %rdi
	addq	%rax, %rsi
	addq	%rax, %rcx
	cmpq	%r9, %rdx
	je	.L11
.L7:
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L9:
	movl	(%rsi,%rax), %r8d
	addl	(%rdi,%rax), %r8d
	addq	$1, %r9
	movl	%r8d, (%rcx,%rax)
	addq	$4, %rax
	cmpq	%r9, %rdx
	jg	.L9
	rep
	ret
	.p2align 4,,10
	.p2align 3
.L17:
	cmpq	%rdi, %r8
	jb	.L4
	.p2align 4,,10
	.p2align 3
.L3:
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L10:
	movl	(%rsi,%rax,4), %r8d
	addl	(%rdi,%rax,4), %r8d
	movl	%r8d, (%rcx,%rax,4)
	addq	$1, %rax
	cmpq	%rdx, %rax
	jne	.L10
.L11:
	rep
	ret
	.p2align 4,,10
	.p2align 3
.L18:
	cmpq	%r8, %rsi
	ja	.L12
	.p2align 4,,8
	jmp	.L3
	.cfi_endproc
.LFE11:
	.size	add2_I4, .-add2_I4
	.ident	"GCC: (GNU) 4.4.5 20110214 (Red Hat 4.4.5-6)"
	.section	.note.GNU-stack,"",@progbits
