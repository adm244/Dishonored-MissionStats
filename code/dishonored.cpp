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

#ifndef _DISHONORED_CPP_
#define _DISHONORED_CPP_

#include "scaleform3.h"

enum MissionStatTypes {
  MissionStat_AlarmsRung = 0x03,
  MissionStat_BodiesFound = 0x04,
  MissionStat_DetectedTimes = 0x0A,
  MissionStat_OverallChaos = 0x1B,
  MissionStat_HostilesKilled = 0x1C,
  MissionStat_CiviliansKilled = 0x1E,
  MissionStat_CoinsFound = 0x21,
  MissionStat_RunesFound = 0x22,
  MissionStat_BoneCharmsFound = 0x23,
  MissionStat_OutsiderShrinesFound = 0x24,
  MissionStat_SokolovPaintingsFound = 0x25,
};

enum DisGFxMoviePlayerPauseMenu_Modes {
  PauseMenuMode_GameOver = 2,
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
  u32 maxValue;
};
assert_size(MissionStatEntry, 0xC);

struct DishonoredPlayerPawn {
  u8 unk0[0xB7C-0x0];
  Array missionStats; // 0xB7C
  // ...
};

struct DisGFxMoviePlayer;

struct DisGFxMovie {
  UString name;
  u8 unk0C[0x34-0x0C];
  GFxMovie *movie; // 0x34
  u32 unk38;
  u32 unk3C;
  u32 unk40;
  u32 unk44;
  u32 flags; // 0x48
  u8 unk4C[0x5C-0x4C];
  DisGFxMoviePlayer *disMoviePlayer; // 0x5C
  // ...
};

typedef bool (THISCALL *_DisGFxMoviePlayer_Start)(DisGFxMoviePlayer *, void *);

struct DisGFxMoviePlayerVTable {
  u8 unk00[0x124-0x0];
  void *Preload; // 0x124
  _DisGFxMoviePlayer_Start Start; // 0x128
  void *Advance; // 0x12C
  void *unk130;
  void *unk134;
  void *Close; // 0x138
  // ...
};

struct DisGFxMoviePlayer {
  DisGFxMoviePlayerVTable *vtable;
  u8 unk04[0x38-0x04];
  DisGFxMovie *disMovie; // 0x38
  u8 unk3C[0xC4-0x3C];
  u32 flags; // 0xC4
  u8 unkC8[0x180-0xC8];
  UString name; // 0x180
  // ...
};

struct DisGFxMoviePlayerPauseMenu {
  DisGFxMoviePlayer base;
  u8 unk3C[0x1F8-sizeof(DisGFxMoviePlayer)];
  u8 mode; // 0x1F8
  // ...
};
assert_size(DisGFxMoviePlayerPauseMenu, 0x1FC);

struct DisGFxMoviePlayerHUD {
  DisGFxMoviePlayer base;
  // ...
};

struct DisGFxMoviePlayerPowerWheel {
  DisGFxMoviePlayer base;
  // ...
};

struct DisGFxMoviePlayerJournal {
  DisGFxMoviePlayer base;
  // ...
};

struct DisGFxMoviePlayerNote {
  DisGFxMoviePlayer base;
  // ...
};

struct DisGFxMoviePlayerMissionStats {
  DisGFxMoviePlayer base;
  // ...
};

struct DisGlobalUIManager {
  void *vtable;
  u8 unk04[0x2DC-0x4];
  DisGFxMoviePlayerHUD *hud; // 0x2DC
  DisGFxMoviePlayerPowerWheel *powerWheel; // 0x2E0
  DisGFxMoviePlayerJournal *journal; // 0x2E4
  DisGFxMoviePlayerNote *note; // 0x2E8
  DisGFxMoviePlayerPauseMenu *pauseMenu; // 0x2EC
  void *unk2F0;
  DisGFxMoviePlayerMissionStats *missionStats; // 0x2F4
  // ...
};
assert_size(DisGlobalUIManager, 0x2F8);

struct DishonoredGameInfo {
  void *vtable;
  u8 unk04[0x41C-0x4];
  DisGlobalUIManager *globalUIManager; // 0x41C
  u8 unk420[0x14E0-0x420];
  UString missionNames[21]; // 0x14E0
  // ...
};
assert_size(DishonoredGameInfo, 0x15DC);

struct WorldInfo {
  void *vtable;
  u8 unk04[0x410-0x4];
  DishonoredGameInfo *gameInfo; // 0x410
  // ...
};
assert_size(WorldInfo, 0x414);

struct World {
  void *vtable;
  u8 unk04[0x2C0-0x4];
  WorldInfo *worldInfo; // 0x2C0
  // ...
};
assert_size(World, 0x2C4);

/*struct UIObject {
  UString name;
  u32 unk0C[15];
  u32 flags; // 0x48
  u32 unk4C[4];
  void *unk20; // 0x5C
  // ...
};

struct Unk20_1 {
  void *vtable;
  u32 unk04;
  u32 unk08;
  u32 unk0C;
  Array uiObjects; // 0x10
  // ...
};

struct NoteContents {
  void *vtable;
  UString title;
  UString text;
};
assert_size(NoteContents, 0x1C);*/

#pragma pack(pop)

internal World** world = (World **)(0x0143D878);

typedef DishonoredGameInfo * (THISCALL *_World_GetGameInfo)(World *);
typedef DisGlobalUIManager * (CDECL _GetGlobalUIManager)();
typedef DisGFxMoviePlayerHUD * (STDCALL _GetHUD)();
typedef void (THISCALL *_DisGFxMoviePlayerHUD_ShowLocationDiscovery)(DisGFxMoviePlayerHUD *, UString *, int);
typedef void (THISCALL *_DisGFxMoviePlayerHUD_ShowGameMessage)(DisGFxMoviePlayerHUD *, UString *, r32);
//typedef void (THISCALL *_Unk20_ShowNote)(void *unk20, NoteContents *contents, int unk03);

internal _World_GetGameInfo World_GetGameInfo = (_World_GetGameInfo)(0x007816E0);
internal _GetGlobalUIManager *GetGlobalUIManager = (_GetGlobalUIManager *)(0x00BBF730);
internal _GetHUD *GetHUD = (_GetHUD *)0x00BBF760;
internal _DisGFxMoviePlayerHUD_ShowLocationDiscovery DisGFxMoviePlayerHUD_ShowLocationDiscovery = (_DisGFxMoviePlayerHUD_ShowLocationDiscovery)0x00B94D60;
internal _DisGFxMoviePlayerHUD_ShowGameMessage DisGFxMoviePlayerHUD_ShowGameMessage = (_DisGFxMoviePlayerHUD_ShowGameMessage)0x00BAFFA0;
//internal _Unk20_ShowNote Unk20_ShowNote = (_Unk20_ShowNote)0x00BCC2B0;

/*internal void * GetUnk20()
{
  Unk20_1 *unk20_1 = *(Unk20_1 **)0x0144CF44;
  if (!unk20_1)
    return 0;
  
  Array *array = &unk20_1->uiObjects;
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
  Array *missionStats = &playerPawn->missionStats;
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
  {"_root.texts.t_MissionStats", "OnMissionStatsClicked", LockState_None},
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

struct UObject;

struct UClass {
  void *vtable;
  u8 unk04[0x24-0x4];
  UObject *package; // 0x24
  u8 unk28[0x44-0x28];
  UClass *classWithin; // 0x44
  u8 unk48[0x130-0x48];
  UString name; // 0x130
  UObject *defaultObject; // 0x13C
  u8 unk140[0x1B4-0x140];
};
assert_size(UClass, 0x1B4);

struct UObject {
  void *vtable;
  u32 unk04;
  u32 flags; // 0x8
  u32 unk0C;
  u32 unk10;
  u32 unk14;
  u32 unk18;
  u32 unk1C;
  u32 unk20;
  u32 unk24;
  u32 uniqueId; // 0x28
  u32 unk2C;
  UClass *classPtr; // 0x30
  u32 unk34;
};
assert_size(UObject, 0x38);

struct DisTweaks_MissionStats {
  void *vtable;
};

typedef int (THISCALL *_DisGFxMoviePlayerMissionStats_Show)(DisGFxMoviePlayerMissionStats *, DisTweaks_MissionStats *, int);
internal _DisGFxMoviePlayerMissionStats_Show DisGFxMoviePlayerMissionStats_Show = (_DisGFxMoviePlayerMissionStats_Show)(0x00BD3A60);

typedef UObject * (CDECL _NewObject)(UClass *classPtr, UObject *package, int uniqueId, int, int, int, UObject *defaultObject, int, int, int);
internal _NewObject *NewObject = (_NewObject *)(0x00494C80);

typedef UObject * (THISCALL *_UClass_GetDefaultObject)(UClass *, int);
internal _UClass_GetDefaultObject UClass_GetDefaultObject = (_UClass_GetDefaultObject)(0x004967F0);

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
    
    //DisGFxMoviePlayerMissionStats_Show(missionStats, tweaksMissionStats, 1);
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

#endif
