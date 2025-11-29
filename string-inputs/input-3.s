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
 leaq .LC_str0(%rip), %rax
 leaq -16(%rbp), %rdx
 movq %rax, 0(%rdx)
 movq $3, %rax
 leaq -16(%rbp), %rdx
 movq %rax, 8(%rdx)
 leaq -16(%rbp), %rax
 leaq .LC_str1(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -16(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -16(%rbp), %rax
 addq $8, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -16(%rbp), %rcx
 addq $0, %rcx
 pushq %rcx
 leaq .LC_str2(%rip), %rax
 popq %rcx
 movq %rax, (%rcx)
 leaq -16(%rbp), %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $0, %rax
 jmp .end_main
.end_main:
leave
ret
.section .rodata
.LC_str2:
 .string "texto actualizado"
.LC_str1:
 .string "---- Programa 3 ----"
.LC_str0:
 .string "hola desde struct"
.section .note.GNU-stack,"",@progbits
