.data
print_fmt: 
.string "%ld \n"
print_fmt_str: 
 .string "%s \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $48, %rsp
 movq $5, %rax
 movq %rax, -8(%rbp)
 movq $7, %rax
 movq %rax, -16(%rbp)
 movq -8(%rbp), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 movq %rax, -32(%rbp)
 movq $0, %rax
 movq %rax, -40(%rbp)
 leaq -8(%rbp), %rcx
 pushq %rcx
 movq -8(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 popq %rcx
 movq %rax, (%rcx)
 leaq -40(%rbp), %rcx
 pushq %rcx
 movq -8(%rbp), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 popq %rcx
 movq %rax, (%rcx)
 movq -24(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
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
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
