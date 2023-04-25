	.text
	.file	"test.S"
	.globl	test                    # -- Begin function test
	.p2align	4, 0x90
	.type	test,@function
test:                                   # @test
	.cfi_startproc
# %bb.0:
	movl	$0, -4(%rsp)
	movl	$0, -8(%rsp)
	cmpl	$9, -8(%rsp)
	jg	.LBB0_4
	.p2align	4, 0x90
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
	movl	-4(%rsp), %eax
	movl	%eax, %ecx
	shrl	$31, %ecx
	addl	%eax, %ecx
	andl	$-2, %ecx
	cmpl	%eax, %ecx
	jne	.LBB0_4
# %bb.3:                                #   in Loop: Header=BB0_2 Depth=1
	movl	-8(%rsp), %eax
	addl	%eax, -4(%rsp)
	incl	%eax
	movl	%eax, -8(%rsp)
	cmpl	$9, -8(%rsp)
	jle	.LBB0_2
.LBB0_4:
	movl	-4(%rsp), %eax
	retq
.Lfunc_end0:
	.size	test, .Lfunc_end0-test
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
