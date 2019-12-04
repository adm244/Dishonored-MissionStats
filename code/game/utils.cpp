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

#ifndef _GAME_COMMON_UTILS_CPP_
#define _GAME_COMMON_UTILS_CPP_

internal INLINE bool IsFlagSet(u32 value, u32 flag)
{
  return (value & flag);
}

internal INLINE void SetFlag(u32 *value, u32 flag)
{
  (*value) |= (flag);
}

internal INLINE void ClearFlag(u32 *value, u32 flag)
{
  (*value) &= (~flag);
}

internal UString GetUString(wchar_t *text)
{
  UString str = {0};
  str.text = text;
  str.length = wcslen(text) + 1;
  str.capacity = str.length;
  
  return str;
}

internal bool UArray_InRange(UArray *arr, int upper)
{
  if ((arr->length < 0) || (arr->length > upper))
    return false;
  
  return true;
}

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

internal i32 GetMissionNumber()
{
  return (*dishonoredEngine)->unk01->missionNumber;
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

/*internal NoteContents GetNoteContents(UString *title, UString *text)
{
  NoteContents note = {0};
  note.vtable = (void *)0x010DEFB8;
  note.title = *title;
  note.text = *text;
  
  return note;
}*/

#endif
