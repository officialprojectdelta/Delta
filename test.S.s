	.text
	.file	"test.S"
	.globl	test1                   # -- Begin function test1
	.p2align	4, 0x90
	.type	test1,@function
test1:                                  # @test1
	.cfi_startproc
# %bb.0:
                                        # kill: def $edi killed $edi def $rdi
	movl	%edi, -4(%rsp)
	leal	4(%rdi), %eax
	retq
.Lfunc_end0:
	.size	test1, .Lfunc_end0-test1
	.cfi_endproc
                                        # -- End function
	.globl	test                    # -- Begin function test
	.p2align	4, 0x90
	.type	test,@function
test:                                   # @test
	.cfi_startproc
# %bb.0:
	pushq	%rax
	.cfi_def_cfa_offset 16
	movb	$-5, 3(%rsp)
	movl	global(%rip), %edi
	movl	%edi, 4(%rsp)
	addl	$251, %edi
	callq	test1
	movl	%eax, 4(%rsp)
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	test, .Lfunc_end1-test
	.cfi_endproc
                                        # -- End function
	.type	global,@object          # @global
	.data
	.globl	global
	.p2align	2
global:
	.long	42                      # 0x2a
	.size	global, 4

	.type	gb2,@object             # @gb2
	.globl	gb2
gb2:
	.byte	141                     # 0x8d
	.size	gb2, 1

	.section	".note.GNU-stack","",@progbits
