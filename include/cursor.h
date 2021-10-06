#ifndef __CURSOR_H__
#define __CURSOR_H__

#include <lua.h>
#include "sqlite3.h"

typedef struct
{
    int remaining;
    sqlite3_stmt *stmt;
    int done;
} cursor_t;

cursor_t *cursor_new(lua_State *L);
cursor_t *cursor_get(lua_State *L);
int cursor_init(cursor_t *self);
int cursor_register(lua_State *L);

#endif