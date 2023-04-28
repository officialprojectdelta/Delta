	.text
	.file	"test.S"
	.globl	test                    # -- Begin function test
	.p2align	4, 0x90
	.type	test,@function
test:                                   # @test
	.cfi_startproc
# %bb.0:
	movl	$0, -4(%rsp)
	.p2align	4, 0x90
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movl	-4(%rsp), %eax
	incl	%eax
	movl	%eax, -4(%rsp)
	cmpl	$4, %eax
	jl	.LBB0_1
# %bb.2:
	movl	-4(%rsp), %eax
	retq
.Lfunc_end0:
	.size	test, .Lfunc_end0-test
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
