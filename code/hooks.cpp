/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

//IMPORTANT(adm244): SCRATCH VERSION JUST TO GET IT UP WORKING

#ifndef _HOOKS_CPP_
#define _HOOKS_CPP_

internal void *modify_stat_variable_hook_ret = (void *)0x00AA94A5;
internal void *showpausemenupost_hook_ret = (void *)0x00BCCEE1;

internal void NAKED ModifyStatVariable_Hook()
{
  __asm {
    push ebp
    mov ebp, esp
    
    mov eax, [ebp+0Ch]
    push eax
    mov eax, [ebp+08h]
    push eax
    push ecx
    call ModifyStatVariable
    pop ecx
    add esp, 08h
    pop ebp
    
    test eax, eax
    jnz continue
    ret 08h
    
  continue:
    push ebp
    mov ebp, esp
    push esi
    push edi
    
    jmp [modify_stat_variable_hook_ret]
  }
}

internal void NAKED ShowPauseMenuPost_Hook()
{
  __asm {
    pushad
    push edi
    call ShowPauseMenuPost
    pop edi
    popad
    
    mov eax, [ebp-03Ch]
    shr eax, 6
    
    jmp [showpausemenupost_hook_ret]
  }
}

#endif
