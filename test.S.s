	.text
	.file	"test.S"
	.globl	test                    # -- Begin function test
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
	subq	$16, %rsp
	movl	$0, -4(%rbp)
	movb	$1, %al
	testb	%al, %al
	jne	.LBB0_2
# %bb.1:
	movq	%rsp, %rax
	leaq	-16(%rax), %rsp
	movl	$2, -16(%rax)
	movl	$2, %eax
	jmp	.LBB0_4
.LBB0_2:
	movq	%rsp, %rax
	leaq	-16(%rax), %rsp
	movl	$3, -16(%rax)
	cmpl	$2, -4(%rbp)
	jg	.LBB0_5
# %bb.3:
	movl	$4, %eax
	jmp	.LBB0_4
.LBB0_5:
	movl	$5, %eax
.LBB0_4:
	movq	%rbp, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	test, .Lfunc_end0-test
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
