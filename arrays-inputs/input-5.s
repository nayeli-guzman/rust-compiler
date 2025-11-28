.data
print_fmt: 
.string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $96, %rsp
 leaq -40(%rbp), %rcx
 movq $1, %rax
 movq %rax, 0(%rcx)
 movq $2, %rax
 movq %rax, 8(%rcx)
 movq $3, %rax
 movq %rax, 16(%rcx)
 movq $4, %rax
 movq %rax, 24(%rcx)
 movq $5, %rax
 movq %rax, 32(%rcx)
 movq %rcx, %rax
 movq $2, %rax
 movq %rax, -48(%rbp)
 leaq -96(%rbp), %rcx
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
 leaq -40(%rbp), %rax
 movq %rax, %rcx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -40(%rbp), %rax
 movq %rax, %rcx
 movq $3, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -40(%rbp), %rcx
 movq %rcx, %rdx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq %rdx, %rcx
 movq $100, %rax
 movq %rax, (%rcx)
 leaq -40(%rbp), %rax
 movq %rax, %rcx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -96(%rbp), %rax
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
 leaq -96(%rbp), %rax
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
 leaq -96(%rbp), %rcx
 movq %rcx, %rdx
 movq $2, %rax
 imulq $16, %rax
 addq %rax, %rdx
 movq %rdx, %rcx
 addq $0, %rcx
 movq $999, %rax
 movq %rax, (%rcx)
 leaq -96(%rbp), %rax
 movq %rax, %rcx
 movq $2, %rax
 imulq $16, %rax
 addq %rax, %rcx
 movq %rcx, %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -40(%rbp), %rax
 movq %rax, %rcx
 movq -48(%rbp), %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
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
