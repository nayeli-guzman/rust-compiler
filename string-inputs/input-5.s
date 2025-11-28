.data
print_fmt: 
.string "%ld \n"
.text
.globl saludar
saludar:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 subq $16, %rsp
 leaq .LC_str0(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $0, %rax
 jmp .end_saludar
.end_saludar:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 leaq -32(%rbp), %rcx
 leaq 0(%rcx), %rdx
 leaq .LC_str1(%rip), %rax
 movq %rax, 0(%rdx)
 movq $20, %rax
 movq %rax, 8(%rdx)
 leaq 16(%rcx), %rdx
 leaq .LC_str2(%rip), %rax
 movq %rax, 0(%rdx)
 movq $25, %rax
 movq %rax, 8(%rdx)
 movq %rcx, %rax
 leaq .LC_str3(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -32(%rbp), %rax
 movq %rax, %rcx
 movq $0, %rax
 imulq $16, %rax
 addq %rax, %rcx
 movq %rcx, %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -32(%rbp), %rax
 movq %rax, %rcx
 movq $0, %rax
 imulq $16, %rax
 addq %rax, %rcx
 movq %rcx, %rax
 addq $8, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -32(%rbp), %rax
 movq %rax, %rcx
 movq $1, %rax
 imulq $16, %rax
 addq %rax, %rcx
 movq %rcx, %rax
 addq $0, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -32(%rbp), %rax
 movq %rax, %rcx
 movq $1, %rax
 imulq $16, %rax
 addq %rax, %rcx
 movq %rcx, %rax
 addq $8, %rax
 movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq .LC_str4(%rip), %rax
 mov %rax, %rdi
call saludar
 leaq -32(%rbp), %rax
 movq %rax, %rcx
 movq $0, %rax
 imulq $16, %rax
 addq %rax, %rcx
 movq %rcx, %rax
 addq $0, %rax
 movq (%rax), %rax
 mov %rax, %rdi
call saludar
 movq $0, %rax
 jmp .end_main
.end_main:
leave
ret
.section .rodata
.LC_str4:
 .string "\"desde funcion\")"
.LC_str3:
 .string "\"---- Programa 5 ----\")"
.LC_str2:
 .string "\"Luis\","
.LC_str1:
 .string "\"Ana\","
.LC_str0:
 .string "\"---- saludo ----\")"
.section .note.GNU-stack,"",@progbits
