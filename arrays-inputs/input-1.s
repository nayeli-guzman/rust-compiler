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
 movq $0, %rax
 jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
