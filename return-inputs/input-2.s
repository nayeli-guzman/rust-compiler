.data
print_fmt: 
.string "%ld \n"
.text
.globl gen
gen:
 pushq %rbp
 movq %rsp, %rbp
 movq $1, %rax
 movq %rax, 0(%rdi)
 movq $2, %rax
 movq %rax, 8(%rdi)
 movq $3, %rax
 movq %rax, 16(%rdi)
 movq $4, %rax
 movq %rax, 24(%rdi)
 movq %rdi, %rax
 jmp .end_gen
.end_gen:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 leaq -32(%rbp), %rcx
 subq $32, %rsp
 movq %rsp, %rdi
 call gen
 movq %rax, %rsi
 movq %rcx, %rdi
 movq $4, %rcx
 rep movsq
 movq %rcx, %rax
 leaq -32(%rbp), %rax
 movq %rax, %rcx
 movq $3, %rax
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
