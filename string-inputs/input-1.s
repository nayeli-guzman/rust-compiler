.data
print_fmt: 
.string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 leaq .LC_str0(%rip), %rax
 movq %rax, -8(%rbp)
 leaq .LC_str1(%rip), %rax
 movq %rax, -16(%rbp)
 leaq .LC_str2(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -16(%rbp), %rax
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
 .string "\"fin del programa 1\")"
.LC_str2:
 .string "\"---- Programa 1 ----\")"
.LC_str1:
 .string "\"mundo\";"
.LC_str0:
 .string "\"hola\";"
.section .note.GNU-stack,"",@progbits
