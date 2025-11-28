.data
print_fmt: 
.string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $48, %rsp
 leaq -48(%rbp), %rcx
 leaq 0(%rcx), %rdx
 movq $10, %rax
 movq %rax, 0(%rdx)
 movq $20, %rax
 movq %rax, 8(%rdx)
 leaq 16(%rcx), %rdx
 movq $30, %rax
 movq %rax, 0(%rdx)
 movq $40, %rax
 movq %rax, 8(%rdx)
 leaq 32(%rcx), %rdx
 movq $50, %rax
 movq %rax, 0(%rdx)
 movq $60, %rax
 movq %rax, 8(%rdx)
 movq %rcx, %rax
 leaq -48(%rbp), %rax
 movq %rax, %rcx
 movq $0, %rax
 imulq $16, %rax
 addq %rax, %rcx
 movq %rcx, %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -48(%rbp), %rax
 movq %rax, %rcx
 movq $1, %rax
 imulq $16, %rax
 addq %rax, %rcx
 movq %rcx, %rax
 addq $8, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $0, %rax
 jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
