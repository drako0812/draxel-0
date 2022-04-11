local class = require("middleclass")
local Command = require("cli/command")
local cli = require("cli/base")

local Del = class("Del", Command)

function Del:initialize()
    Command.initialize(self)
end

--[[

Del delete either a file or a directory

del <filename> ==> Deletes an individual file
del --dir <directory> ==> Deletes a directory
del -d <directory> ==> Alias for `del --dir <directory>`

--]]

function Del:run(args)
    if #args < 1 then
        cli.print("ERROR: No destination selected to delete\n", 0x9)
        return
    end

    if #args == 1 then
        -- We're in file delete mode
        if intern.delete_file(args[1]) then
            cli.print(string.format("Deleted file: '%s'\n", args[1]), 0x8)
        else
            cli.print(string.format("ERROR: Failed to delete file: '%s'\n", args[1]), 0x9)
        end
    elseif #args == 2 then
        if (args[1] == "--dir") or (args[1] == "-d") then
            if intern.delete_dir(args[2]) then
                cli.print(string.format("Deleted directory: '%s'\n", args[2]), 0x8)
            else
                cli.print(string.format("ERROR: Failed to delete directory: '%s'\n", args[2]), 0x9)
            end
        else
            cli.print(string.format("ERROR: Invalid argument: '%s'\n", args[1]), 0x9)
        end
    else
        cli.print(string.format("ERROR: Received an invalid number of arguments, expect 1 or 2, received %d\n", #args), 0x9)
    end
end

return Del
