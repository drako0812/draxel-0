local class = require("middleclass")
local Command = require("cli/command")
local cli = require("cli/base")

local Mkdir = class("Mkdir", Command)

function Mkdir:initialize()
    Command.initialize(self)
end

function Mkdir:run(args)
    if #args < 1 then
        cli.print("ERROR: No directory passed to mkdir\n", 0x9)
        return
    end
    if intern.mkdir(args[1]) == true then
        cli.print(string.format("Created directory '%s'\n", args[1]), 0x8)
    else
        cli.print(string.format("ERROR: Failed to create directory '%s'\n", args[1]), 0x9)
    end
end

return Mkdir
