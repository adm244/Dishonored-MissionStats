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
struct FunctionHandler;

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

enum ValueTypeControl {
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

typedef void (THISCALL *_GFxMovie_CreateString)(GFxMovie *, GFxValue *, char *);
typedef void (THISCALL *_GFxMovie_CreateStringW)(GFxMovie *, GFxValue *, wchar_t *);
typedef void (THISCALL *_GFxMovie_CreateObject)(GFxMovie *, GFxValue *, char *, GFxValue *, int);
typedef void (THISCALL *_GFxMovie_CreateArray)(GFxMovie *, GFxValue *);
typedef void (THISCALL *_GFxMovie_CreateFunction)(GFxMovie *, GFxValue *, FunctionHandler *, void *);

typedef bool (THISCALL *_GFxMovie_SetVariable)(GFxMovie *, char *, GFxValue *, SetVarType);
typedef bool (THISCALL *_GFxMovie_GetVariable)(GFxMovie *, GFxValue *, char *);
typedef bool (THISCALL *_GFxMovie_SetVariableArray)(GFxMovie *, SetArrayType, char *, int, void *, int, SetVarType);
typedef bool (THISCALL *_GFxMovie_SetVariableArraySize)(GFxMovie *, char *, int, SetVarType);
typedef uint (THISCALL *_GFxMovie_GetVariableArraySize)(GFxMovie *, char *);
typedef bool (THISCALL *_GFxMovie_GetVariableArray)(GFxMovie *, SetArrayType, char *, int, void *, int);

typedef bool (THISCALL *_ValueInterface_GetMember)(void *, void *, char *, GFxValue *, bool);
typedef bool (THISCALL *_ValueInterface_SetMember)(void *, void *, char *, GFxValue *, bool);
typedef bool (THISCALL *_ValueInterface_Invoke)(void *, void *, GFxValue *, char *, GFxValue *, int, bool);
typedef bool (THISCALL *_ValueInterface_PushBack)(void *, void *, GFxValue *);
typedef bool (THISCALL *_ValueInterface_PopBack)(void *, void *, GFxValue *);

internal _ValueInterface_GetMember ValueInterface_GetMember = (_ValueInterface_GetMember)0x00DA8AD0;
internal _ValueInterface_SetMember ValueInterface_SetMember = (_ValueInterface_SetMember)0x00DA58F0;
internal _ValueInterface_Invoke ValueInterface_Invoke = (_ValueInterface_Invoke)0x00DA8230;
internal _ValueInterface_PushBack ValueInterface_PushBack = (_ValueInterface_PushBack)0x00DA5A20;
internal _ValueInterface_PopBack ValueInterface_PopBack = (_ValueInterface_PopBack)0x00DA68C0;

typedef void (THISCALL *_GFxValue_SetUndefined)(GFxValue *);
typedef void (THISCALL *_GFxValue_SetNull)(GFxValue *);
typedef void (THISCALL *_GFxValue_SetBoolean)(GFxValue *, bool);
typedef void (THISCALL *_GFxValue_SetDouble)(GFxValue *, double);
typedef void (THISCALL *_GFxValue_SetStringW)(GFxValue *, wchar_t *);

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
  u8 unk00[0x2C-0x00];
  _GFxMovie_CreateString CreateString; // 0x2C
  _GFxMovie_CreateStringW CreateStringW; // 0x30
  _GFxMovie_CreateObject CreateObject; // 0x34
  _GFxMovie_CreateArray CreateArray; // 0x38
  _GFxMovie_CreateFunction CreateFunction; // 0x3C
  _GFxMovie_SetVariable SetVariable; // 0x40
  _GFxMovie_GetVariable GetVariable; // 0x44
  _GFxMovie_SetVariableArray SetVariableArray; // 0x48
  _GFxMovie_SetVariableArraySize SetVariableArraySize; // 0x4C
  _GFxMovie_GetVariableArraySize GetVariableArraySize; // 0x50
  _GFxMovie_GetVariableArray GetVariableArray; // 0x54
  // ...
};
assert_size(GFxMovieVTable, 0x58);

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

struct FunctionHandler {
  struct Params {
    GFxValue* retValue;
    GFxMovie* movie;
    GFxValue* thisPtr;
    GFxValue* argsWithThisRef;
    GFxValue* args;
    int argsCount;
    void* userData;
  };
  
  virtual ~FunctionHandler() {}
  virtual void Call(Params *params) = 0;
};
assert_size(FunctionHandler, 0x4);

//--------------------------------------------\\
//------------- Public interface -------------\\
//--------------------------------------------\\

#define GFxMovie_CreateString(p, a, b) ((p)->vtable->CreateString(p, a, b))
#define GFxMovie_CreateStringW(p, a, b) ((p)->vtable->CreateStringW(p, a, b))
#define GFxMovie_CreateObject(p, a, b, c, d) ((p)->vtable->CreateObject(p, a, b, c, d))
#define GFxMovie_CreateArray(p, a) ((p)->vtable->CreateArray(p, a))
#define GFxMovie_CreateFunction(p, a, b, c) ((p)->vtable->CreateFunction(p, a, b, c))

#define GFxMovie_SetVariable(p, a, b, c) ((p)->vtable->SetVariable(p, a, b, c))
#define GFxMovie_GetVariable(p, a, b) ((p)->vtable->GetVariable(p, a, b))
#define GFxMovie_SetVariableArray(p, a, b, c, d, e, f) ((p)->vtable->SetVariableArray(p, a, b, c, d, e, f))
#define GFxMovie_SetVariableArraySize(p, a, b, c) ((p)->vtable->SetVariableArraySize(p, a, b, c))
#define GFxMovie_GetVariableArraySize(p, a) ((p)->vtable->GetVariableArraySize(p, a))
#define GFxMovie_GetVariableArray(p, a, b, c, d, e) ((p)->vtable->GetVariableArray(p, a, b, c, d, e))

internal _GFxValue_SetUndefined GFxValue_SetUndefined = (_GFxValue_SetUndefined)0x0097B650;
internal _GFxValue_SetNull GFxValue_SetNull = (_GFxValue_SetNull)0x0097B680;
internal _GFxValue_SetBoolean GFxValue_SetBoolean = (_GFxValue_SetBoolean)0x0097B6B0;
internal _GFxValue_SetDouble GFxValue_SetDouble = (_GFxValue_SetDouble)0x0097B700;
internal _GFxValue_SetStringW GFxValue_SetStringW = (_GFxValue_SetStringW)0x0097B740;

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

//FIX(adm244): swap result and name args
internal bool GFxValue_Invoke(GFxValue *ptr, GFxValue *result, char *name, GFxValue *args, int argsCount)
{
  bool isDisplayObject = GFxValue_IsDisplayObject(ptr);
  return ValueInterface_Invoke(ptr->valueInterface, ptr->data, result, name, args, argsCount, isDisplayObject);
}

internal bool GFxValue_PushBack(GFxValue *ptr, GFxValue *value)
{
  return ValueInterface_PushBack(ptr->valueInterface, ptr->data, value);
}

internal bool GFxValue_PopBack(GFxValue *ptr, GFxValue *value)
{
  return ValueInterface_PopBack(ptr->valueInterface, ptr->data, value);
}

#endif
