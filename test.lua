local sqlite3 = require "sqlite3"

local conn = sqlite3.open(":memory:")

conn:exec [[
    CREATE TABLE test (
        id      INTEGER PRIMARY KEY AUTOINCREMENT,
        data    TEXT
    )
]]()

conn:exec [[
    INSERT INTO test
    VALUES
        (NULL, "A"),
        (NULL, "B"),
        (NULL, "C"),
        (NULL, "D"),
        (NULL, "E")
]]()
print("---")

for r in conn:exec "SELECT id, data FROM test" do
    print(r.id, r.data)
end
--[[
    1       A
    2       B
    3       C
    4       D
    5       E
]]

local stmt = [[
    SELECT 
        id,
        data 
    FROM 
        test 
    WHERE 
        id < ?
]]
print("---")

for r in conn:exec(stmt, 4) do
    print(r.id, r.data)
end
--[[
    1       A
    2       B
    3       C
]]
print("---")
local stmt = [[
    SELECT 
        id * 2 AS d
    FROM 
        test 
    WHERE 
        id < ?
]]

for r in conn:exec(stmt, 4) do
    print(r.d)
end
--[[
    2
    4
    6
]]

conn:close()
