.data
print_fmt: 
.string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $48, %rsp
 movq $1, -8(%rbp)
 movq $2, -16(%rbp)
 movq $3, -24(%rbp)
 movq -8(%rbp), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -32(%rbp)
 movq -32(%rbp), %rax
 movq %rax, -40(%rbp)
 movq $0, -48(%rbp)
 leaq -24(%rbp), %rcx
 movq -24(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, (%rcx)
 leaq -48(%rbp), %rcx
 movq -32(%rbp), %rax
 movq %rax, (%rcx)
 movq -32(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -40(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -48(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits