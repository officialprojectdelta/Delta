	.text
	.file	"test.c"
	.globl	get_max                         # -- Begin function get_max
	.p2align	4, 0x90
	.type	get_max,@function
get_max:                                # @get_max
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -8(%rbp)
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movq	-8(%rbp), %rcx
	cmpl	(%rcx), %eax
	jle	.LBB0_2
# %bb.1:
	movq	-16(%rbp), %rax
	jmp	.LBB0_3
.LBB0_2:
	movq	-8(%rbp), %rax
.LBB0_3:
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	get_max, .Lfunc_end0-get_max
	.cfi_endproc
                                        # -- End function
	.globl	test                            # -- Begin function test
	.p2align	4, 0x90
	.type	test,@function
test:                                   # @test
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$112, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	movl	a(%rip), %eax
	movl	%eax, -96(%rbp)
	movl	$2, -92(%rbp)
	movl	$3, -88(%rbp)
	movl	$4, -84(%rbp)
	movl	$5, -80(%rbp)
	movl	$6, -76(%rbp)
	movl	$7, -72(%rbp)
	movl	$8, -68(%rbp)
	movl	$9, -64(%rbp)
	movl	$10, -60(%rbp)
	movl	$11, -56(%rbp)
	movl	$2, -52(%rbp)
	movl	$3, -48(%rbp)
	movl	$4, -44(%rbp)
	movl	$5, -40(%rbp)
	movl	$56, -36(%rbp)
	movl	$7, -32(%rbp)
	movl	$8, -28(%rbp)
	movl	$45, -24(%rbp)
	movl	$5, -108(%rbp)
	movl	$10, -104(%rbp)
	movb	$10, -97(%rbp)
	movl	-96(%rbp), %eax
	movq	%fs:40, %rcx
	movq	-8(%rbp), %rdx
	cmpq	%rdx, %rcx
	jne	.LBB1_2
# %bb.1:                                # %SP_return
	addq	$112, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB1_2:                                # %CallStackCheckFailBlk
	.cfi_def_cfa %rbp, 16
	callq	__stack_chk_fail@PLT
.Lfunc_end1:
	.size	test, .Lfunc_end1-test
	.cfi_endproc
                                        # -- End function
	.type	a,@object                       # @a
	.bss
	.globl	a
	.p2align	4
a:
	.zero	60
	.size	a, 60

	.ident	"clang version 15.0.7"
	.section	".note.GNU-stack","",@progbits
