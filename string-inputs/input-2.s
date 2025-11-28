.data
print_fmt: 
.string "%ld \n"
TITULO:
 .quad .LC_str0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 leaq .LC_str1(%rip), %rax
 movq %rax, -8(%rbp)
 leaq .LC_str2(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq TITULO(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq .LC_str3(%rip), %rax
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
.LC_str3:
 .string "\"adios\")"
.LC_str2:
 .string "\"---- Programa 2 ----\")"
.LC_str1:
 .string "\"linea local\";"
.LC_str0:
 .string "\"Titulo global\";"
.section .note.GNU-stack,"",@progbits
