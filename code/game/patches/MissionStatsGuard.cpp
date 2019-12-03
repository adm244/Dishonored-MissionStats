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

#ifndef _PATCH_MISSIONSTATS_GUARD_CPP_
#define _PATCH_MISSIONSTATS_GUARD_CPP_

//------------- Static pointers -------------//
STATIC_POINTER(void, detour_modifystatvariable);
STATIC_POINTER(void, hook_modifystatvariable_ret);

//------------- Functions -------------//
internal MissionStatEntry * GetMissionStatVariable(DishonoredPlayerPawn *playerPawn, int type)
{
  UArray *missionStats = &playerPawn->missionStats;
  MissionStatEntry *missionStatEntries = (MissionStatEntry *)missionStats->data;
  
  for (int i = 0; i < missionStats->length; ++i) {
    if (missionStatEntries[i].type == (u8)type) {
      return &missionStatEntries[i];
    }
  }
  
  OutputDebugStringA("GetMissionStatVariable: Couldn't find a specified mission stat.");
  return 0;
}

//------------- Detours -------------//
internal bool CDECL Detour_ModifyStatVariable(DishonoredPlayerPawn *playerPawn, int type, r32 amount)
{
  MissionStatEntry *stat = GetMissionStatVariable(playerPawn, type);
  
  switch (type) {
    case MissionStat_DetectedTimes: {
      if ((!stat->value) && (amount > 0.0f)) {
        ShowLocationDiscovery(L"You've been spotted", false);
      }
    } break;
    
    case MissionStat_HostilesKilled:
    case MissionStat_CiviliansKilled: {
      MissionStatEntry *hostilesStat = GetMissionStatVariable(playerPawn, MissionStat_HostilesKilled);
      MissionStatEntry *civiliansStat = GetMissionStatVariable(playerPawn, MissionStat_CiviliansKilled);
      
      if ((!hostilesStat->value) && (!civiliansStat->value) && (amount > 0.f)) {
        ShowLocationDiscovery(L"You've killed somebody", false);
      }
    } break;
    
    case MissionStat_CoinsFound: {
      wchar_t buffer[255];
      swprintf(buffer, 255, L"Collected %g of %d coins", stat->value + amount, stat->maxValue);
      
      ShowGameMessage(buffer, 2.f);
      
      /*UString *title = &GetUString(L"Coins collected");
      UString *text = &GetUString(buffer);
      NoteContents *note = &GetNoteContents(title, text);
      
      //TODO(adm244): figure out how to preload UI_NOTES gfx
      void *unk20 = GetUnk20();
      Unk20_ShowNote(unk20, note, 0);*/
    } break;
    
    default:
      break;
  }
  
  return true;
}

//------------- Hooks -------------//
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
    call Detour_ModifyStatVariable
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
    
    jmp [hook_modifystatvariable_ret]
  }
}

//------------- Init -------------//
internal bool InitMissionStatsGuard()
{
  if (!WriteDetour(detour_modifystatvariable, ModifyStatVariable_Hook, 0)) {
    return false;
  }
  
  return true;
}

#endif
