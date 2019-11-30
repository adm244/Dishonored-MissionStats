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
  FIX(adm244): rename UArray to TArray
  FIX(adm244): rename UString to FName
*/

/*
  Get Dishonored mission stats:
    1) Get an object at [0x1452DE8] (DishonoredPlayerPawn)
    2) Get an array of mission stats at [0xB7C]
  
  MissionStatEntry:
    [0x00] Type (1 byte)
    [0x04] Current Value (4 bytes, float)
    [0x08] Maximum Value (4 bytes, int)
  
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
  Get DisGFxMoviePlayerNote to use ShowNote function:
    1) Get UIManager at [0x0144CF44]
    2) Get array of DisGFxMovie's at [0x10]
      // this list keeps all preloaded DisGFxMovie's
      // so, we have to load it somehow before we can use it :sad_face:
    3) Get Notes DisGFxMovie at [0x8] (assuming it's always in a same place)
    4) (optional) Check that value is not 0 at [0x48]
    5) Get DisGFxMoviePlayer object at [0x5C]
  
  DisGFxMoviePlayer contains NotesContents
*/

/*
  DisGFxMoviePlayer
    ...
    [0x180] UString name
    ...
  
  World object
    - WorldInfo object
      - DishonoredGameInfo object
        - GlobalUIManager object
          ...
          [0x2DC] DisGFxMoviePlayerHUD (a.k.a. Unk10)
          [0x2E0] DisGFxMoviePlayerPowerWheel
          [0x2E4] DisGFxMoviePlayerJournal
          [0x2E8] DisGFxMoviePlayerNote
          [0x2EC] DisGFxMoviePlayerPauseMenu
          [0x2F0] ???
          [0x2F4] DisGFxMoviePlayerMissionStats
          ...
*/

/*
  DisTweaks_MissionStats creation:
    1) Get DisTweak_MissionStats class at [0x01451028]
    2) Call UClass::GetDefaultObject(this, 0) at [0x004967F0]
      or just get it directly at [0x13C]
    3) TODO(adm244): Initialize it with stuff...
    
    0x1BD, 0x1BE - DisTweak_MissionStats export index ???
*/

/*
  Getting current mission number:
    1) Get DishonoredEngine object at [0x0143B20C]
    2) Get DishonoredEngineUnk01 at [0x7BC]
    3) Get current mission number at [0x80]
*/

#ifndef _DISHONORED_CPP_
#define _DISHONORED_CPP_

#include "dishonored_types.h"

internal DisTweaks_MissionStats *g_MissionStatsTweaks[30];
internal uint g_MissionStatsTweaksCount = 0;

/*internal void * GetUnk20()
{
  Unk20_1 *unk20_1 = *(Unk20_1 **)0x0144CF44;
  if (!unk20_1)
    return 0;
  
  UArray *array = &unk20_1->uiObjects;
  if (array->length < 3)
    return 0;
  
  UIObject **uiObjects = (UIObject **)array->data;
  if (!uiObjects)
    return 0;
  
  UIObject *uiNotes = uiObjects[2];
  if (uiNotes && uiNotes->flags)
    return uiNotes->unk20;
  
  return 0;
}*/

internal UString GetUString(wchar_t *text)
{
  UString str = {0};
  str.text = text;
  str.length = wcslen(text) + 1;
  str.capacity = str.length;
  
  return str;
}

/*internal NoteContents GetNoteContents(UString *title, UString *text)
{
  NoteContents note = {0};
  note.vtable = (void *)0x010DEFB8;
  note.title = *title;
  note.text = *text;
  
  return note;
}*/

internal i32 GetMissionNumber()
{
  return (*dishonoredEngine)->unk01->missionNumber;
}

internal void ShowLocationDiscovery(wchar_t *text, bool playSound)
{
  DisGFxMoviePlayerHUD *hud = GetHUD();
  UString *str = &GetUString(text);
  DisGFxMoviePlayerHUD_ShowLocationDiscovery(hud, str, playSound ? 1 : 0);
}

internal void ShowGameMessage(wchar_t *text, r32 duration)
{
  DisGFxMoviePlayerHUD *hud = GetHUD();
  UString *str = &GetUString(text);
  DisGFxMoviePlayerHUD_ShowGameMessage(hud, str, duration);
}

internal NOINLINE MissionStatEntry * GetMissionStatVariable(DishonoredPlayerPawn *playerPawn, int type)
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

ButtonsData data[] = {
  {"_root.texts.t_ResumeGame", "OnResumeClicked", LockState_None},
  {"_root.texts.t_SaveGame", "OnSaveGameClicked", LockState_SaveGame},
  {"_root.texts.t_LoadGamePauseMenu", "OnLoadGameClicked", LockState_LoadGame},
  {"_root.texts.t_Options", "OnOptionsClicked", LockState_None},
  {"_root.texts.t_MissionStats", "OnMissionStatsClicked", LockState_Stats},
  {"_root.texts.t_Help", "OnTutorialsClicked", LockState_Help},
  {"_root.texts.t_BackToMainMenu", "OnQuitGameClicked", LockState_None},
  {"_root.texts.t_BackToWindows", "OnBackToWindowsClicked", LockState_None},
};

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
    //TODO(adm244): check dlcNumber as well
    DisTweaks_MissionStats *tweaks = g_MissionStatsTweaks[i];
    if (tweaks->missionNumber == missionNumber)
      return tweaks;
  }
  
  return 0;
}

internal bool UArray_InRange(UArray *arr, int upper)
{
  if ((arr->length < 0) || (arr->length > upper))
    return false;
  
  return true;
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
      //TODO(adm244): get chaos threshold and current value
      currentValue = 0.0f;
    }
    
    buffer[i] = currentValue;
  }
  
  return true;
}

internal i32 GetMissionIndex(i32 missionNumber)
{
  //NOTE(adm244): DisTweaks_MissionStats has dlcNumber, why we're doing this?
  if (IsDLC06()) {
    missionNumber += 9;
  } else if (IsDLC07()) {
    missionNumber += 12;
  }
  
  return missionNumber;
}

internal bool SetMissionStats(DisTweaks_MissionStats *tweaks)
{
  if (!UArray_InRange(&tweaks->specialActions, 16))
    return false;
  
  //NOTE(adm244): buffer is limited to hold only 16 stat values...
  if (!UArray_InRange(&tweaks->statsValues, 16))
    return false;
  
  r32 statsValuesBuffer[16];
  u16 specialActionsFlags = GetSpecialActionsFlags(tweaks);
  if (!GetStatsValuesBuffer(tweaks, statsValuesBuffer, 16))
    return false;
  
  i32 missionIndex = GetMissionIndex(tweaks->missionNumber);
  
  ArkProfileSettings *profileSettings = (*playerController)->vtable->GetProfileSettings(*playerController);
  ArkProfileSettings_SetMissionStats(profileSettings, missionIndex, tweaks->dlcNumber, specialActionsFlags, statsValuesBuffer, tweaks->statsValues.length);
  
  return true;
}

struct OnMissionStatsClickedFunc : FunctionHandler {
  virtual void Call(Params *params)
  {
    //NOTE(adm244): just for now...
    GFxValue_Invoke(params->thisPtr, 0, "OnResumeClicked", 0, 0);
    
    //DishonoredGameInfo *gameInfo = World_GetGameInfo(*world);
    //DisGlobalUIManager *globalUIManager = gameInfo->globalUIManager;
    DisGlobalUIManager *globalUIManager = GetGlobalUIManager();
    DisGFxMoviePlayerMissionStats *missionStats = globalUIManager->missionStats;
    
    //NOTE(adm244): DisTweaks_MissionStats doesn't have actual stats data
    // instead, it is used as a template to construct mission stats screen
    // real data is stored in ArkProfileSettings, but it is set only through
    // ArkProfileSettings::SetMissionStats().
    //
    // In short, we have to get "current mission number", "current dlc number",
    // then retrieve proper DisTweaks_MissionStats for current mission and use it
    // to build two things: "mission stats array (TArray)" and "special actions flags (u16)"
    // pass those into ArkProfileSettings::SetMissionStats() and finally call
    // DisGFxMoviePlayerMissionStats::Show().
    
    //TODO(adm244): things to make mission stats button work:
    // 1) Get DisTweaks_MissionStats for current mission
    //    figure out how to get current mission index
    // 2) Use ArkProfileSettings::SetMissionStats() to set mission stats values
    //    here we have to recreate DisSeqAct_MissionStatsTracking::StoreMissionStats() logic
    // 3) Call DisGFxMoviePlayerMissionStats::Show()
    // 
    // Also, we have to modify behavior at mission stats exit,
    // so it returns to pause menu correctly...
    
    i32 missionNumber = GetMissionNumber();
    DisTweaks_MissionStats *tweaks = GetMissionStatsTweaks(missionNumber);
    
    if (SetMissionStats(tweaks)) {
      DisGFxMoviePlayerMissionStats_Show(missionStats, tweaks, 1);
    }
  }
};

internal SetPauseMenuFunc SetPauseMenu;
//internal OnMissionStatsClickedFunc OnMissionStatsClicked;
internal OnMissionStatsClickedFunc *OnMissionStatsClicked = 0;

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
  OnMissionStatsClicked = new OnMissionStatsClickedFunc();
  
  GFxMovie_CreateFunction(gfxPauseMenu, &OnMissionStatsClicked_func, OnMissionStatsClicked, 0);
  if (!GFxValue_SetMember(&pauseMenu_mc, "OnMissionStatsClicked", &OnMissionStatsClicked_func))
    return;
  
  GFxMovie_CreateFunction(gfxPauseMenu, &SetPauseMenu_func, &SetPauseMenu, 0);
  if (!GFxValue_SetMember(&pauseMenu_mc, "SetPauseMenu", &SetPauseMenu_func))
    return;
}

//internal void CDECL ShowPauseMenuPost(DisGFxMoviePlayerPauseMenu *pauseMenuMoviePlayer)
//{
//  /*
//    DisGFxMoviePlayerPauseMenu -> UI_struct (at 0x38) -> GFxMovie (at 0x34)
//    
//    GetVariable() returns a GFXValue that contains an ArrayObject (scaleform object)
//  */
//  
//  //TODO(adm244): while stuff below works perfectly fine, we have to replace a call to
//  // "SetPauseMenu" entirely in order to display any changes without any issues
//  
//  //GFxValue value = {0};
//  GFxValue _menuContent[10] = {0};
//  char *pathToVar = "_root.pauseMenu_mc._menu_mc._menuContent";
//  
//  GFxMovie *movie = pauseMenuMoviePlayer->disMovie->movie;
//  //bool result = movie->vtable->GetVariable(movie, &value, pathToVar);
//  
//  uint length = movie->vtable->GetVariableArraySize(movie, pathToVar);
//  bool result = movie->vtable->GetVariableArray(movie, SA_Value, pathToVar, 0, _menuContent, length);
//  
//  for (int i = 0; i < length; ++i) {
//    GFxValue txtField = {0};
//    GFxValue callbackField = {0};
//    GFxValue lockStateField = {0};
//    
//    bool result = false;
//    result = GFxValue_GetMember(&_menuContent[i], "txt", &txtField);
//    result = GFxValue_GetMember(&_menuContent[i], "callback", &callbackField);
//    result = GFxValue_GetMember(&_menuContent[i], "lockState", &lockStateField);
//    
//    wchar_t buffer[100];
//    swprintf(buffer, 100, L"Button %d", (i + 1));
//    GFxValue_SetStringW(&txtField, buffer);
//    
//    GFxValue_SetBoolean(&lockStateField, (i % 2) ? true : false);
//    
//    result = GFxValue_SetMember(&_menuContent[i], "txt", &txtField);
//    result = GFxValue_SetMember(&_menuContent[i], "callback", &callbackField);
//    result = GFxValue_SetMember(&_menuContent[i], "lockState", &lockStateField);
//  }
//  
//  result = movie->vtable->SetVariableArray(movie, SA_Value, pathToVar, 0, _menuContent, length, SV_Sticky);
//  
//  GFxValue _menuContentArray = {0};
//  result = movie->vtable->GetVariable(movie, &_menuContentArray, pathToVar);
//  
//  GFxValue _menu_mc = {0};
//  result = movie->vtable->GetVariable(movie, &_menu_mc, "_root.pauseMenu_mc._menu_mc");
//  
//  GFxValue invokeResult = {0};
//  result = GFxValue_Invoke(&_menu_mc, &invokeResult, "SetMenu", &_menuContentArray, 1);
//}

internal bool CDECL ModifyStatVariable(DishonoredPlayerPawn *playerPawn, int type, r32 amount)
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

internal void CDECL DisTweaks_MissionStats_Constructor(DisTweaks_MissionStats *missionStats)
{
  //FIX(adm244): hack, for now
  if (g_MissionStatsTweaksCount >= 9)
    g_MissionStatsTweaksCount = 0;
  
  g_MissionStatsTweaks[g_MissionStatsTweaksCount++] = missionStats;
}

#endif
