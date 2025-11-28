.data
print_fmt: 
.string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 leaq -16(%rbp), %rcx
 leaq .LC_str0(%rip), %rax
 movq %rax, 0(%rcx)
 movq $3, %rax
 movq %rax, 8(%rcx)
 movq %rcx, %rax
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
 leaq .LC_str2(%rip), %rax
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
 .string "\"texto actualizado\";"
.LC_str1:
 .string "\"---- Programa 3 ----\")"
.LC_str0:
 .string "\"hola desde struct\","
.section .note.GNU-stack,"",@progbits
