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

struct SetPauseMenuFunc;
struct OnMissionStatsClickedFunc;

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

//------------- Static pointers -------------//
STATIC_POINTER(void, detour_showpausemenu);
STATIC_POINTER(void, detour_distweaks_missionstats_ctor);
STATIC_POINTER(void, detour_distweaks_missionstats_dtor);

STATIC_POINTER(void, hook_showpausemenu_ret);
STATIC_POINTER(void, hook_distweaks_missionstats_ctor_ret);
STATIC_POINTER(void, hook_distweaks_missionstats_dtor_ret);

//------------- Static variables -------------//
//FIX(adm244): proper array implementation
internal DisTweaks_MissionStats *g_MissionStatsTweaks[30];
internal uint g_MissionStatsTweaksCount = 0;

internal SetPauseMenuFunc *SetPauseMenu = 0;
internal OnMissionStatsClickedFunc *OnMissionStatsClicked = 0;

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
};

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
      int missionNumber = (tweaks->missionNumber + 1);
      
      //NOTE(adm244): last mission of base game displays chaos level incorrectly
      // I guess let's do the same, why not...
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

struct OnMissionStatsClickedFunc : FunctionHandler {
  virtual void Call(Params *params)
  {
    //TODO(adm244): call Close() and replace BPressed() in MissionStats.gfx
    GFxValue_Invoke(params->thisPtr, 0, "BackToPauseMenu", 0, 0);
    
    //GFxValue bBackToGame_field = {0};
    //GFxValue_SetBoolean(&bBackToGame_field, true);
    //GFxValue_Invoke(params->thisPtr, 0, "Close", &bBackToGame_field, 1);
    
    DisGlobalUIManager *globalUIManager = GetGlobalUIManager();
    DisGFxMoviePlayerMissionStats *missionStats = globalUIManager->missionStats;
    
    i32 missionNumber = GetMissionNumber();
    DisTweaks_MissionStats *tweaks = GetMissionStatsTweaks(missionNumber);
    assert(tweaks);
    
    if (SetMissionStats(tweaks)) {
      DisGFxMoviePlayerMissionStats_Show(missionStats, tweaks, 1);
    }
  }
};

//------------- Detours -------------//
internal void CDECL ShowPauseMenu(DisGFxMoviePlayerPauseMenu *pauseMenuMoviePlayer)
{
  if (pauseMenuMoviePlayer->mode == PauseMenuMode_GameOver)
    return;
  
  GFxValue pauseMenu_mc;
  GFxValue SetPauseMenu_func;
  GFxValue OnMissionStatsClicked_func;
  
  GFxMovie *gfxPauseMenu = pauseMenuMoviePlayer->base.disMovie->movie;
  if (!GFxMovie_GetVariable(gfxPauseMenu, &pauseMenu_mc, "_root.pauseMenu_mc"))
    return;
  
  GFxValue t_MissionStats_field = {0};
  GFxValue_SetStringW(&t_MissionStats_field, L"STATISTICS");
  GFxMovie_SetVariable(gfxPauseMenu, "_root.texts.t_MissionStats", &t_MissionStats_field, SV_Normal);
  
  //IMPORTANT(adm244): so, apparently I'm being stupid and everything WAS correct
  // except that the constructor for _THIS_ class was never called, though it _WAS_
  // called for the SetPauseMenuFunc for some reason...
  // I guess I have to educate myself on how C++ nightmare _REALLY_ works...
  //
  // Nonetheless, good thing I've checked what the actual problem was,
  // never would've thought that the issue lies in garbage being stored in a vtable...
  //FIX(adm244): memory leaks
  SetPauseMenu = new SetPauseMenuFunc();
  OnMissionStatsClicked = new OnMissionStatsClickedFunc();
  
  GFxMovie_CreateFunction(gfxPauseMenu, &OnMissionStatsClicked_func, OnMissionStatsClicked, 0);
  if (!GFxValue_SetMember(&pauseMenu_mc, "OnMissionStatsClicked", &OnMissionStatsClicked_func))
    return;
  
  GFxMovie_CreateFunction(gfxPauseMenu, &SetPauseMenu_func, SetPauseMenu, 0);
  if (!GFxValue_SetMember(&pauseMenu_mc, "SetPauseMenu", &SetPauseMenu_func))
    return;
}

internal void CDECL DisTweaks_MissionStats_Constructor(DisTweaks_MissionStats *missionStats)
{
  g_MissionStatsTweaks[g_MissionStatsTweaksCount] = missionStats;
  ++g_MissionStatsTweaksCount;
}

internal void CDECL DisTweaks_MissionStats_Destructor(DisTweaks_MissionStats *missionStats)
{
  if (g_MissionStatsTweaksCount != 0)
    g_MissionStatsTweaksCount = 0;
}

//------------- Hooks -------------//
internal void NAKED ShowPauseMenu_Hook()
{
  __asm {
    push ecx
    call ShowPauseMenu
    pop ecx
    
    push ebp
    mov ebp, esp
    push 0FFFFFFFFh
    
    jmp [hook_showpausemenu_ret]
  }
}

internal void NAKED DisTweaks_MissionStats_Constructor_Hook()
{
  __asm {
    push [esp+04h]
    call DisTweaks_MissionStats_Constructor
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
    call DisTweaks_MissionStats_Destructor
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
  if (!WriteDetour(detour_distweaks_missionstats_ctor, DisTweaks_MissionStats_Constructor_Hook, 2))
    return false;
  if (!WriteDetour(detour_distweaks_missionstats_dtor, DisTweaks_MissionStats_Destructor_Hook, 0))
    return false;
  
  return true;
}

#endif
