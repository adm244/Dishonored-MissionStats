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

//IMPORTANT(adm244): NOT AN ACTUAL SCALEFORM SOURCE!

#ifndef _SCALEFORM_3_3_89_DISHONORED_
#define _SCALEFORM_3_3_89_DISHONORED_

struct GFxValue;
struct GFxMovie;

enum SetVarType {
  SV_Normal,
  SV_Sticky,
  SV_Permanent
};

enum SetArrayType {
  SA_Int,
  SA_Double,
  SA_Float,
  SA_String,
  SA_StringW,
  SA_Value
};

enum ValueTypeControl
{
    VTC_ConvertBit = 0x80,
    VTC_ManagedBit = 0x40,
    VTC_TypeMask = VTC_ConvertBit | 0x0F,
};

enum ValueType {
  VT_Undefined = 0x00,
  VT_Null = 0x01,
  VT_Boolean = 0x02,
  VT_Number = 0x03,
  VT_String = 0x04,
  VT_StringW = 0x05,
  VT_Object = 0x06,
  VT_Array = 0x07,
  VT_DisplayObject = 0x08,
  VT_ConvertBoolean = VTC_ConvertBit | VT_Boolean,
  VT_ConvertNumber = VTC_ConvertBit | VT_Number,
  VT_ConvertString = VTC_ConvertBit | VT_String,
  VT_ConvertStringW = VTC_ConvertBit | VT_StringW
};

typedef bool (THISCALL *_GFxMovie_GetVariable)(GFxMovie * movie, GFxValue *value, char *pathToVar);
typedef uint (THISCALL *_GFxMovie_GetVariableArraySize)(GFxMovie * movie, char *pathToVar);
typedef bool (THISCALL *_GFxMovie_GetVariableArray)(GFxMovie * movie, SetArrayType type, char *pathToVar, int index, void *data, int count);
typedef bool (THISCALL *_GFxMovie_SetVariableArray)(GFxMovie *, SetArrayType, char *, int, void *, int, SetVarType);
typedef bool (THISCALL *_GFxMovie_SetVariableArraySize)(GFxMovie *, char *, int, SetVarType);

typedef bool (THISCALL *_ValueInterface_GetMember)(void *, void *, char *, GFxValue *, bool);
internal _ValueInterface_GetMember ValueInterface_GetMember = (_ValueInterface_GetMember)0x00DA8AD0;

typedef bool (THISCALL *_ValueInterface_SetMember)(void *, void *, char *, GFxValue *, bool);
internal _ValueInterface_SetMember ValueInterface_SetMember = (_ValueInterface_SetMember)0x00DA58F0;

typedef void (THISCALL *_GFxValue_SetStringW)(GFxValue *, wchar_t *);
internal _GFxValue_SetStringW GFxValue_SetStringW = (_GFxValue_SetStringW)0x0097B740;

struct GFxValue {
  void *valueInterface;
  ValueType type;
  union {
    void *data;
    bool boolean;
    char *str;
    wchar_t *wstr;
    r64 number;
  };
};
assert_size(GFxValue, 0x10);

struct GFxMovieVTable {
  u8 unk00[0x44-0x00];
  _GFxMovie_GetVariable GetVariable; // 0x44
  _GFxMovie_SetVariableArray SetVariableArray; // 0x48
  _GFxMovie_SetVariableArraySize SetVariableArraySize; // 0x4C
  _GFxMovie_GetVariableArraySize GetVariableArraySize; // 0x50
  _GFxMovie_GetVariableArray GetVariableArray; // 0x54
  // ...
};

struct GFxMovie {
  GFxMovieVTable *vtable;
  // ...
};

/*struct ArrayObjectVTable {
  void *unk00;
  void *GetTextValue; // 0x04
  void *GetObjectType; // 0x08
  void *SetMember; // 0x0C
  _Object_GetMember GetMember; // 0x10
  // ...
};

struct ArrayObject {
  ArrayObjectVTable *vtable;
  // ...
};*/

//--------------------------------------------\\
//------------- Public interface -------------\\
//--------------------------------------------\\

internal bool GFxValue_IsDisplayObject(GFxValue *value)
{
  return (value->type & VT_DisplayObject) ? true : false;
}

internal bool GFxValue_GetMember(GFxValue *ptr, char *name, GFxValue *dest)
{
  bool isDisplayObject = GFxValue_IsDisplayObject(ptr);
  return ValueInterface_GetMember(ptr->valueInterface, ptr->data, name, dest, isDisplayObject);
}

internal bool GFxValue_SetMember(GFxValue *ptr, char *name, GFxValue *src)
{
  bool isDisplayObject = GFxValue_IsDisplayObject(ptr);
  return ValueInterface_SetMember(ptr->valueInterface, ptr->data, name, src, isDisplayObject);
}

#endif
