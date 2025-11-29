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
 subq $16, %rsp
 movq $0, %rax
 leaq -16(%rbp), %rdx
 movq %rax, 0(%rdx)
 movq $0, %rax
 leaq -16(%rbp), %rdx
 movq %rax, 8(%rdx)
 leaq -16(%rbp), %rax
while_0:
 leaq -16(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 pushq %rax
 movq $5, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je endwhile_0
 leaq -16(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -16(%rbp), %rcx
 addq $0, %rcx
 pushq %rcx
 leaq -16(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 popq %rcx
 movq %rax, (%rcx)
 jmp while_0
endwhile_0:
 leaq -16(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 pushq %rax
 movq $10, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_1
 movq $100, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_1
 else_1:
 movq $200, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
endif_1:
 movq $0, %rax
 jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
