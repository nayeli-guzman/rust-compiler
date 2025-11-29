.data
print_fmt: 
.string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $0, %rax
 movq %rax, -8(%rbp)
 leaq -8(%rbp), %rcx
 pushq %rcx
 movq $10, %rax
 popq %rcx
 movq %rax, (%rcx)
 movq -8(%rbp), %rax
 pushq %rax
 movq $5, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movq $1, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_0
 else_0:
 movq $2, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
endif_0:
 movq $0, %rax
 jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
