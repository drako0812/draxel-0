local class = require("middleclass")
local Command = require("cli/command")
local cli = require("cli/base")
--local inspect = require("inspect").inspect

local Call = class("Call", Command)

function Call:initialize()
    Command.initialize(self)
end

function Call:run(args)
    if #args < 1 then
        cli.print("ERROR: No script passed to call\n", 0x9)
        return
    end

    --print(string.format("args = %s, %s", inspect(args), inspect(args[1])))

    local script_path = args[1]
    ARGS = {}
    for k,v in ipairs(args) do
        if k ~= 1 then
            ARGS[k-1] = v
        end
    end

    if script_path == nil then
        cli.print("ERROR: No script passed to call\n", 0x9)
        return
    end
    if exec(script_path) == false then
        cli.print(string.format("ERROR: Unable to call script: '%s'\n", script_path), 0x9)
    end
end

return Call
