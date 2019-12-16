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

/*
  Simple INI file parser
    
    EXAMPLE:
      01: global_key = 0
      02: 
      03: ; this is a line comment
      04: [Main Section]
      05: 
      06: key1 = 16
      07: key2 = -123
      08: key3 = +1 ; comments in values are also valid
      09: 
      10: key4 = true
      11: key5 = false
      12: 
      13: [Strings]
      14: key1 = "some c-string"
      15: key2 = L"some wide string (converted to utf16)"
    
    NOTES:
    - No dynamic memory allocations
    - Uses two arenas to store parsed data
    - CR (carrige return) characters are replaces with SPACE (' ')
    - Blank lines are ignored
    - Empty (global) section is supported
    - Section name is considered to be a string enclosed in DOUBLE_QUOTE ('"')
    - Key is one word only
    - Value can be INT, BOOL, STRING (char *) or WSTRING (wchar_t *)
    - WSTRING is assumed to be in UTF-8 encoding
    - Float values are NOT supported
    - Hexadecimal\octal\binary format of INT is NOT supported
    - Escaped characters (\t, \n, \x, etc.) are NOT supported
    
    USAGE:
      #define INI_STORAGE_SIZE 1024
      internal char ini_storage[INI_STORAGE_SIZE];
      
      int main(int argc, char *argv[])
      {
        ini_init("..\\data\\test.ini", ini_storage, INI_STORAGE_SIZE);
        bool parse_result = ini_parse();
        if (!parse_result) {
          printf("Can't read file or some lines couldn't be parsed and were skipped!");
        }
        
        int int_value = ini_read_int("section", "key", 0);
        bool bool_value = ini_read_bool(0, "globalkey", false);
        char *str_value = ini_read_string("section", "key", "default");
        wchar_t *wstr_value = ini_read_wstring("section", "key", L"default");
      }
*/

#ifndef _INIPARSER_H_
#define _INIPARSER_H_

/////////////////////////// GLOBAL DATA ////////////////////////////
#ifndef assert
#define assert
#endif

#ifndef INI_FILEPATH_MAX
#define INI_FILEPATH_MAX MAX_PATH
#endif

#ifndef INI_BUFFER_SIZE
#define INI_BUFFER_SIZE 4096
#endif

#define INI_STORAGE_MIN 64
#define INI_KEY_LENGTH 128
#define INI_VALUE_LENGTH 1024

enum ini_value_type {
  INI_VALUE_GARBAGE = 0,
  INI_VALUE_INT,
  INI_VALUE_BOOL,
  INI_VALUE_STRING,
  INI_VALUE_WSTRING,
};

enum ini_entry_type {
  INI_ENTRY_GARBAGE = 0,
  INI_ENTRY_SECTION,
  INI_ENTRY_KEYVALUE,
};

struct arena_t {
  void *base;
  void *end;
  void *cur;
  size_t count;
};

struct ini_value_t {
  ini_value_type type;
  union {
    int number;
    bool boolean;
    char *str;
    wchar_t *wstr;
  };
};

struct ini_entry_t {
  ini_entry_type type;
  char *name;
  ini_value_t value;
};

struct ini_settings_t {
  char filepath[INI_FILEPATH_MAX];
  arena_t data;
  arena_t entries;
};

internal ini_settings_t ini_settings;

//////////////////////// UTILITY FUNCTIONS /////////////////////////
internal bool is_number(char c)
{
  return (c >= '0') && (c <= '9');
}

internal bool is_sign(char c)
{
  return (c == '-') || (c == '+');
}

internal bool is_whitespace(char c)
{
  return (c == 9) || (c == 10) || (c == 13) || (c == 32);
}

///////////////////////// ARENA FUNCTIONS //////////////////////////
internal inline void arena_init(arena_t *arena, void *base, size_t size)
{
  assert(arena);
  assert(base);
  assert(size > 0);
  
  arena->base = base;
  arena->end = ((char *)base + size);
  arena->cur = arena->base;
  arena->count = 0;
}

internal inline void arena_reset(arena_t *arena)
{
  assert(arena);
  
  arena->cur = arena->base;
  arena->count = 0;
}

internal void * arena_push(arena_t *arena, void *data, size_t size)
{
  assert(arena);
  assert(data);
  
  if (size == 0) {
    size = strlen((char *)data) + 1;
  }
  
  size_t free_space = ((char *)arena->end - arena->cur);
  if (free_space < size) {
    return 0;
  }
  
  memcpy_s(arena->cur, free_space, data, size);
  
  void *result = arena->cur;
  arena->cur = (char *)arena->cur + size;
  
  assert((arena->cur >= arena->base) && (arena->cur <= arena->end));
  
  ++arena->count;
  return result;
}

//////////////////////// PRIVATE FUNCTIONS /////////////////////////
internal void ini_store_section(char *buffer)
{
  assert(buffer);
  
  ini_entry_t entry;
  entry.type = INI_ENTRY_SECTION;
  entry.name = (char *)arena_push(&ini_settings.data, buffer, 0);
  entry.value.type = INI_VALUE_GARBAGE;
  
  arena_push(&ini_settings.entries, &entry, sizeof(ini_entry_t));
}

internal void ini_store_keyvalue(char *key, ini_value_t *value)
{
  assert(key);
  assert(value);
  assert(value->type != INI_VALUE_GARBAGE);
  
  ini_entry_t entry;
  entry.type = INI_ENTRY_KEYVALUE;
  entry.name = (char *)arena_push(&ini_settings.data, key, 0);
  
  entry.value.type = value->type;
  switch (entry.value.type) {
    case INI_VALUE_INT: {
      entry.value.number = value->number;
    } break;
    
    case INI_VALUE_BOOL: {
      entry.value.boolean = value->boolean;
    } break;
    
    case INI_VALUE_STRING: {
      entry.value.str = (char *)arena_push(&ini_settings.data, value->str, 0);
    } break;
    
    case INI_VALUE_WSTRING: {
      entry.value.wstr = (wchar_t *)arena_push(&ini_settings.data, value->wstr, (wcslen(value->wstr) + 1) * 2);
    } break;
    
    default: {
      entry.value.type = INI_VALUE_GARBAGE;
    } break;
  }
  
  arena_push(&ini_settings.entries, &entry, sizeof(ini_entry_t));
}

internal bool ini_parse_read_file(char *buffer, size_t size)
{
  assert(buffer);
  
  HANDLE file = CreateFileA(ini_settings.filepath, GENERIC_READ, FILE_SHARE_READ, 0,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (file == INVALID_HANDLE_VALUE) {
    printf("Error: Cannot open file!\n");
    return false;
  }
  
  DWORD sizeRead;
  BOOL result = ReadFile(file, buffer, size, &sizeRead, 0);
  if (!result) {
    printf("Error: Cannot read file!\n");
    return false;
  }
  
  CloseHandle(file);
  
  buffer[sizeRead] = '\0';
  //NOTE(adm244): replace CR with SPACE
  for (int i = 0; i < sizeRead; ++i) {
    if (buffer[i] == 13) {
      buffer[i] = ' ';
    }
  }
  
  return true;
}

internal char * ini_nextline(char **buffer)
{
  assert(buffer);
  assert(*buffer);
  
  char *start = *buffer;
  char *p = start;
  
  if (*start == '\0') {
    return 0;
  }
  
  while ((*p != '\n') && (*p != '\0')) {
    ++p;
  }
  
  if (*p == '\0') {
    *buffer = p;
  } else {
    assert(*p == '\n');
    
    *p = '\0';
    *buffer = p + 1;
  }
  
  return start;
}

internal inline char * ini_strip_whitespace_left(char *buffer)
{
  assert(buffer);
  
  char *p = buffer;
  while (is_whitespace(*p)) {
    ++p;
  }
  return p;
}

internal inline char * ini_strip_whitespace_right(char *buffer)
{
  assert(buffer);
  
  char *start = buffer;
  char *last = 0;
  
  char *p = start;
  while (*p != '\0') {
    if (!is_whitespace(*p)) {
      last = p;
    }
    ++p;
  }
  
  if (last) {
    *(last + 1) = '\0';
  }
  
  return start;
}

internal char * ini_strip_whitespace(char *buffer)
{
  assert(buffer);
  
  buffer = ini_strip_whitespace_left(buffer);
  buffer = ini_strip_whitespace_right(buffer);
  
  return buffer;
}

internal char * ini_strip_comments(char *buffer)
{
  assert(buffer);
  
  char *start = 0;
  char *p = buffer;
  bool inside_value = false;
  
  while (*p != '\0') {
    if (*p == '\"') {
      inside_value = !inside_value;
    }
    
    if (!inside_value) {
      if (*p == ';') {
        start = p;
        break;
      }
    }
    
    ++p;
  }
  
  if (start) {
    *start = '\0';
  }
  
  return buffer;
}

internal bool ini_parse_section(char *buffer)
{
  assert(buffer);
  
  if (buffer[0] != '[') {
    return false;
  }
  
  char *start = buffer + 1;
  char *p = start;
  
  while ((*p != ']') && (*p != '\0')) {
    ++p;
  }
  
  if (*p == '\0') {
    return false;
  }
  
  assert(*p == ']');
  
  *p = '\0';
  ini_store_section(start);
  
  return true;
}

internal bool ini_parse_keyvalue_key(char *buffer, char *dest, size_t size)
{
  assert(buffer);
  assert(dest);
  assert(size > 0);
  
  char *start = buffer;
  char *p = start;
  
  while (*p != '\0') {
    if (is_whitespace(*p)) {
      break;
    }
    if (*p == '=') {
      break;
    }
    ++p;
  }
  
  if (*p == '\0') {
    return false;
  }
  
  if (*p == '=') {
    strncpy_s(dest, size, buffer, (size_t)(p - start));
    return true;
  } else {
    char *end = p;
    ++p;
    
    while (*p != '\0') {
      if (!is_whitespace(*p)) {
        break;
      }
      ++p;
    }
    
    if (*p != '=') {
      return false;
    }
    
    strncpy_s(dest, size, buffer, (size_t)(end - start));
    return true;
  }
}

internal bool ini_parse_keyvalue_value(char *buffer, char *dest, size_t size)
{
  assert(buffer);
  assert(dest);
  assert(size > 0);
  
  char *start = buffer;
  char *p = start;
  
  while (*p != '\0') {
    if (*p == '=') {
      break;
    }
    ++p;
  }
  
  if (*p != '=') {
    return false;
  }
  
  start = p + 1;
  p = start;
  
  while (*p != '\0') {
    if (!is_whitespace(*p)) {
      break;
    }
    ++p;
  }
  
  if (*p == '\0') {
    return false;
  }
  
  start = p;
  p = start;
  bool quoted = false;
  
  while (*p != '\0') {
    if (*p == '\"') {
      quoted = !quoted;
    }
    if (!quoted) {
      if (is_whitespace(*p)) {
        break;
      }
    }
    ++p;
  }
  
  if (quoted) {
    return false;
  }
  
  if (*p != '\0') {
    return false;
  }
  
  strcpy_s(dest, size, start);
  return true;
}

internal bool ini_convert_utf8_to_utf16(char *src, wchar_t *dest, size_t count)
{
  if (!MultiByteToWideChar(CP_UTF8, 0, src, -1, dest, count)) {
    return false;
  }
  
  return true;
}

internal bool ini_parse_value(char *buffer, ini_value_t *value)
{
  assert(buffer);
  assert(value);
  
  value->type = INI_VALUE_GARBAGE;
  
  if (is_number(buffer[0]) || is_sign(buffer[0])) {
    char *p = is_sign(buffer[0]) ? buffer + 1 : buffer;
    int number = 0;
    
    while (*p != '\0') {
      if (!is_number(*p)) {
        break;
      }
      
      number *= 10;
      number += (*p - '0');
      
      ++p;
    }
    
    if (*p != '\0') {
      return false;
    }
    
    if (buffer[0] == '-') {
      number = -number;
    }
    
    value->type = INI_VALUE_INT;
    value->number = number;
    return true;
  }
  
  else if ((buffer[0] == 't') || (buffer[0] == 'f')) {
    if (strcmp(buffer, "true") == 0) {
      value->type = INI_VALUE_BOOL;
      value->boolean = true;
      return true;
    }
    
    if (strcmp(buffer, "false") == 0) {
      value->type = INI_VALUE_BOOL;
      value->boolean = false;
      return true;
    }
  }
  
  else if (buffer[0] == '\"') {
    char *start = &buffer[1];
    char *p = start;
    
    //NOTE(adm244): guaranteed to have a closing quote
    while (*p != '\"') {
      ++p;
    }
    
    *p = '\0';
    
    value->type = INI_VALUE_STRING;
    value->str = start;
    return true;
  }
  
  else if (buffer[0] == 'L') {
    //NOTE(adm244): guaranteed that buffer[1] == '\"'
    char *start = &buffer[2];
    char *p = start;
    
    while (*p != '\"') {
      ++p;
    }
    
    *p = '\0';
    
    char buffer_copy[INI_BUFFER_SIZE];
    strcpy_s(buffer_copy, INI_BUFFER_SIZE, start);
    
    wchar_t *str_utf16 = (wchar_t *)buffer;
    if (!ini_convert_utf8_to_utf16(buffer_copy, str_utf16, INI_BUFFER_SIZE / 2)) {
      return false;
    }
    
    value->type = INI_VALUE_WSTRING;
    value->wstr = str_utf16;
    return true;
  }
  
  return false;
}

internal bool ini_parse_keyvalue(char *buffer)
{
  assert(buffer);
  
  char key[INI_KEY_LENGTH];
  if (!ini_parse_keyvalue_key(buffer, key, INI_KEY_LENGTH)) {
    return false;
  }
  
  char value[INI_VALUE_LENGTH];
  if (!ini_parse_keyvalue_value(buffer, value, INI_VALUE_LENGTH)) {
    return false;
  }
  
  ini_value_t value_typed;
  if (!ini_parse_value(value, &value_typed)) {
    return false;
  }
  
  ini_store_keyvalue(key, &value_typed);
  
  return true;
}

internal bool ini_parse_line(char *buffer)
{
  assert(buffer);
  
  if (buffer[0] == '[') {
    if (!ini_parse_section(buffer)) {
      return false;
    }
  } else {
    if (!ini_parse_keyvalue(buffer)) {
      return false;
    }
  }
  
  return true;
}

internal bool ini_parse_buffer(char *buffer, size_t size)
{
  assert(buffer);
  
  bool result = true;
  
  char *p = buffer;
  char *line;
  
  while (line = ini_nextline(&p)) {
    line = ini_strip_comments(line);
    line = ini_strip_whitespace(line);
    
    if (strlen(line) == 0)
      continue;
    
    if (!ini_parse_line(line)) {
      result = false;
      continue;
    }
  }
  
  return result;
}

internal ini_value_t * ini_get_value(char *section, char *key)
{
  assert(key);
  
  ini_entry_t *entries = (ini_entry_t *)ini_settings.entries.base;
  ini_value_t *value = 0;
  bool section_search = section ? true : false;
  
  for (int i = 0; i < ini_settings.entries.count; ++i) {
    if (section_search) {
      if (entries[i].type == INI_ENTRY_SECTION) {
        if (strcmp(entries[i].name, section) == 0) {
          section_search = false;
        }
      }
    } else {
      if (entries[i].type == INI_ENTRY_SECTION) {
        break;
      }
      
      if (strcmp(entries[i].name, key) == 0) {
        value = &entries[i].value;
        break;
      }
    }
  }
  
  return value;
}

///////////////////////// PUBLIC FUNCTIONS /////////////////////////
internal void ini_init(const char *filepath, void *storage, size_t size)
{
  assert(filepath);
  assert(storage);
  assert(size >= INI_STORAGE_MIN);
  
  strcpy_s(ini_settings.filepath, INI_FILEPATH_MAX, filepath);
  
  //NOTE(adm244): 3:1, 3 - data, 1 - entries
  size_t entries_size = (size / 4);
  size_t data_size = (size - entries_size - 1);
  
  arena_init(&ini_settings.data, storage, data_size);
  arena_init(&ini_settings.entries, ((char *)storage + data_size + 1), entries_size);
}

internal bool ini_parse()
{
  assert(ini_settings.filepath);
  
  arena_reset(&ini_settings.data);
  arena_reset(&ini_settings.entries);
  
  char buffer[INI_BUFFER_SIZE];
  bool result = ini_parse_read_file(buffer, INI_BUFFER_SIZE);
  if (!result) {
    //NOTE(adm244): cannot read file
    return false;
  }
  
  result = ini_parse_buffer(buffer, INI_BUFFER_SIZE);
  if (!result) {
    //NOTE(adm244): some lines couldn't be parsed
    return false;
  }
  
  return true;
}

internal int ini_read_int(char *section, char *key, int default)
{
  assert(key);
  
  ini_value_t *value = ini_get_value(section, key);
  if (!value) {
    return default;
  }
  
  if (value->type == INI_VALUE_INT) {
    return value->number;
  }
  
  if (value->type == INI_VALUE_BOOL) {
    return value->boolean;
  }
  
  return default;
}

internal bool ini_read_bool(char *section, char *key, bool default)
{
  assert(key);
  
  ini_value_t *value = ini_get_value(section, key);
  if (!value) {
    return default;
  }
  
  if (value->type == INI_VALUE_BOOL) {
    return value->boolean;
  }
  
  if (value->type == INI_VALUE_INT) {
    return (value->number) ? true : false;
  }
  
  return default;
}

internal char * ini_read_string(char *section, char *key, char *default)
{
  assert(key);
  assert(default);
  
  ini_value_t *value = ini_get_value(section, key);
  if (!value) {
    return default;
  }
  
  if (value->type == INI_VALUE_STRING) {
    return value->str;
  }
  
  return default;
}

internal wchar_t * ini_read_wstring(char *section, char *key, wchar_t *default)
{
  assert(key);
  assert(default);
  
  ini_value_t *value = ini_get_value(section, key);
  if (!value) {
    return default;
  }
  
  if (value->type == INI_VALUE_WSTRING) {
    return value->wstr;
  }
  
  return default;
}

#endif
