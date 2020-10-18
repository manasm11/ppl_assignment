%include 'printInteger.asm'
section .data
newLine db 0ah
section .bss
section .text
global _start
_start:
mov ebp,esp
mov ax, 10
mov [ebp+0], ax
mov ax, 20
mov [ebp+2], ax
mov ax, 30
mov [ebp+4], ax
mov ax, 6
mov [ebp+20], ax
mov ax, 26
mov [ebp+6], ax
jmp L1
L0:
mov ax, 0
mov [ebp+6], ax
L1:
mov ax, [ebp+6]
call iprintLF


mov ebx,0
mov eax,1
int 80h