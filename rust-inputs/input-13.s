.data
print_fmt: 
.string "%ld \n"
.text
.globl fac
fac:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 subq $16, %rsp
 movq -8(%rbp), %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movq -8(%rbp), %rax
 jmp .end_fac
 jmp endif_0
 else_0:
 movq -8(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 subq %rcx, %rax
 movq %rax, %rdi
 call fac
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 jmp .end_fac
endif_0:
.end_fac:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $0, %rax
 movq %rax, -8(%rbp)
 leaq -8(%rbp), %rcx
 pushq %rcx
 movq $1, %rax
 popq %rcx
 movq %rax, (%rcx)
while_1:
 movq -8(%rbp), %rax
 pushq %rax
 movq $20, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je endwhile_1
 movq -8(%rbp), %rax
 movq %rax, %rdi
 call fac
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -8(%rbp), %rcx
 pushq %rcx
 movq -8(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 popq %rcx
 movq %rax, (%rcx)
 jmp while_1
endwhile_1:
 movq $0, %rax
 jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
