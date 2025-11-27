.data
print_fmt: 
.string "%ld \n"
ORIGIN:
.quad 0
.quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $0, %rax
 jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
