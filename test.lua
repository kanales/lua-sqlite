local sqlite3 = require "sqlite3"

local conn = sqlite3.open("test.db")

local cur = conn:exec "SELECT * FROM test"

for r in cur do
    print(r)
end

conn:close()
