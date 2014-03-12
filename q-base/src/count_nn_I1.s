	.file	"count_nn_I1.c"
	.text
	.p2align 4,,15
.globl count_nn_I1
	.type	count_nn_I1, @function
count_nn_I1:
.LFB11:
	.cfi_startproc
	testb	%r9b, %r9b
	jne	.L2
	xorl	%eax, %eax
	testq	%rsi, %rsi
	jle	.L4
	.p2align 4,,10
	.p2align 3
.L12:
	cmpb	$0, (%rdx,%rax)
	je	.L9
	movsbl	(%rdi,%rax), %r8d
	movslq	%r8d, %r8
	addq	$1, (%rcx,%r8,8)
.L9:
	addq	$1, %rax
	cmpq	%rsi, %rax
	jne	.L12
.L4:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L2:
	testq	%rsi, %rsi
	jle	.L4
	xorl	%r9d, %r9d
	xorl	%eax, %eax
	.p2align 4,,2
	jmp	.L7
	.p2align 4,,10
	.p2align 3
.L17:
	movslq	%r10d, %r10
	addq	$1, (%rcx,%r10,8)
.L5:
	addq	$1, %r9
	cmpq	%rsi, %r9
	je	.L16
.L7:
	cmpb	$0, (%rdx,%r9)
	je	.L5
	movzbl	(%rdi,%r9), %r10d
	testb	%r10b, %r10b
	js	.L6
	movsbl	%r10b, %r10d
	cmpl	%r8d, %r10d
	jl	.L17
.L6:
	addq	$1, %r9
	movl	$-1, %eax
	cmpq	%rsi, %r9
	jne	.L7
.L16:
	rep
	ret
	.cfi_endproc
.LFE11:
	.size	count_nn_I1, .-count_nn_I1
	.ident	"GCC: (GNU) 4.4.5 20110214 (Red Hat 4.4.5-6)"
	.section	.note.GNU-stack,"",@progbits
