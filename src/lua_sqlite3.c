#include <lauxlib.h>

#include "row.h"
#include "cursor.h"
#include "connection.h"

int cursor_dummy(lua_State *L)
{
    cursor_new(L);
    return 1;
}

int luaopen_sqlite3(lua_State *L)
{

    row_register(L);
    connection_register(L);
    cursor_register(L);

    lua_newtable(L);

    lua_pushcfunction(L, connection_open);
    lua_setfield(L, -2, "open");

    lua_pushcfunction(L, cursor_dummy);
    lua_setfield(L, -2, "cursor_dummy");

    return 1;
}