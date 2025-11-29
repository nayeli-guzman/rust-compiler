.data
print_fmt: 
.string "%ld \n"
print_fmt_str: 
 .string "%s \n"
.text
.globl suma3
suma3:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 movq %rdx, -24(%rbp)
 subq $80, %rsp
 movq $1, -32(%rbp)
 movq $2, -40(%rbp)
 movq $3, -48(%rbp)
 pushq -8(%rbp)
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -56(%rbp)
 pushq -56(%rbp)
 movq -24(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 pushq %rax
 movq $0, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -64(%rbp)
 pushq -64(%rbp)
 movq -32(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 pushq %rax
 movq -40(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 pushq %rax
 movq -48(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -72(%rbp)
 movq -72(%rbp), %rax
 jmp .end_suma3
.end_suma3:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $5, %rax
 movq %rax, %rdi
 movq $6, %rax
 movq %rax, %rsi
 movq $7, %rax
 movq %rax, %rdx
 call suma3
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
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