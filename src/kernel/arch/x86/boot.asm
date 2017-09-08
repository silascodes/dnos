
; Declare constants used for creating a multiboot header.
MBALIGN     equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
VBEINFO		equ  1<<2					; provice VBE info
FLAGS       equ  MBALIGN | MEMINFO | VBEINFO    ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot
 
; Declare a header as in the Multiboot Standard. We put this into a special
; section so we can force the header to be in the start of the final program.
; You don't need to understand all these details as it is just magic values that
; is documented in the multiboot standard. The bootloader will search for this
; magic sequence and recognize us as a multiboot kernel.
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 1
	dd 40
	dd 25
	dd 0

; Reserve 16kB for stack space
section .bootstrap_stack, nobits
align 4
stack_bottom:
times 16384 db 0
stack_top:

section .data

gdtr dw 0 ; For limit storage
     dd 0 ; For base storage

idtr dw 0
	 dd 0
 

section .text
align 4

; Kernel entry point
global _start
_start:
	cli

	; Setup stack pointer
	mov esp, stack_top
 
	; Call kmain with multiboot magic and info (hopefully)
	push ebx
	push eax
	extern _x86_init
	call _x86_init
 
	; Halt
	cli
.hang:
	hlt
	jmp .hang

global _enableInterrupts
_enableInterrupts:
	sti
	ret

global _disableInterrupts
_disableInterrupts:
	cli
	ret

global _doInt3
_doInt3:
	int3
	ret

global _x86_enablePaging
_x86_enablePaging:
	mov eax, [esp + 4]
	mov cr3, eax
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret

global _loadTR
_loadTR:
	xor eax, eax
	mov ax, 0x18	; TODO: hardcoded
	ltr ax
	ret

global _loadGDT
_loadGDT:
	mov eax, [esp + 4]
	mov [gdtr + 2], eax
	mov ax, [esp + 8]
	mov [gdtr], ax
	lgdt [gdtr]
	ret

global _storeGDTAddress
_storeGDTAddress:
	sgdt [gdtr]
	mov eax, [gdtr + 2]
	ret

global _storeGDTSize
_storeGDTSize:
	sgdt [gdtr]
	xor eax, eax
	mov ax, [gdtr]
	ret

global _loadIDT
_loadIDT:
	mov eax, [esp + 4]
	mov [idtr + 2], eax
	mov ax, [esp + 8]
	mov [idtr], ax
	lidt [idtr]
	ret

global _reloadSegs
_reloadSegs:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:_reloadCS
_reloadCS:
	ret

extern _x86_irq_handler

global _irq0
_irq0:
	pushad
;	xor eax, eax
;	push eax
;	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
;	pop eax
	popad
	iretd

global _irq1
_irq1:
	pushad
;	push 1
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	popad
	iretd

global _irq2
_irq2:
	pushad
	push 2
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	popad
	iretd

global _irq3
_irq3:
	pushad
	push 3
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	popad
	iretd

global _irq4
_irq4:
	pushad
	push 4
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	popad
	iretd

global _irq5
_irq5:
	pushad
	push 5
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	popad
	iretd

global _irq6
_irq6:
	pushad
	push 6
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	popad
	iretd

global _irq7
_irq7:
	pushad
	push 7
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	popad
	iretd

global _irq8
_irq8:
	pushad
	push 8
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	out 0xA0, al
	popad
	iretd

global _irq9
_irq9:
	pushad
	push 9
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	out 0xA0, al
	popad
	iretd

global _irq10
_irq10:
	pushad
	push 10
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	out 0xA0, al
	popad
	iretd

global _irq11
_irq11:
	pushad
	push 11
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	out 0xA0, al
	popad
	iretd

global _irq12
_irq12:
	pushad
	push 12
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	out 0xA0, al
	popad
	iretd

global _irq13
_irq13:
	pushad
	push 13
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	out 0xA0, al
	popad
	iretd

global _irq14
_irq14:
	pushad
	push 14
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	out 0xA0, al
	popad
	iretd

global _irq15
_irq15:
	pushad
	push 15
	call _x86_irq_handler
	mov al, 0x20
	out 0x20, al
	out 0xA0, al
	popad
	iretd

global _isr0
_isr0:
	cli
	mov ebx, 0x00000F30
	mov [0xb8000], bx
	hlt
	iretd

global _isr1
_isr1:
	cli
	mov ebx, 0x00000F31
	mov [0xb8000], bx
	hlt
	iretd

global _isr2
_isr2:
	cli
	mov ebx, 0x00000F32
	mov [0xb8000], bx
	hlt
	iretd

global _isr3
_isr3:
	cli
	mov ebx, 0x00000F33
	mov [0xb8000], bx
	hlt
	iretd

global _isr4
_isr4:
	cli
	mov ebx, 0x00000F34
	mov [0xb8000], bx
	hlt
	iretd

global _isr5
_isr5:
	cli
	mov ebx, 0x00000F35
	mov [0xb8000], bx
	hlt
	iretd

global _isr6
_isr6:
	cli
	mov ebx, 0x00000F36
	mov [0xb8000], bx
	hlt
	iretd

global _isr7
_isr7:
	cli
	mov ebx, 0x00000F37
	mov [0xb8000], bx
	hlt
	iretd

global _isr8
_isr8:
	cli
	mov ebx, 0x00000F38
	mov [0xb8000], bx
	hlt
	iretd

global _isr9
_isr9:
	cli
	mov ebx, 0x00000F39
	mov [0xb8000], bx
	hlt
	iretd

global _isr10
_isr10:
	cli
	mov ebx, 0x00000F41
	mov [0xb8000], bx
	hlt
	iretd

global _isr11
_isr11:
	cli
	mov ebx, 0x00000F42
	mov [0xb8000], bx
	hlt
	iretd

global _isr12
_isr12:
	cli
	mov ebx, 0x00000F43
	mov [0xb8000], bx
	hlt
	iretd

global _isr13
_isr13:
	cli
	mov ebx, 0x00000F44
	mov [0xb8000], bx
	hlt
	iretd

global _isr14
_isr14:
	cli
	mov ebx, 0x00000F45
	mov [0xb8000], bx
	hlt
	iretd

; reserved
global _isr15
_isr15:
	cli
	mov ebx, 0x00000F46
	mov [0xb8000], bx
	hlt
	iretd

global _isr16
_isr16:
	cli
	mov ebx, 0x00000F47
	mov [0xb8000], bx
	hlt
	iretd

global _isr17
_isr17:
	cli
	mov ebx, 0x00000F48
	mov [0xb8000], bx
	hlt
	iretd

global _isr18
_isr18:
	cli
	mov ebx, 0x00000F49
	mov [0xb8000], bx
	hlt
	iretd

global _isr19
_isr19:
	cli
	mov ebx, 0x00000F4A
	mov [0xb8000], bx
	hlt
	iretd

global _isr20
_isr20:
	cli
	mov ebx, 0x00000F4B
	mov [0xb8000], bx
	hlt
	iretd

; 21-31 reserved
