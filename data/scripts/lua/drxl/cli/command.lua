local class = require("middleclass")

---@class Command
local Command = class("Command")

function Command:initialize() end

---@param args string[]
---@return boolean @true on success; false otherwise
function Command:run(args)
    return true
end

return Command
