require "luasql.sqlite3"

env = luasql.sqlite3()
conn = env:connect("test.db3")

assert(conn:execute("create table if not exists tbl1(one, two)"))
assert(conn:execute("insert into tbl1 values('hello!',10)"))
assert(conn:execute("insert into tbl1 values('goodbye',20)"))

cursor = assert(conn:execute("select * from tbl1"))
row = {}
while cursor:fetch(row) do
	z:write(table.concat(row, '|'))
end

cursor:close()
conn:close()
env:close()
