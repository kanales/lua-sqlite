#include <stdio.h>
#include <lauxlib.h>

#include "connection.h"
#include "cursor.h"

connection_t *connection_get(lua_State *L, int n)
{
    void *ud = luaL_checkudata(L, n, "sqlite3.connection");
    luaL_argcheck(L, ud != NULL, n, "`connection` expected");

    return (connection_t *)ud;
}

int connection_close(lua_State *L)
{
    connection_t *conn = (connection_t *)lua_touserdata(L, -1);

    sqlite3_close(conn->db);

    return 1;
}

int connection_open(lua_State *L)
{
    const char *dbname = lua_tostring(L, 1);
    connection_t *conn = (connection_t *)lua_newuserdata(L, sizeof(connection_t));

    if (sqlite3_open(dbname, &conn->db))
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(conn->db));
        sqlite3_close(conn->db);
        return (1);
    }

    luaL_getmetatable(L, "sqlite3.connection");
    lua_setmetatable(L, -2);

    return 1;
}

int connection_exec(lua_State *L)
{
    connection_t *conn = connection_get(L, 1);
    const char *s = luaL_checkstring(L, 2);
    lua_remove(L, 1);
    lua_remove(L, 1);

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(
            conn->db,
            s,
            -1,
            &stmt,
            NULL) != SQLITE_OK)
    {
        lua_pushfstring(L, "Error: %s", sqlite3_errmsg(conn->db));
        lua_error(L);
        return 0;
    }

    int expect = sqlite3_bind_parameter_count(stmt);
    int params = lua_gettop(L);

    if (expect != params)
    {
        lua_pushfstring(L, "Error: expected %d parameters; found %d", expect, params);
        lua_error(L);
        return 0;
    }

    for (int i = 1; i <= params; i++)
    {
        double n;
        int b;
        const char *str;
        switch (lua_type(L, 1))
        {
        case LUA_TNIL:
            lua_remove(L, 1);
            sqlite3_bind_null(stmt, i);
            break;
        case LUA_TNUMBER:
            n = lua_tonumber(L, 1);
            if (n == (int)n)
            {
                // n is int
                sqlite3_bind_int(stmt, i, (int)n);
            }
            else
            {
                // n is double
                sqlite3_bind_double(stmt, i, n);
            }
            break;
        case LUA_TBOOLEAN:
            b = lua_toboolean(L, 1);
            sqlite3_bind_int(stmt, i, b);
            break;
        case LUA_TSTRING:
            str = lua_tostring(L, 1);
            sqlite3_bind_text(stmt, i, str, -1, SQLITE_STATIC);
            break;

        default:
            str = luaL_typename(L, 1);
            lua_pushfstring(L, "Error: unexpected type %s; expected one of [number, boolean, string]", str);
            lua_error(L);
            break;
        }
    }

    cursor_t *cur = cursor_new(L);
    cur->stmt = stmt;
    return 1;

    // char *zErrMsg = 0;
    // int rc;

    // const char *stmt = lua_tostring(L, -1);

    // result_t result;
    // result_init(&result);

    // rc = sqlite3_exec(conn->db, stmt, callback, &result, &zErrMsg);
    // if (rc != SQLITE_OK)
    // {
    //     fprintf(stderr, "\x1b[31mError:\x1b[0m %s\n", zErrMsg);
    //     sqlite3_free(zErrMsg);
    // }

    // _lua_pushresult(L, &result);
    // result_destroy(&result);
}

int connection_register(lua_State *L)
{

    luaL_newmetatable(L, "sqlite3.connection");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, connection_close);
    lua_setfield(L, -2, "close");
    lua_pushcfunction(L, connection_exec);
    lua_setfield(L, -2, "exec");

    return 0;
}