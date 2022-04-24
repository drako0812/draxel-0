local class = require("middleclass")
local Command = require("cli/command")
local cli = require("cli/base")

local Cd = class("Cd", Command)

function Cd:initialize()
    Command.initialize(self)
end

function Cd:run(args)
    if #args == 0 then
        -- Display CWD
        cli.print(string.format("%s\n", intern.get_cwd()), 0x8)
    elseif #args == 1 then
        -- Change CWD
        if (args[1] == "--reset") or (args[1] == "-r") then
            intern.reset_cwd()
        else
            intern.set_cwd(args[1])
        end
        cli.print(string.format("%s\n", intern.get_cwd()), 0x8)
    else
        -- Too many args
        cli.print("ERROR: Too many arguments\n", 0x9)
    end
end

return Cd
