.data
print_fmt: 
.string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 leaq -24(%rbp), %rcx
 leaq .LC_str0(%rip), %rax
 movq %rax, 0(%rcx)
 leaq .LC_str1(%rip), %rax
 movq %rax, 8(%rcx)
 leaq .LC_str2(%rip), %rax
 movq %rax, 16(%rcx)
 movq %rcx, %rax
 movq $1, %rax
 movq %rax, -32(%rbp)
 leaq .LC_str3(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -24(%rbp), %rax
 movq %rax, %rcx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -24(%rbp), %rax
 movq %rax, %rcx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -24(%rbp), %rax
 movq %rax, %rcx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -24(%rbp), %rcx
 movq %rcx, %rdx
 movq -32(%rbp), %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq %rdx, %rcx
 leaq .LC_str4(%rip), %rax
 movq %rax, (%rcx)
 leaq -24(%rbp), %rax
 movq %rax, %rcx
 movq -32(%rbp), %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
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
.LC_str4:
 .string "\"DOS EN MAYUS\";"
.LC_str3:
 .string "\"---- Programa 4 ----\")"
.LC_str2:
 .string "\"tres\""
.LC_str1:
 .string "\"dos\","
.LC_str0:
 .string "\"uno\","
.section .note.GNU-stack,"",@progbits
