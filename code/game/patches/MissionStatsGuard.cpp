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
  if (missionStats) {
    MissionStatEntry *missionStatEntries = (MissionStatEntry *)missionStats->data;
    if (missionStatEntries) {
      for (int i = 0; i < missionStats->length; ++i) {
        if (missionStatEntries[i].type == (u8)type) {
          return &missionStatEntries[i];
        }
      }
    }
  }
  
  return 0;
}

//------------- Detours -------------//
internal bool CDECL Detour_ModifyStatVariable(DishonoredPlayerPawn *playerPawn, int type, r32 amount)
{
  MissionStatEntry *stat = GetMissionStatVariable(playerPawn, type);
  if (!stat)
    return true;
  
  switch (type) {
    case MissionStat_AlarmsRung: {
      if (patchSettings.options.bShowAlarm) {
        if ((!stat->value) && (amount > 0.f)) {
          ShowLocationDiscovery(patchSettings.strings.msgAlarm, false);
        }
      }
    } break;
    
    case MissionStat_BodiesFound: {
      if (patchSettings.options.bShowBodyFound) {
        if ((!stat->value) && (amount > 0.f)) {
          ShowLocationDiscovery(patchSettings.strings.msgBodyFound, false);
        }
      }
    } break;
    
    case MissionStat_DetectedTimes: {
      if (patchSettings.options.bShowSpotted) {
        if ((!stat->value) && (amount > 0.0f)) {
          ShowLocationDiscovery(patchSettings.strings.msgSpotted, false);
        }
      }
    } break;
    
    case MissionStat_HostilesKilled:
    case MissionStat_CiviliansKilled: {
      if (patchSettings.options.bShowKilled) {
        MissionStatEntry *hostilesStat = GetMissionStatVariable(playerPawn, MissionStat_HostilesKilled);
        MissionStatEntry *civiliansStat = GetMissionStatVariable(playerPawn, MissionStat_CiviliansKilled);
        
        if ((!hostilesStat->value) && (!civiliansStat->value) && (amount > 0.f)) {
          ShowLocationDiscovery(patchSettings.strings.msgKilled, false);
        }
      }
    } break;
    
    case MissionStat_CoinsFound: {
      if (patchSettings.options.bShowCoinsCollected) {
        wchar_t buffer[255];
        swprintf(buffer, 255, patchSettings.strings.msgCoinsCollected, (stat->value + amount), stat->maxValue);
        ShowGameMessage(buffer, 2.f);
      }
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
  bool guard_active = patchSettings.options.bShowAlarm
    || patchSettings.options.bShowBodyFound
    || patchSettings.options.bShowSpotted
    || patchSettings.options.bShowKilled
    || patchSettings.options.bShowCoinsCollected;
  
  if (guard_active) {
    if (!WriteDetour(detour_modifystatvariable, ModifyStatVariable_Hook, 0)) {
      return false;
    }
  }
  
  return true;
}

internal void InitMissionStatsGuardConfig()
{
  patchSettings.strings.msgAlarm = ini_read_wstring(CONFIG_STRINGS, "msgAlarm", L"Alarm is set off");
  patchSettings.strings.msgBodyFound = ini_read_wstring(CONFIG_STRINGS, "msgBodyFound", L"Body was found");
  patchSettings.strings.msgSpotted = ini_read_wstring(CONFIG_STRINGS, "msgSpotted", L"You've been spotted");
  patchSettings.strings.msgKilled = ini_read_wstring(CONFIG_STRINGS, "msgKilled", L"You've killed somebody");
  patchSettings.strings.msgCoinsCollected = ini_read_wstring(CONFIG_STRINGS, "msgCoinsCollected", L"Collected %g of %d coins");
  
  patchSettings.options.bShowAlarm = ini_read_bool(CONFIG_OPTIONS, "bShowAlarm", false);
  patchSettings.options.bShowBodyFound = ini_read_bool(CONFIG_OPTIONS, "bShowBodyFound", false);
  patchSettings.options.bShowSpotted = ini_read_bool(CONFIG_OPTIONS, "bShowSpotted", true);
  patchSettings.options.bShowKilled = ini_read_bool(CONFIG_OPTIONS, "bShowKilled", true);
  patchSettings.options.bShowCoinsCollected = ini_read_bool(CONFIG_OPTIONS, "bShowCoinsCollected", true);
}

#endif
