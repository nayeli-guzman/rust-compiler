.data
print_fmt: 
.string "%ld \n"
X:
.quad 0
.text
.globl suma
suma:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 subq $32, %rsp
 movq $0, %rax
 movq %rax, -24(%rbp)
 leaq -24(%rbp), %rcx
 pushq %rcx
 movq -8(%rbp), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 popq %rcx
 movq %rax, (%rcx)
 movq -24(%rbp), %rax
 jmp .end_suma
.end_suma:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $2, %rax
 movq %rax, %rdi
 movq $3, %rax
 movq %rax, %rsi
 call suma
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
