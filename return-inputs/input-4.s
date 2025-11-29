.data
print_fmt: 
.string "%ld \n"
print_fmt_str: 
 .string "%s \n"
.text
.globl build
build:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq $30, %rax
 movq %rax, -8(%rbp)
 movq $12, %rax
 movq %rax, -16(%rbp)
 movq -8(%rbp), %rax
 movq %rax, -24(%rbp)
 movq -8(%rbp), %rax
 movq %rax, -32(%rbp)
 movq -16(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 leaq -24(%rbp), %rcx
 pushq %rcx
 movq -16(%rbp), %rax
 popq %rcx
 movq %rax, (%rcx)
 jmp endif_0
 else_0:
 leaq -32(%rbp), %rcx
 pushq %rcx
 movq -16(%rbp), %rax
 popq %rcx
 movq %rax, (%rcx)
endif_0:
 movq -24(%rbp), %rax
 movq %rax, 0(%rdi)
 movq -32(%rbp), %rax
 movq %rax, 8(%rdi)
 movq %rdi, %rax
 jmp .end_build
.end_build:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 leaq -16(%rbp), %rcx
 subq $16, %rsp
 movq %rsp, %rdi
 call build
 movq %rax, %rsi
 movq %rcx, %rdi
 movq $2, %rcx
 rep movsq
 movq %rcx, %rax
 leaq -16(%rbp), %rax
 movq %rax, %rcx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -16(%rbp), %rax
 movq %rax, %rcx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
