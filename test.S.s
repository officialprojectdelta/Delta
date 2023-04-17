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
	movl	global(%rip), %edi
	movl	%edi, 4(%rsp)
	callq	test1
	movl	%eax, 4(%rsp)
	movb	$1, %al
	.p2align	4, 0x90
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$4, 4(%rsp)
	je	.LBB1_4
# %bb.2:                                #   in Loop: Header=BB1_1 Depth=1
	decl	4(%rsp)
	je	.LBB1_1
# %bb.3:                                #   in Loop: Header=BB1_1 Depth=1
	testb	%al, %al
	jne	.LBB1_1
.LBB1_4:
	decl	4(%rsp)
	movl	4(%rsp), %eax
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

	.section	".note.GNU-stack","",@progbits
