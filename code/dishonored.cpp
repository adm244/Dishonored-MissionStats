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
  Get Unk20 to use ShowNote function:
    1) Get object at [0x0144CF44]
    2) Get array of UIObject's at [0x10]
      // this list keeps all preloaded scaleform gfx objects
      // so, we have to load it somehow before we can use it :sad_face:
    3) Get Notes UIObject at [0x8] (assuming it's always in a same place)
    4) (optional) Check that value is not 0 at [0x48]
    5) Get Unk20 object at [0x5C]
  
  Unk20 contains NotesContents
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

struct Unk01 {
  u8 unk0[0xB7C-0x0];
  Array missionStats; // 0xB7C
  // ...
};

//FIX(adm244): same as UIObject???
struct DisGFxMovie {
  UString name;
  u8 unk0C[0x34-0x0C];
  GFxMovie *movie; // 0x34
  // ...
};

struct DisGFxMoviePlayerPauseMenu {
  void *vtable;
  u8 unk04[0x38-0x04];
  DisGFxMovie *disMovie; // 0x38
  // ...
};

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

typedef void * (STDCALL _GetUnk10)();
typedef void (THISCALL *_Unk10_ShowLocationDiscovery)(void *unk10, UString *text, int playSound);
typedef void (THISCALL *_Unk10_ShowGameMessage)(void *unk10, UString *text, r32 duration);
//typedef void (THISCALL *_Unk20_ShowNote)(void *unk20, NoteContents *contents, int unk03);

internal _GetUnk10 *GetUnk10 = (_GetUnk10 *)0x00BBF760;
internal _Unk10_ShowLocationDiscovery Unk10_ShowLocationDiscovery = (_Unk10_ShowLocationDiscovery)0x00B94D60;
internal _Unk10_ShowGameMessage Unk10_ShowGameMessage = (_Unk10_ShowGameMessage)0x00BAFFA0;
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
  void *unk10 = GetUnk10();
  UString *str = &GetUString(text);
  Unk10_ShowLocationDiscovery(unk10, str, playSound ? 1 : 0);
}

internal void ShowGameMessage(wchar_t *text, r32 duration)
{
  void *unk10 = GetUnk10();
  UString *str = &GetUString(text);
  Unk10_ShowGameMessage(unk10, str, duration);
}

//INVESTIGATE(adm244): In release mode when msvc 2010 inlines this function
// the return value overwrites unk parameter on a stack with itself,
// but it happens only if this function returns float, all is fine when integer is returned
// Possible compiler bug or am I missing something regarding calling-conventions?
internal NOINLINE MissionStatEntry * GetMissionStatVariable(Unk01 *unk, int type)
{
  Array *missionStats = &unk->missionStats;
  MissionStatEntry *missionStatEntries = (MissionStatEntry *)missionStats->data;
  
  for (int i = 0; i < missionStats->length; ++i) {
    if (missionStatEntries[i].type == (u8)type) {
      return &missionStatEntries[i];
    }
  }
  
  OutputDebugStringA("GetMissionStatVariable: Couldn't find a specified mission stat.");
  return 0;
}

internal void CDECL ShowPauseMenuPost(DisGFxMoviePlayerPauseMenu *pauseMenuMoviePlayer)
{
  /*
    DisGFxMoviePlayerPauseMenu -> UI_struct (at 0x38) -> GFxMovie (at 0x34)
    
    GetVariable() returns a GFXValue that contains an ArrayObject (scaleform object)
  */
  
  //TODO(adm244): while stuff below works perfectly fine, we have to replace a call to
  // "ShowPauseMenu" entirely in order to display any changes...
  
  //GFxValue value = {0};
  GFxValue _menuContent[10] = {0};
  char *pathToVar = "_root.pauseMenu_mc._menu_mc._menuContent";
  
  GFxMovie *movie = pauseMenuMoviePlayer->disMovie->movie;
  //bool result = movie->vtable->GetVariable(movie, &value, pathToVar);
  
  uint length = movie->vtable->GetVariableArraySize(movie, pathToVar);
  bool result = movie->vtable->GetVariableArray(movie, SA_Value, pathToVar, 0, _menuContent, length);
  
  for (int i = 0; i < length; ++i) {
    GFxValue txtField = {0};
    GFxValue callbackField = {0};
    GFxValue lockStateField = {0};
    
    bool result = false;
    result = GFxValue_GetMember(&_menuContent[i], "txt", &txtField);
    result = GFxValue_GetMember(&_menuContent[i], "callback", &callbackField);
    result = GFxValue_GetMember(&_menuContent[i], "lockState", &lockStateField);
    
    wchar_t buffer[100];
    swprintf(buffer, 100, L"Button %d", (i + 1));
    GFxValue_SetStringW(&txtField, buffer);
    
    /*lockStateField.valueInterface = txtField.valueInterface;
    lockStateField.type = VT_Boolean;
    lockStateField.boolean = (i % 2) ? true : false;*/
    
    result = GFxValue_SetMember(&_menuContent[i], "txt", &txtField);
    result = GFxValue_SetMember(&_menuContent[i], "callback", &callbackField);
    //result = GFxValue_SetMember(&_menuContent[i], "lockState", &lockStateField);
  }
  
  result = movie->vtable->SetVariableArray(movie, SA_Value, pathToVar, 0, _menuContent, length, SV_Sticky);
}

internal bool CDECL ModifyStatVariable(Unk01 *unk, int type, r32 amount)
{
  MissionStatEntry *stat = GetMissionStatVariable(unk, type);
  
  switch (type) {
    case MissionStat_DetectedTimes: {
      if ((!stat->value) && (amount > 0.0f)) {
        ShowLocationDiscovery(L"You've been spotted", false);
      }
    } break;
    
    case MissionStat_HostilesKilled:
    case MissionStat_CiviliansKilled: {
      MissionStatEntry *hostilesStat = GetMissionStatVariable(unk, MissionStat_HostilesKilled);
      MissionStatEntry *civiliansStat = GetMissionStatVariable(unk, MissionStat_CiviliansKilled);
      
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
