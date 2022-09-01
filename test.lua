local r = require "roadmap"

local m = r.map {
	{ 1, 2, 100 },
	{ 1, 3, 42 },
	{ 2, 3, 10 },
}

r.dumpmap(m)

local cache = r.routecache(m)

local function path(from, to)
	local checkpoint = from
	while checkpoint ~= to do
		local index = checkpoint << 16 | to
		local dest = cache[index]
		print(checkpoint, dest)
		checkpoint = dest
	end
end

path(1,2)

