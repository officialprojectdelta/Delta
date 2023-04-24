	.text
	.file	"main.ll"
	.globl	test1                   # -- Begin function test1
	.p2align	4, 0x90
	.type	test1,@function
test1:                                  # @test1
	.cfi_startproc
# %bb.0:
	movl	%edi, -4(%rsp)
	movsbl	%dil, %eax
	movb	%al, -5(%rsp)
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
	movabsq	$12312322323, %rax      # imm = 0x2DDDF2113
	movq	%rax, -8(%rsp)
	movabsq	$4597811265048713506, %rax # imm = 0x3FCEB4FC3D849922
	movq	%rax, -16(%rsp)
	movl	$-5, -20(%rsp)
	movw	$-5, -30(%rsp)
	movl	global(%rip), %eax
	movl	%eax, -24(%rsp)
	movl	$983386449, -28(%rsp)   # imm = 0x3A9D4951
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
