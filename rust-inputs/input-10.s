.data
print_fmt: 
.string "%ld \n"
print_fmt_str: 
 .string "%s \n"
.text
.globl cond
cond:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 subq $16, %rsp
 movq -8(%rbp), %rax
 pushq %rax
 movq $4, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movq $0, %rax
 jmp .end_cond
 jmp endif_0
 else_0:
 movq $1, %rax
 jmp .end_cond
endif_0:
.end_cond:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $5, %rax
 movq %rax, %rdi
 call cond
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $3, %rax
 movq %rax, %rdi
 call cond
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
