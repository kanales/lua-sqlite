#include <lauxlib.h>
#include <string.h>

#include "sqlite3.h"
#include "cursor.h"
#include "row.h"

cursor_t *cursor_new(lua_State *L)
{
    cursor_t *self = (cursor_t *)lua_newuserdata(L, sizeof(cursor_t));

    luaL_getmetatable(L, "sqlite3.cursor");
    lua_setmetatable(L, -2);

    cursor_init(self);
    return self;
}

int cursor_init(cursor_t *self)
{
    self->remaining = 10;
    self->stmt = NULL;
    self->done = 0;
    return 1;
}

cursor_t *cursor_get(lua_State *L)
{
    void *ud = luaL_checkudata(L, 1, "sqlite3.cursor");
    luaL_argcheck(L, ud != NULL, 1, "`cursor` expected");

    return (cursor_t *)ud;
}

int cursor__tostring(lua_State *L)
{
    cursor_t *self = cursor_get(L);

    lua_pushstring(L, "<Cursor>");
    return 1;
}

int cursor__call(lua_State *L)
{
    cursor_t *self = cursor_get(L);

    if (self->done || self->stmt == NULL)
        return 0;

    int rc = sqlite3_step(self->stmt);

    if (rc == SQLITE_DONE)
    {

        // maybe cleanup?
        self->done = 1;
        return 0;
    }

    if (rc == SQLITE_ROW)
    {
        row_new(L);
        int cols = sqlite3_column_count(self->stmt);
        for (int i = 0; i < cols; i++)
        {

            const char *name = sqlite3_column_name(self->stmt, i);
            const char *type = sqlite3_column_decltype(self->stmt, i);

            if (type == NULL)
            {
                lua_pushstring(L, (const char *)sqlite3_column_text(self->stmt, i));
            }
            else if (strcmp(type, "INTEGER") == 0)
            {
                lua_pushinteger(L, sqlite3_column_int(self->stmt, i));
            }
            else if (strcmp(type, "TEXT") == 0)
            {
                lua_pushstring(L, (const char *)sqlite3_column_text(self->stmt, i));
            }
            else if (strcmp(type, "REAL") == 0)
            {
                lua_pushnumber(L, sqlite3_column_double(self->stmt, i));
            }
            else
            {
                // string as a default
                lua_pushstring(L, (const char *)sqlite3_column_text(self->stmt, i));
            }

            lua_setfield(L, -2, name);
        }

        return 1;
    }

    if (self->remaining > 0)
    {
        lua_pushinteger(L, self->remaining);
        self->remaining -= 1;
        return 1;
    }
    return 0;
}

int cursor_register(lua_State *L)
{
    luaL_newmetatable(L, "sqlite3.cursor");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, cursor__tostring);
    lua_setfield(L, -2, "__tostring");

    lua_pushcfunction(L, cursor__call);
    lua_setfield(L, -2, "__call");

    return 0;
}