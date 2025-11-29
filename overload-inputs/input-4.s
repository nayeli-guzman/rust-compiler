.data
print_fmt: 
.string "%ld \n"
print_fmt_str: 
 .string "%s \n"
.text
.globl __op_div_Punto_Punto
__op_div_Punto_Punto:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 subq $32, %rsp
 movq -8(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 pushq %rax
 movq -16(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rcx
 popq %rax
 cqto
 idivq %rcx
 leaq -32(%rbp), %rdx
 movq %rax, 0(%rdx)
 movq -8(%rbp), %rax
 addq $8, %rax
 movq (%rax), %rax
 pushq %rax
 movq -16(%rbp), %rax
 addq $8, %rax
 movq (%rax), %rax
 movq %rax, %rcx
 popq %rax
 cqto
 idivq %rcx
 leaq -32(%rbp), %rdx
 movq %rax, 8(%rdx)
 leaq -32(%rbp), %rax
 leaq -32(%rbp), %rax
 jmp .end___op_div_Punto_Punto
.end___op_div_Punto_Punto:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $64, %rsp
 movq $1, %rax
 leaq -16(%rbp), %rdx
 movq %rax, 0(%rdx)
 movq $2, %rax
 leaq -16(%rbp), %rdx
 movq %rax, 8(%rdx)
 leaq -16(%rbp), %rax
 movq $3, %rax
 leaq -32(%rbp), %rdx
 movq %rax, 0(%rdx)
 movq $4, %rax
 leaq -32(%rbp), %rdx
 movq %rax, 8(%rdx)
 leaq -32(%rbp), %rax
 movq $0, %rax
 leaq -48(%rbp), %rdx
 movq %rax, 0(%rdx)
 movq $0, %rax
 leaq -48(%rbp), %rdx
 movq %rax, 8(%rdx)
 leaq -48(%rbp), %rax
 movq $6, %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 cqto
 idivq %rcx
 movq %rax, -56(%rbp)
 leaq -48(%rbp), %rcx
 pushq %rcx
 leaq -16(%rbp), %rax
 pushq %rax
 leaq -32(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 movq %rax, %rdi
 movq %rcx, %rsi
 call __op_div_Punto_Punto
 movq %rax, %rsi
 popq %rdi
 movq $2, %rcx
 rep movsq
 movq %rdi, %rax
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
 movq -56(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
