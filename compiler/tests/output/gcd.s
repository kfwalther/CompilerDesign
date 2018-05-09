	.text
	.def	 gcd;
	.scl	2;
	.type	32;
	.endef
	.globl	gcd                     # -- Begin function gcd
	.p2align	4, 0x90
gcd:                                    # @gcd
.seh_proc gcd
# %bb.0:                                # %EntryBlock
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movl	%ecx, 32(%rsp)
	movl	%edx, 36(%rsp)
	testl	%edx, %edx
	je	.LBB0_1
# %bb.2:                                # %else
	movl	36(%rsp), %ecx
	movl	32(%rsp), %r8d
	movl	%r8d, %eax
	cltd
	idivl	%ecx
	imull	%ecx, %eax
	subl	%eax, %r8d
	movl	%r8d, %edx
	callq	gcd
	jmp	.LBB0_3
.LBB0_1:                                # %then
	movl	32(%rsp), %eax
.LBB0_3:                                # %ifcont
	addq	$40, %rsp
	retq
	.seh_handlerdata
	.text
	.seh_endproc
                                        # -- End function
	.def	 main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:                                # %EntryBlock
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movl	$0, 32(%rsp)
	movl	$0, 36(%rsp)
	callq	input
	movl	%eax, 32(%rsp)
	callq	input
	movl	%eax, 36(%rsp)
	movl	32(%rsp), %ecx
	movl	%eax, %edx
	callq	gcd
	movl	%eax, %ecx
	callq	output
	nop
	addq	$40, %rsp
	retq
	.seh_handlerdata
	.text
	.seh_endproc
                                        # -- End function

