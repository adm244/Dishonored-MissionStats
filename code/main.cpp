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

/*
  Get Dishonored mission stats:
    1) Get an object at [0x1452DE8]
    2) Get an array of mission stats at [0xB7C]
  
  MissionStatEntry:
    [0x00] Type (1 byte)
    [0x04] Current Value (4 bytes, float)
    [0x08] Maximum Value (4 bytes, int or float)
  
  MissionStatTypes:
    AlarmsRung = 0x03,
    BodiesFound = 0x04,
    DetectedTimes = 0x0A, // ghost
    OverallChaos = 0x1B,
    HostilesKilled = 0x1C,
    CiviliansKilled = 0x1E,
    CoinsFound = 0x21,
    RunesFound = 0x22,
    BoneCharmsFound = 0x23,
    OutsiderShrinesFound = 0x24,
    SokolovPaintingsFound = 0x25,
*/

/*
  DONE:
    - Locate where mission stats are stored in memory
    - Detect if ghost playthrough has been ruinned
    - Show location discovery message with a custom text
    
  TODO:
    - Draw all mission stats on a screen
    - Draw if specific mission stat has changed
    - Ability to lock a specific mission stat value
*/

#define PSAPI_VERSION 1
#include <windows.h>
#include <psapi.h>
#include <cstdio>

#include "types.h"
#include "detours.cpp"

#define HOOK_CALL __cdecl
#define HOOK_FUNC __declspec(naked)

#define assert_size(obj, size) static_assert(sizeof(obj) == size, "Size of " #obj " should be " #size)

enum MissionStatTypes {
  MissionStat_DetectedTimes = 0x0A,
};

#pragma pack(4)

struct Array {
  void *data;
  u32 length;
  u32 capacity;
};
assert_size(Array, 0xC);

struct UString {
  wchar_t *text;
  u32 length;
  u32 capacity;
};
assert_size(UString, 0xC);

struct MissionStatEntry {
  u8 type;
  r32 value;
  union {
    u32 maxValueInt;
    r32 maxValueFloat;
  };
};
assert_size(MissionStatEntry, 0xC);

struct Unk01 {
  u8 unk0[0xB7C-0x0];
  Array missionStats; // 0xB7C
  // ...
};

#pragma pack(pop)

typedef void * (__stdcall _GetUnk10)();
typedef void (__thiscall *_Unk10_ShowLocationDiscovery)(void *unk10, UString *text, int playSound);

internal _GetUnk10 *GetUnk10 = (_GetUnk10 *)0x00BBF760;
internal _Unk10_ShowLocationDiscovery Unk10_ShowLocationDiscovery = (_Unk10_ShowLocationDiscovery)0x00B94D60;

internal void ShowLocationDiscovery(wchar_t *text, bool playSound)
{
  void *unk10 = GetUnk10();
  
  UString str = {0};
  str.text = text;
  str.length = wcslen(text);
  str.capacity = str.length;
  
  Unk10_ShowLocationDiscovery(unk10, &str, playSound ? 1 : 0);
}

//INVESTIGATE(adm244): In release mode when msvc 2010 inlines this function
// the return value overwrites unk parameter on a stack with itself,
// but it happens only if this function returns float, all is fine when integer is returned
// Possible compiler bug or am I missing something regarding calling-conventions?
internal r32 __declspec(noinline) GetMissionStatVariable(Unk01 *unk, int type)
{
  Array *missionStats = &unk->missionStats;
  MissionStatEntry *missionStatEntries = (MissionStatEntry *)missionStats->data;
  
  for (int i = 0; i < missionStats->length; ++i) {
    if (missionStatEntries[i].type == (u8)type) {
      return missionStatEntries[i].value;
    }
  }
  
  return -1.0;
}

internal bool HOOK_CALL ModifyStatVariable(Unk01 *unk, int type, r32 amount)
{
  r32 currentValue = GetMissionStatVariable(unk, type);
  
  switch (type) {
    case MissionStat_DetectedTimes: {
      if ((currentValue > -1.0) && (currentValue < 1.0) && (amount > 0.0)) {
        ShowLocationDiscovery(L"You've been spotted", false);
      }
    } break;
    
    default:
      break;
  }
  
  return true;
}

internal void *modify_stat_variable_hook_ret = (void *)0x00AA94A5;

internal void HOOK_FUNC ModifyStatVariable_Hook()
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

internal bool Initialize()
{
  if (!WriteDetour((void *)0x00AA94A0, ModifyStatVariable_Hook, 0)) {
    return false;
  }
  
  return true;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  if (reason == DLL_PROCESS_ATTACH) {
    return Initialize() ? TRUE : FALSE;
  }
  
  return TRUE;
}
