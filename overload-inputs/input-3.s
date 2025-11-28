.data
print_fmt: 
.string "%ld \n"
.text
.globl restar
restar:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 subq $16, %rsp
 jmp .end_restar
.end_restar:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $48, %rsp
 leaq -16(%rbp), %rcx
 movq $10, %rax
 movq %rax, 0(%rcx)
 movq $20, %rax
 movq %rax, 8(%rcx)
 movq %rcx, %rax
 leaq -32(%rbp), %rcx
 movq $3, %rax
 movq %rax, 0(%rcx)
 movq $7, %rax
 movq %rax, 8(%rcx)
 movq %rcx, %rax
 leaq -48(%rbp), %rcx
 movq $0, %rax
 movq %rax, 0(%rcx)
 movq $0, %rax
 movq %rax, 8(%rcx)
 movq %rcx, %rax
 leaq -48(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -48(%rbp), %rax
 addq $8, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
