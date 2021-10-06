#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <lua.h>

#include "sqlite3.h"

typedef struct
{
    sqlite3 *db;
} connection_t;

int connection_register(lua_State *L);
connection_t *connection_get(lua_State *L, int n);
int connection_open(lua_State *L);

#endif