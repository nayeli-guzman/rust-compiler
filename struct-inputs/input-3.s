.data
print_fmt: 
.string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 leaq -16(%rbp), %rcx
 movq $0, %rax
 movq %rax, 0(%rcx)
 movq $0, %rax
 movq %rax, 8(%rcx)
 movq %rcx, %rax
 movq $5, %rax
 movq %rax, -24(%rbp)
 leaq -16(%rbp), %rcx
 movq $10, %rax
 movq %rax, 0(%rcx)
 movq $20, %rax
 movq %rax, 8(%rcx)
 movq %rcx, %rax
 leaq -24(%rbp), %rcx
 pushq %rcx
 movq $1, %rax
 popq %rcx
 movq %rax, (%rcx)
 leaq -16(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -16(%rbp), %rax
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
