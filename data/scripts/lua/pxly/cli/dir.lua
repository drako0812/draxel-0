local class = require("middleclass")
local Command = require("cli/command")
local cli = require("cli/base")
--local inspect = require("inspect").inspect

local Dir = class("Dir", Command)

function Dir:initialize()
    Command.initialize(self)
end

Dir.static.PLAIN_FILE_COLOR = 0xF -- Gray
Dir.static.LUA_FILE_COLOR = 0xA -- Yellow
Dir.static.CART_FILE_COLOR = 0xB -- Green
Dir.static.DIR_COLOR = 0xD -- Blue

function Dir:run(args)
    cli.print("Contents of Directory " .. intern.get_cwd() .. ":\n", 0x8)
    local entries = intern.get_dir_entries()
    --print("ENTRIES = " .. inspect(entries))
    for i,entry in ipairs(entries) do
        if type(entry) == "table" then
            --print("ENTRY = " .. inspect(entry))

            local etype = entry[1]
            local epath = entry[2]

            --print("ETYPE = " .. etype)
            --print("EPATH = " .. epath)

            if etype == "directory" then
                cli.print(epath .. "/\t", Dir.DIR_COLOR)
            elseif etype == "lua" then
                cli.print(epath .. "\t", Dir.LUA_FILE_COLOR)
            elseif etype == "cart" then
                cli.print(epath .. "\t", Dir.CART_FILE_COLOR)
            elseif etype == "generic" then
                cli.print(epath .. "\t", Dir.PLAIN_FILE_COLOR)
            end
        end
    end
    cli.printc('\n')
end

return Dir
