# mark_description "Intel(R) C Intel(R) 64 Compiler XE for applications running on Intel(R) 64, Version 13.0.1.117 Build 2012101";
# mark_description "0";
# mark_description "-I. -std=c99 -O3 -S";
	.file "count_nn_I1.c"
	.text
..TXTST0:
# -- Begin  count_nn_I1
# mark_begin;
       .align    16,0x90
	.globl count_nn_I1
count_nn_I1:
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
# parameter 4: %rcx
# parameter 5: %r8d
# parameter 6: %r9d
..B1.1:                         # Preds ..B1.0
..___tag_value_count_nn_I1.1:                                   #16.1
        xorl      %eax, %eax                                    #17.14
        cmpb      $1, %r9b                                      #20.21
        je        ..B1.14       # Prob 16%                      #20.21
                                # LOE rdx rcx rbx rbp rsi rdi r12 r13 r14 r15 eax r8d
..B1.2:                         # Preds ..B1.1
        testq     %rsi, %rsi                                    #31.32
        jle       ..B1.13       # Prob 10%                      #31.32
                                # LOE rdx rcx rbx rbp rsi rdi r12 r13 r14 r15 eax
..B1.3:                         # Preds ..B1.2
        movq      %rsi, %r9                                     #31.5
        movl      $1, %r10d                                     #31.5
        shrq      $63, %r9                                      #31.5
        xorl      %r8d, %r8d                                    #31.5
        addq      %rsi, %r9                                     #31.5
        sarq      $1, %r9                                       #31.5
        testq     %r9, %r9                                      #31.5
        jbe       ..B1.10       # Prob 0%                       #31.5
                                # LOE rdx rcx rbx rbp rsi rdi r8 r9 r10 r12 r13 r14 r15 eax
..B1.5:                         # Preds ..B1.3 ..B1.8
        cmpb      $0, (%rdx)                                    #32.21
        je        ..B1.8        # Prob 50%                      #32.21
                                # LOE rdx rcx rbx rbp rsi rdi r8 r9 r12 r13 r14 r15 eax
..B1.6:                         # Preds ..B1.5
        incq      %rdx                                          #36.7
        movsbq    (%rdi,%r8,2), %r10                            #33.18
        incq      (%rcx,%r10,8)                                 #35.7
        cmpb      $0, (%rdx)                                    #32.21
        je        ..B1.8        # Prob 50%                      #32.21
                                # LOE rdx rcx rbx rbp rsi rdi r8 r9 r12 r13 r14 r15 eax
..B1.7:                         # Preds ..B1.6
        movsbq    1(%rdi,%r8,2), %r10                           #33.18
        incq      %rdx                                          #36.7
        incq      (%rcx,%r10,8)                                 #35.7
                                # LOE rdx rcx rbx rbp rsi rdi r8 r9 r12 r13 r14 r15 eax
..B1.8:                         # Preds ..B1.5 ..B1.7 ..B1.6
        incq      %r8                                           #31.5
        cmpq      %r9, %r8                                      #31.5
        jb        ..B1.5        # Prob 63%                      #31.5
                                # LOE rdx rcx rbx rbp rsi rdi r8 r9 r12 r13 r14 r15 eax
..B1.9:                         # Preds ..B1.8
        lea       1(,%r8,2), %r10                               #31.5
                                # LOE rdx rcx rbx rbp rsi rdi r10 r12 r13 r14 r15 eax
..B1.10:                        # Preds ..B1.9 ..B1.3
        lea       -1(%r10), %r8                                 #31.5
        cmpq      %r8, %rsi                                     #31.5
        jbe       ..B1.13       # Prob 0%                       #31.5
                                # LOE rdx rcx rbx rbp rdi r10 r12 r13 r14 r15 eax
..B1.11:                        # Preds ..B1.10
        cmpb      $0, (%rdx)                                    #32.21
        je        ..B1.13       # Prob 50%                      #32.21
                                # LOE rcx rbx rbp rdi r10 r12 r13 r14 r15 eax
..B1.12:                        # Preds ..B1.11
        movsbq    -1(%rdi,%r10), %rdx                           #33.18
        incq      (%rcx,%rdx,8)                                 #35.7
                                # LOE rbx rbp r12 r13 r14 r15 eax
..B1.13:                        # Preds ..B1.21 ..B1.12 ..B1.11 ..B1.14 ..B1.2
                                #       ..B1.10
        ret                                                     #39.9
                                # LOE
..B1.14:                        # Preds ..B1.1                  # Infreq
        xorl      %r9d, %r9d                                    #21.23
        testq     %rsi, %rsi                                    #21.32
        jle       ..B1.13       # Prob 10%                      #21.32
                                # LOE rdx rcx rbx rbp rsi rdi r9 r12 r13 r14 r15 eax r8d
..B1.16:                        # Preds ..B1.14 ..B1.21         # Infreq
        cmpb      $0, (%rdx)                                    #22.21
        je        ..B1.21       # Prob 50%                      #22.21
                                # LOE rdx rcx rbx rbp rsi rdi r9 r12 r13 r14 r15 eax r8d
..B1.17:                        # Preds ..B1.16                 # Infreq
        movsbl    (%r9,%rdi), %r10d                             #23.18
        testl     %r10d, %r10d                                  #25.20
        jl        ..B1.19       # Prob 16%                      #25.20
                                # LOE rdx rcx rbx rbp rsi rdi r9 r12 r13 r14 r15 eax r8d r10d
..B1.18:                        # Preds ..B1.17                 # Infreq
        cmpl      %r8d, %r10d                                   #25.36
        jl        ..B1.20       # Prob 50%                      #25.36
                                # LOE rdx rcx rbx rbp rsi rdi r9 r12 r13 r14 r15 eax r8d r10d
..B1.19:                        # Preds ..B1.18 ..B1.17         # Infreq
        movl      $-1, %eax                                     #25.48
        jmp       ..B1.21       # Prob 100%                     #25.48
                                # LOE rdx rcx rbx rbp rsi rdi r9 r12 r13 r14 r15 eax r8d
..B1.20:                        # Preds ..B1.18                 # Infreq
        movslq    %r10d, %r10                                   #26.7
        incq      %rdx                                          #27.7
        incq      (%rcx,%r10,8)                                 #26.7
                                # LOE rdx rcx rbx rbp rsi rdi r9 r12 r13 r14 r15 eax r8d
..B1.21:                        # Preds ..B1.19 ..B1.20 ..B1.16 # Infreq
        incq      %r9                                           #21.35
        cmpq      %rsi, %r9                                     #21.32
        jl        ..B1.16       # Prob 82%                      #21.32
        jmp       ..B1.13       # Prob 100%                     #21.32
        .align    16,0x90
..___tag_value_count_nn_I1.3:                                   #
                                # LOE rdx rcx rbx rbp rsi rdi r9 r12 r13 r14 r15 eax r8d
# mark_end;
	.type	count_nn_I1,@function
	.size	count_nn_I1,.-count_nn_I1
	.data
# -- End  count_nn_I1
	.data
	.section .note.GNU-stack, ""
// -- Begin DWARF2 SEGMENT .eh_frame
	.section .eh_frame,"a",@progbits
.eh_frame_seg:
	.align 8
	.4byte 0x00000014
	.8byte 0x7801000100000000
	.8byte 0x0000019008070c10
	.4byte 0x00000000
	.4byte 0x00000014
	.4byte 0x0000001c
	.8byte ..___tag_value_count_nn_I1.1
	.8byte ..___tag_value_count_nn_I1.3-..___tag_value_count_nn_I1.1
# End
