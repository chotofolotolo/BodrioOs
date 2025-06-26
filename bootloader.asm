; =========================================================================
; bootloader.asm
; Carga un kernel en la dirección 0x10000 y cambia a Modo Protegido.
; =========================================================================
BITS 16
ORG 0x7C00

start:
    mov bp, 0x9000
    mov sp, bp

    mov [boot_drive], dl

    mov si, msg_loading
    call print_string

    ; --- Cargar kernel desde el disco a la memoria 0x10000 ---
    mov ax, 0x1000
    mov es, ax
    mov bx, 0x0000   ; es:bx = 0x1000:0x0000 = 0x10000 (64KB)

    mov ah, 0x02
    mov al, 32
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm

print_string:
    mov ah, 0x0E
.loop:
    lodsb
    or al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

disk_error:
    mov si, msg_disk_error
    call print_string
    hlt

BITS 32
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    jmp CODE_SEG:0x0000

BITS 16
boot_drive      db 0
msg_loading     db "Cargando el kernel...", 13, 10, 0
msg_disk_error  db "Error al leer el disco.", 13, 10, 0

gdt_start:
gdt_null:
    dd 0x0, 0x0
gdt_code:
    dw 0xFFFF, 0x0000, 0x009A, 0x00CF
gdt_data:
    dw 0xFFFF, 0x0000, 0x0092, 0x00CF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; --- Constantes ---
KERNEL_OFFSET equ 0x10000   ; <-- ACTUALIZADO
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

times 510 - ($ - $$) db 0
dw 0xAA55
