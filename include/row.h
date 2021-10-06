#ifndef __ROW_H__
#define __ROW_H__

#include <lua.h>

typedef struct
{
    int columns;
    char **names;
    char **values;
} row_t;

int row__tostring(lua_State *L);
int row_register(lua_State *L);
int row_new(lua_State *L);

#endif