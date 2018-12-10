;сборка
;nasm -f elf32 just_read.asm
;gcc -m32 -o just_read just_read.o -fno-stack-protector
;выключение aslr
;echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

section .data
welcome_msg:                                    
      db    "Type something:", 0ah
reply_msg_1:
      db    "You typed:", 0ah
reply_msg_2:
      db    "You didn't type anything :(", 0ah
secret_msg:
      db    "So hax much wow", 0ah
stack_sz:
      dd    20                ;размер стека
max_msg_sz:
      dd    1024              ;максимальный размер считываемого сообщения

section .text
      global main
main:                         ;точка входа
      call  mirror_msg        ;вызов функции ввода данных пользователя
      mov   ebx, 0
      mov   eax, 1
      int   80h               ;sys_exit(0)

secret_func:                  ;функция, которая не вызывается в программе
                              ;используется для демонстрации stack overflow
      mov   edx, 16
      mov   ecx, secret_msg
      mov   ebx, 1
      mov   eax, 4
      int   80h               ;sys_write(stdout, secret_msg, 16)

      mov   ebx, 0
      mov   eax, 1
      int   80h               ;sys_exit(0)

mirror_msg:                   ;функция ввода данных пользователя
      push  ebp               ;сохранение адреса предыдущего фрейма на стек
      mov   ebp, esp          ;сохранение адреса стека
      sub   esp, [stack_sz]   ;выделение места на стеке

      mov   edx, 16
      mov   ecx, welcome_msg
      mov   ebx, 1
      mov   eax, 4
      int   80h               ;sys_write(stdout, welcome_msg, 16)

      mov   edx, max_msg_sz
      mov   ecx, esp
      mov   ebx, 0
      mov   eax, 3
      int   80h               ;sys_read(stdin, addr, max_msg_sz)

      cmp   eax, 0            ;если сообщение считалось - прыжок на .reply_1
      jg    .Lreply_1

.Lreply_2:
      mov   edx, 28     
      mov   ecx, reply_msg_2
      mov   ebx, 1
      mov   eax, 4
      int   80h               ;sys_write(stdout, reply_msg_2, 28)
      jmp   .Lfooter

.Lreply_1:
      mov   ebx, eax          ;сохранение длины введенного сообщения из eax в esi
      mov   edx, 0
      lea   esi, [esp + eax]

.Lloop_1:
      mov   byte cl, [esp + ebx - 1]

      mov   byte [esi + edx], cl

      dec   ebx
      inc   edx
      cmp   ebx, 0
      jge   .Lloop_1

      mov   esi, eax

      mov   edx, 11
      mov   ecx, reply_msg_1
      mov   ebx, 1
      mov   eax, 4
      int   80h               ;sys_write(stdout, reply_msg_1, 11)

      mov   edx, esi
      lea   ecx, [esp+esi]
      mov   ebx, 1
      mov   eax, 4
      int   80h               ;sys_write(stdout, msg, msg_len)

.Lfooter:
      mov   esp, ebp          ;восстановление адреса cтека
      pop   ebp               ;восстановление адреса предыдущего фрейма
      ret

