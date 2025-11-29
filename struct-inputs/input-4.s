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
 subq $32, %rsp
 leaq -32(%rbp), %rdx
 movq $0, %rax
 movq %rax, 0(%rdx)
 movq $0, %rax
 movq %rax, 8(%rdx)
 leaq -16(%rbp), %rdx
 movq $10, %rax
 movq %rax, 0(%rdx)
 movq $10, %rax
 movq %rax, 8(%rdx)
 leaq -32(%rbp), %rax
 leaq -32(%rbp), %rax
 addq $0, %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -32(%rbp), %rax
 addq $16, %rax
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
