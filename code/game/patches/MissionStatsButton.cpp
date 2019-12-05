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

#ifndef _PATCH_MISSIONSTATS_BUTTON_CPP_
#define _PATCH_MISSIONSTATS_BUTTON_CPP_

//------------- Enumerables -------------//
enum ButtonLockState {
  LockState_None,
  LockState_SaveGame,
  LockState_LoadGame,
  LockState_Help,
  LockState_Stats,
};

struct ButtonsData {
  char *name;
  char *callback;
  ButtonLockState state;
};

enum DisTweaks_MissionStats_Flags {
  Patch_ReplaceBPressed = 0x80000000,
  Patch_SkipMustDoFlag = 0x40000000,
};

//------------- Static pointers -------------//
STATIC_POINTER(void, detour_showpausemenu);
STATIC_POINTER(void, detour_setmissionstats);
STATIC_POINTER(void, detour_shouldskipmustdoflag);
STATIC_POINTER(void, detour_distweaks_missionstats_ctor);
STATIC_POINTER(void, detour_distweaks_missionstats_dtor);

STATIC_POINTER(void, hook_showpausemenu_ret);
STATIC_POINTER(void, hook_setmissionstats_ret);
STATIC_POINTER(void, hook_shouldskipmustdoflag_ret);
STATIC_POINTER(void, hook_distweaks_missionstats_ctor_ret);
STATIC_POINTER(void, hook_distweaks_missionstats_dtor_ret);

//------------- Static variables -------------//
//FIX(adm244): proper array implementation
internal DisTweaks_MissionStats *g_MissionStatsTweaks[30];
internal uint g_MissionStatsTweaksCount = 0;

internal ButtonsData data[] = {
  {"_root.texts.t_ResumeGame", "OnResumeClicked", LockState_None},
  {"_root.texts.t_SaveGame", "OnSaveGameClicked", LockState_SaveGame},
  {"_root.texts.t_LoadGamePauseMenu", "OnLoadGameClicked", LockState_LoadGame},
  {"_root.texts.t_Options", "OnOptionsClicked", LockState_None},
  {"_root.texts.t_MissionStats", "OnMissionStatsClicked", LockState_Stats},
  {"_root.texts.t_Help", "OnTutorialsClicked", LockState_Help},
  {"_root.texts.t_BackToMainMenu", "OnQuitGameClicked", LockState_None},
  {"_root.texts.t_BackToWindows", "OnBackToWindowsClicked", LockState_None},
};

//------------- Functions -------------//
internal DisTweaks_MissionStats * GetMissionStatsTweaks(i32 missionNumber)
{
  for (int i = 0; i < g_MissionStatsTweaksCount; ++i)
  {
    DisTweaks_MissionStats *tweaks = g_MissionStatsTweaks[i];
    if (tweaks->missionNumber == missionNumber)
      return tweaks;
  }
  
  return 0;
}

internal u16 GetSpecialActionsFlags(DisTweaks_MissionStats *tweaks)
{
  u16 flags = 0;
  
  for (int i = 0; i < tweaks->specialActions.length; ++i) {
    DisSpecialAction *specialActions = (DisSpecialAction *)tweaks->specialActions.data;
    DisStoryFlagSet *flagSet = specialActions[i].storyFlagSet;
    DisSpecialActionFlagSet *specialActionFlagSet = &specialActions[i].flagSet;
    
    if (!flagSet)
      continue;
    
    if (!specialActionFlagSet)
      continue;
    
    if (!DisStoryFlagSet_GetStoryFlagSet(flagSet, specialActionFlagSet))
      continue;
    
    if (!DishonoredPlayerPawn_HasCompleteAction(*playerPawn, flagSet, specialActionFlagSet))
      continue;
    
    flags |= (1 << i);
  }
  
  return flags;
}

internal bool GetStatsValuesBuffer(DisTweaks_MissionStats *tweaks, r32 *buffer, int size)
{
  if (size < tweaks->statsValues.length)
    return false;
  
  for (int i = 0; i < tweaks->statsValues.length; ++i) {
    StatValueTemplate *templates = (StatValueTemplate *)tweaks->statsValues.data;
    float currentValue = 0.0f;
    int maxValue = 0;
    
    DishonoredPlayerPawn_GetStatsValue(*playerPawn, templates[i].type1, &currentValue, &maxValue);
    
    if (templates[i].checkType1) {
      float currentValue2 = 0.0f;
      int maxValue2 = 0;
      
      DishonoredPlayerPawn_GetStatsValue(*playerPawn, templates[i].type2, &currentValue2, &maxValue2);
      currentValue += currentValue2;
    }
    
    if (templates[i].type1 == MissionStat_OverallChaos) {
      DisDarknessManager *darknessManager = (*playerPawn)->darknessManager;
      int index = DisDarknessManager_GetChaosTresholdIndex(darknessManager);
      //TODO(adm244): maybe get these from Game.ini
      // m_MissionChaosThreshold under DishonoredGame.DishonoredGameInfo
      // so, it's probably stored in DishonoredGameInfo
      int missionNumber = (tweaks->missionNumber + 1);
      
      //NOTE(adm244): last mission of base game displays chaos level incorrectly
      // I guess let's do the same, why not...
      //NOTE(adm244): no need to check for dlc, none has more than 4 missions
      if (missionNumber == 9)
        missionNumber = 8;
      
      if (index < missionNumber)
        currentValue = 0.0f; // low chaos
      else
        currentValue = 1.0f; // high chaos
    }
    
    buffer[i] = currentValue;
  }
  
  return true;
}

internal bool SetMissionStats(DisTweaks_MissionStats *tweaks)
{
  if (!UArray_InRange(&tweaks->specialActions, 16))
    return false;
  
  if (!UArray_InRange(&tweaks->statsValues, 16))
    return false;
  
  r32 statsValuesBuffer[16];
  u16 specialActionsFlags = GetSpecialActionsFlags(tweaks);
  if (!GetStatsValuesBuffer(tweaks, statsValuesBuffer, 16))
    return false;
  
  i32 missionIndex = GetMissionIndex(tweaks->missionNumber);
  
  ArkProfileSettings *profileSettings = DishonoredPlayerController_GetProfileSettings(*playerController);
  ArkProfileSettings_SetMissionStats(profileSettings, missionIndex, tweaks->dlcNumber, specialActionsFlags, statsValuesBuffer, tweaks->statsValues.length);
  
  return true;
}

//------------- Function handlers -------------//
struct SetPauseMenuFunc : FunctionHandler {
  void Call(Params *params)
  {
    GFxValue saveState_obj = {0};
    GFxValue_Invoke(params->thisPtr, &saveState_obj, "GetSaveState", 0, 0);
    
    GFxValue _isSaveGame_field = {0};
    GFxValue _isLoadGame_field = {0};
    GFxValue _showHelpButton_field = {0};
    
    GFxValue_GetMember(&saveState_obj, "_isSaveGame", &_isSaveGame_field);
    GFxValue_GetMember(&saveState_obj, "_isLoadGame", &_isLoadGame_field);
    GFxValue_GetMember(params->thisPtr, "_showHelpButton", &_showHelpButton_field);
    
    GFxValue buttonsASArray = {0};
    GFxMovie_CreateArray(params->movie, &buttonsASArray);
    
    for (int i = 0; i < arraysize(data); ++i) {
      GFxValue button = {0};
      GFxMovie_CreateObject(params->movie, &button, 0, 0, 0);
      
      GFxValue text = {0};
      GFxValue callback = {0};
      GFxValue lockState = {0};

      GFxMovie_GetVariable(params->movie, &text, data[i].name);
      GFxValue_GetMember(params->thisPtr, data[i].callback, &callback);
      
      switch (data[i].state) {
        case LockState_SaveGame: {
          GFxValue_SetBoolean(&lockState, !_isSaveGame_field.boolean);
        } break;
        
        case LockState_LoadGame: {
          GFxValue_SetBoolean(&lockState, !_isLoadGame_field.boolean);
        } break;
        
        case LockState_Help: {
          GFxValue_SetBoolean(&lockState, !_showHelpButton_field.boolean);
        } break;
        
        case LockState_Stats: {
          GFxValue_SetBoolean(&lockState, GetMissionNumber() < 0);
        } break;
        
        default: {
          GFxValue_SetBoolean(&lockState, false);
        } break;
      }
      
      GFxValue_SetMember(&button, "txt", &text);
      GFxValue_SetMember(&button, "callback", &callback);
      GFxValue_SetMember(&button, "lockState", &lockState);
      
      GFxValue_PushBack(&buttonsASArray, &button);
    }
    
    GFxValue _menu_mc_field = {0};
    GFxValue_GetMember(params->thisPtr, "_menu_mc", &_menu_mc_field);
    
    GFxValue delayedOpeningAnimation_field = {0};
    GFxValue_SetBoolean(&delayedOpeningAnimation_field, true);
    
    GFxValue_SetMember(&_menu_mc_field, "_bDelayedOpeningAnimation", &delayedOpeningAnimation_field);
    GFxValue_Invoke(&_menu_mc_field, 0, "SetMenu", &buttonsASArray, 1);
  }
} SetPauseMenu;

struct OnMissionStatsClickedFunc : FunctionHandler {
  virtual void Call(Params *params)
  {
    DisGlobalUIManager *globalUIManager = GetGlobalUIManager();
    DisGFxMoviePlayerMissionStats *missionStats = globalUIManager->missionStats;
    
    i32 missionNumber = GetMissionNumber();
    DisTweaks_MissionStats *tweaks = GetMissionStatsTweaks(missionNumber);
    if (!tweaks)
      return;
    
    if (SetMissionStats(tweaks)) {
      //NOTE(adm244): should be save to use
      SetFlag(&missionStats->base.flags, Patch_ReplaceBPressed | Patch_SkipMustDoFlag);
      DisGFxMoviePlayerMissionStats_Show(missionStats, tweaks, 1);
    }
  }
} OnMissionStatsClicked;

struct BPressedFunc : FunctionHandler {
  virtual void Call(Params *params)
  {
    DisGlobalUIManager *uiManager = GetGlobalUIManager();
    DisGFxMoviePlayerPauseMenu *pauseMenu = uiManager->pauseMenu;
    GFxMovie *gfxPauseMenu = pauseMenu->base.disMovie->movie;
    
    GFxValue pauseMenu_mc = {0};
    if (GFxMovie_GetVariable(gfxPauseMenu, &pauseMenu_mc, "_root.pauseMenu_mc"))
      GFxValue_Invoke(&pauseMenu_mc, 0, "Open", 0, 0);
    
    GFxValue soundHandler_field = {0};
    if (GFxMovie_GetVariable(params->movie, &soundHandler_field, "SoundHandler")) {
      GFxValue soundName = {0};
      GFxValue_SetStringW(&soundName, L"Back");
      GFxValue_Invoke(&soundHandler_field, 0, "PlaySound", &soundName, 1);
    }
    
    GFxValue_Invoke(params->thisPtr, 0, "Close", 0, 0);
  }
} BPressed;

//------------- Detours -------------//
internal void CDECL Detour_ShowPauseMenu(DisGFxMoviePlayerPauseMenu *pauseMenu)
{
  if (pauseMenu->mode == PauseMenuMode_GameOver)
    return;
  
  GFxValue pauseMenu_mc = {0};
  GFxValue SetPauseMenu_func = {0};
  GFxValue OnMissionStatsClicked_func = {0};
  GFxMovie *gfxPauseMenu = pauseMenu->base.disMovie->movie;
  
  if (!GFxMovie_GetVariable(gfxPauseMenu, &pauseMenu_mc, "_root.pauseMenu_mc"))
    return;
  
  GFxValue t_MissionStats_field = {0};
  //TODO(adm244): load button text from a config file
  GFxValue_SetStringW(&t_MissionStats_field, L"STATISTICS");
  GFxMovie_SetVariable(gfxPauseMenu, "_root.texts.t_MissionStats", &t_MissionStats_field, SV_Normal);

  GFxMovie_CreateFunction(gfxPauseMenu, &OnMissionStatsClicked_func, &OnMissionStatsClicked, 0);
  if (!GFxValue_SetMember(&pauseMenu_mc, "OnMissionStatsClicked", &OnMissionStatsClicked_func))
    return;
  
  GFxMovie_CreateFunction(gfxPauseMenu, &SetPauseMenu_func, &SetPauseMenu, 0);
  if (!GFxValue_SetMember(&pauseMenu_mc, "SetPauseMenu", &SetPauseMenu_func))
    return;
}

internal void CDECL Detour_SetMissionStats(DisGFxMoviePlayerMissionStats *missionStats)
{
  if (!IsFlagSet(missionStats->base.flags, Patch_ReplaceBPressed))
    return;
  ClearFlag(&missionStats->base.flags, Patch_ReplaceBPressed);
  
  GFxValue missionStats_mc = {0};
  GFxValue BPressed_func = {0};
  GFxMovie *gfxMissionStats = missionStats->base.disMovie->movie;
  
  if (!GFxMovie_GetVariable(gfxMissionStats, &missionStats_mc, "_root.missionStats_mc"))
    return;
  
  GFxMovie_CreateFunction(gfxMissionStats, &BPressed_func, &BPressed, 0);
  GFxValue_SetMember(&missionStats_mc, "BPressed", &BPressed_func);
}

internal bool CDECL Detour_ShouldSkipMustDoFlag(DisGFxMoviePlayerMissionStats *missionStats, DisSpecialAction *specialAction, int index)
{
  //NOTE(adm244): don't check "special action must be complete" flag for last base game mission
  // fixes "Rescued Emily" showed mid-mission
  if (IsFlagSet(missionStats->base.flags, Patch_SkipMustDoFlag)) {
    int last_index = (missionStats->tweaks->specialActions.length - 1);
    
    if (index == last_index)
      ClearFlag(&missionStats->base.flags, Patch_SkipMustDoFlag);
    
    if (missionStats->tweaks->missionNumber == 8)
      return true;
  }
  
  return false;
}

internal void CDECL Detour_DisTweaks_MissionStats_Constructor(DisTweaks_MissionStats *missionStats)
{
  g_MissionStatsTweaks[g_MissionStatsTweaksCount] = missionStats;
  ++g_MissionStatsTweaksCount;
}

internal void CDECL Detour_DisTweaks_MissionStats_Destructor(DisTweaks_MissionStats *missionStats)
{
  if (g_MissionStatsTweaksCount != 0)
    g_MissionStatsTweaksCount = 0;
}

//------------- Hooks -------------//
internal void NAKED ShowPauseMenu_Hook()
{
  __asm {
    push ebp
    mov ebp, esp
    push 0FFFFFFFFh
    
    push ecx
    call Detour_ShowPauseMenu
    pop ecx
    
    jmp [hook_showpausemenu_ret]
  }
}

internal void NAKED SetMissionStats_Hook()
{
  __asm {
    push ecx
    call Detour_SetMissionStats
    pop ecx
    
    push ebx
    mov ebx, esp
    sub esp, 08h
    
    jmp [hook_setmissionstats_ret]
  }
}

internal void NAKED ShouldSkipMustDoFlag_Hook()
{
  __asm {
    push eax
    
    push esi
    push edx
    mov ecx, [ebp-0828h]
    push ecx
    call Detour_ShouldSkipMustDoFlag
    pop ecx
    movzx ecx, al
    
    pop edx
    pop esi
    pop eax
    
    test ecx, ecx
    jnz end
    
    mov ecx, [edx+020h]
    and ecx, 01h
    
  end:
    jmp [hook_shouldskipmustdoflag_ret]
  }
}

internal void NAKED DisTweaks_MissionStats_Constructor_Hook()
{
  __asm {
    push [esp+04h]
    call Detour_DisTweaks_MissionStats_Constructor
    pop eax
    
    push ebp
    mov ebp, esp
    push esi
    mov esi, [ebp+08h]
    
    jmp [hook_distweaks_missionstats_ctor_ret]
  }
}

internal void NAKED DisTweaks_MissionStats_Destructor_Hook()
{
  __asm {
    push ecx
    call Detour_DisTweaks_MissionStats_Destructor
    pop ecx
    
    push ebp
    mov ebp, esp
    push 0FFFFFFFFh
    
    jmp [hook_distweaks_missionstats_dtor_ret]
  }
}

//------------- Init -------------//
internal bool InitMissionStatsButton()
{
  if (!WriteDetour(detour_showpausemenu, ShowPauseMenu_Hook, 0))
    return false;
  if (!WriteDetour(detour_setmissionstats, SetMissionStats_Hook, 1))
    return false;
  if (!WriteDetour(detour_shouldskipmustdoflag, ShouldSkipMustDoFlag_Hook, 1))
    return false;
  if (!WriteDetour(detour_distweaks_missionstats_ctor, DisTweaks_MissionStats_Constructor_Hook, 2))
    return false;
  if (!WriteDetour(detour_distweaks_missionstats_dtor, DisTweaks_MissionStats_Destructor_Hook, 0))
    return false;
  
  return true;
}

#endif
