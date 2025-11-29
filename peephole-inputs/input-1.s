.data
print_fmt: 
.string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq $1, -8(%rbp)
 movq $2, -16(%rbp)
 movq -8(%rbp), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 pushq %rax
 movq $3, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -32(%rbp)
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits