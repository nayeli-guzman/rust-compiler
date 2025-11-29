.data
print_fmt: 
.string "%ld \n"
print_fmt_str: 
 .string "%s \n"
.text
.globl hola
hola:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq $10, %rax
 movq %rax, -8(%rbp)
 movq $20, %rax
 movq %rax, -16(%rbp)
 movq -8(%rbp), %rax
 movq %rax, -24(%rbp)
 leaq -8(%rbp), %rcx
 pushq %rcx
 movq -16(%rbp), %rax
 popq %rcx
 movq %rax, (%rcx)
 leaq -16(%rbp), %rcx
 pushq %rcx
 movq -24(%rbp), %rax
 popq %rcx
 movq %rax, (%rcx)
 movq -8(%rbp), %rax
 movq %rax, 0(%rdi)
 movq -16(%rbp), %rax
 movq %rax, 8(%rdi)
 movq %rdi, %rax
 jmp .end_hola
.end_hola:
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
 call hola
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
