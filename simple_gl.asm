; File: simple_gl.asm
; Assemble with: nasm -f elf32 simple_gl.asm -o simple_gl.o
; Link with: gcc -m32 simple_gl.o -lGL -lX11 -o simple_gl

section .data
display db 0
screen db 0
visual_info db 0
glc db 0
title db 'Simple OpenGL', 0

; Floating-point values
red dd 1.0
green dd 0.0
blue dd 0.0
alpha dd 1.0

section .text
global main

extern glClearColor
extern glClear
extern glXChooseVisual
extern glXCreateContext
extern glXMakeCurrent
extern XOpenDisplay
extern XCreateWindow
extern XMapWindow
extern XStoreName
extern _exit

main:
    ; Open X display
    push dword 0
    call XOpenDisplay
    add esp, 4
    mov [display], eax

    ; Choose visual info
    push dword 0x22  ; GLX_RGBA
    push dword [display]
    call glXChooseVisual
    add esp, 8
    mov [visual_info], eax

    ; Create an OpenGL context
    push dword [visual_info]
    push dword [display]
    call glXCreateContext
    add esp, 8
    mov [glc], eax

    ; Create a simple X window
    push dword 0
    push dword 0x000000FF  ; Black
    push dword 0xFFFFFF00  ; White
    push dword 0  ; border_width
    push dword 100  ; height
    push dword 100  ; width
    push dword 0  ; y
    push dword 0  ; x
    push dword 0  ; parent
    push dword [visual_info]
    push dword [display]
    call XCreateWindow
    add esp, 44
    mov [screen], eax

    ; Map the window
    push dword [screen]
    push dword [display]
    call XMapWindow
    add esp, 8

    ; Set window title
    push dword title
    push dword [screen]
    push dword [display]
    call XStoreName
    add esp, 12

    ; Make the OpenGL context current
    push dword [screen]
    push dword [glc]
    push dword [display]
    call glXMakeCurrent
    add esp, 12

    ; Clear the screen with a color
    fld dword [alpha]
    fstp dword [esp]
    fld dword [blue]
    fstp dword [esp + 4]
    fld dword [green]
    fstp dword [esp + 8]
    fld dword [red]
    fstp dword [esp + 12]
    call glClearColor
    add esp, 16

    ; Clear the buffer
    push dword 0x00004000  ; GL_COLOR_BUFFER_BIT
    call glClear
    add esp, 4

    ; Exit
    push dword 0  ; exit status
    call _exit

