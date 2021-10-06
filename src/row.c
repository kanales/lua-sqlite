#include <string.h>
#include <lauxlib.h>

#include "row.h"

int row_new(lua_State *L)
{

    lua_newtable(L);

    //metatable
    luaL_getmetatable(L, "sqlite3.row");
    lua_setmetatable(L, -2);

    return 1;
}

int row__tostring(lua_State *L)
{
    return 1;
}

int row_register(lua_State *L)
{
    luaL_newmetatable(L, "sqlite3.row");
    lua_pushcfunction(L, row__tostring);
    lua_setfield(L, -2, "__tostring");

    return 0;
}
