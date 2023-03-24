	.text
	.file	"test2.c"
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
	movl	$3, %edi
	callq	spread
	addl	t, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	t, %eax
	subl	$8, %eax
	addq	$16, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	test, .Lfunc_end0-test
	.cfi_endproc
                                        # -- End function
	.globl	spread                  # -- Begin function spread
	.p2align	4, 0x90
	.type	spread,@function
spread:                                 # @spread
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -8(%rbp)
	movl	$0, -4(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$4, -4(%rbp)
	jge	.LBB1_4
# %bb.2:                                #   in Loop: Header=BB1_1 Depth=1
	movl	-8(%rbp), %ecx
	movl	-4(%rbp), %eax
	cltd
	movl	$4, %esi
	idivl	%esi
	addl	%eax, %ecx
	movl	%ecx, -8(%rbp)
# %bb.3:                                #   in Loop: Header=BB1_1 Depth=1
	movl	-4(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -4(%rbp)
	jmp	.LBB1_1
.LBB1_4:
	movl	-8(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end1:
	.size	spread, .Lfunc_end1-spread
	.cfi_endproc
                                        # -- End function
	.globl	blah                    # -- Begin function blah
	.p2align	4, 0x90
	.type	blah,@function
blah:                                   # @blah
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %edi
	callq	spread
	movl	%eax, -12(%rbp)
	leaq	-12(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$16, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end2:
	.size	blah, .Lfunc_end2-blah
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3               # -- Begin function test1
.LCPI3_0:
	.quad	4613712638259704627     # double 2.8999999999999999
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2
.LCPI3_1:
	.long	1090519040              # float 8
.LCPI3_2:
	.long	1092430594              # float 9.82299995
	.text
	.globl	test1
	.p2align	4, 0x90
	.type	test1,@function
test1:                                  # @test1
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movsd	.LCPI3_0(%rip), %xmm0   # xmm0 = mem[0],zero
	movss	.LCPI3_1(%rip), %xmm1   # xmm1 = mem[0],zero,zero,zero
	movss	.LCPI3_2(%rip), %xmm2   # xmm2 = mem[0],zero,zero,zero
	movss	%xmm2, -4(%rbp)
	addss	-4(%rbp), %xmm1
	movss	%xmm1, -4(%rbp)
	movss	-4(%rbp), %xmm1         # xmm1 = mem[0],zero,zero,zero
	cvtss2sd	%xmm1, %xmm1
	addsd	%xmm0, %xmm1
	cvtsd2ss	%xmm1, %xmm0
	movss	%xmm0, -4(%rbp)
	movl	$23, -8(%rbp)
	movss	-4(%rbp), %xmm0         # xmm0 = mem[0],zero,zero,zero
	cvtsi2ssl	-8(%rbp), %xmm1
	addss	%xmm1, %xmm0
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end3:
	.size	test1, .Lfunc_end3-test1
	.cfi_endproc
                                        # -- End function
	.type	t,@object               # @t
	.comm	t,4,4
	.ident	"clang version 10.0.0-4ubuntu1 "
	.section	".note.GNU-stack","",@progbits
