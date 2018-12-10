hWnd equ 08h
Msg equ 0ch
wParam equ 10h
lParam equ 14h


offs_oldWindowProc equ 21083h
offs_CallWindowProc equ 21087h
offs_SendMessageW equ 2ee4bh
offs_SendMessageA equ 0a7d9h
offs_SetTimer equ 24ah
offs_KillTimer equ 0fbeh

offs_flag equ 2108eh
offs_counter_secret equ 2108fh
offs_counter_frames equ 2108dh
offs_cur_frame equ 2108bh
offs_secret equ 50e81h
offs_frames equ 0d249h

sz_frame equ 3855
num_frames equ 72
interval equ 500
timer_id equ 888



BITS 32
    mov edi, edi
    push ebp
    mov ebp, esp
    push esi
    push edi

    call _here
_here:
    nop
    jmp _main

_default_func:
    mov eax, [esp]
    push dword [ebp+lParam]
    push dword [ebp+wParam]
    push dword [ebp+Msg]
    push dword [ebp+hWnd]
    push dword [eax-offs_oldWindowProc]
    call [eax-offs_CallWindowProc]

_retn:
    pop edi
    pop edi
    pop esi
    pop ebp
    retn 10h

_main:
    mov ebx, [ebp+Msg]
_case_char:
    cmp ebx, 102h
    jnz _case_timer
    
    mov eax, [esp]
    movzx eax, byte [eax-offs_flag]
    test eax, eax
    jz _not_running

_running:
    mov ebx, [ebp+wParam]
    cmp ebx, 61h
    jnz _retn
    jmp _turn_off

_not_running:
    mov eax, [esp]
    movzx eax, byte [eax-offs_counter_secret]

    mov ecx, [esp]
    lea ecx, [ecx+offs_secret]
    movzx ecx, byte [ecx + eax]

    mov ebx, [ebp+wParam]
    cmp ebx, ecx
    jnz _reset_counter

    mov eax, [esp]
    movzx eax, byte [eax-offs_counter_secret]
    inc eax
    cmp eax, 100h
    jz _turn_on

_not_full:
    mov ecx, [esp]
    lea ecx, [ecx+offs_secret]
    movzx ecx, byte [ecx + eax]
    test ecx, ecx
    jz _turn_on

    mov ecx, [esp]
    add byte [ecx-offs_counter_secret], 1
    jmp _default_func

_turn_on:
    mov ecx, [esp]
    mov byte [ecx-offs_flag], 1
    mov byte [ecx-offs_counter_secret], 0

    lea ebx, [ecx+offs_frames]
    mov dword [ecx-offs_cur_frame], ebx
    
    mov byte [ecx-offs_counter_frames], 0

    mov eax, [esp]
    mov eax, [eax-offs_SendMessageW]
    lea eax, [eax-offs_SetTimer]

    push 0
    push interval
    push timer_id
    push dword [ebp+hWnd]
    call eax
    
    jmp _retn

_turn_off:
    mov ecx, [esp]
    mov byte [ecx-offs_flag], 0

    mov eax, [esp]
    mov eax, [eax-offs_SendMessageW]
    lea eax, [eax-offs_SendMessageA]

    mov ecx, [esp]
    lea ecx, [ecx+offs_secret]
    lea ecx, [ecx+0feh]
    
    push ecx
    push 0
    push 0ch
    push dword [ebp+hWnd]
    call eax

    mov eax, [esp]
    mov eax, [eax-offs_SendMessageW]
    lea eax, [eax+offs_KillTimer]

    push timer_id
    push dword [ebp+hWnd]
    call eax
    jmp _retn

_reset_counter:
    mov ecx, [esp]
    mov byte [ecx-offs_counter_secret], 0
    jmp _default_func

_case_timer:
    cmp ebx, 113h
    jnz _default_func
    mov eax, [esp]
    movzx eax, byte [eax-offs_flag]
    test eax, eax
    jz _retn

    mov edi, [esp]
    mov edi, [edi-offs_SendMessageW]
    lea edi, [edi-offs_SendMessageA]

    mov ecx, [esp]
    mov ecx, [ecx-offs_cur_frame]
    
    push ecx
    push 0
    push 0ch
    push dword [ebp+hWnd]
    call edi

    mov eax, [esp]
    movzx eax, byte [eax-offs_counter_frames]
    inc eax
    cmp eax, num_frames
    jz _reset_loop
    
    mov ecx, [esp]
    add byte [ecx-offs_counter_frames], 1
    add dword [ecx-offs_cur_frame], sz_frame

    mov eax, 0
    jmp _retn

_reset_loop:
    mov ecx, [esp]
    mov byte [ecx-offs_counter_frames], 0

    lea ebx, [ecx+offs_frames]
    mov dword [ecx-offs_cur_frame], ebx
    
    mov eax, 0
    jmp _retn

    
    
    
    

