local class = require("middleclass")
local Command = require("cli/command")
local cli = require("cli/base")

local Cls = class("Cls", Command)

function Cls:initialize()
    Command.initialize(self)
end

function Cls:run(args)
    cli.cls()
    gpu.text.csrpos(0, 0)
end

return Cls
