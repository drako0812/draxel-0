local class = require("middleclass")
local inspect = require("inspect").inspect
local Command = require("cli/command")
local Call = require("cli/call")
local Cd = require("cli/cd")
local Cls = require("cli/cls")
local Del = require("cli/del")
local Dir = require("cli/dir")
local Help = require("cli/help")
local Mkdir = require("cli/mkdir")
local cli = require("cli/base")

---@class Interpreter
---@field currentCommand Command
---@field prompt string
---@field commandString string
local Interpreter = class("Interpreter")

function Interpreter:initialize()
    self.prompt = "> "
end

---@param str string The string to search
---@param start string The string to find
---@return boolean @true if `str` starts with `start`
local function starts_with(str, start)
    return str:sub(1, #start) == start
end

---@param str string
---@param pat? string
---@return table<string>
local function split(str, pat)
    pat = pat or '[^ ]*'
    local tbl = {}
    str:gsub(pat, function(x) tbl[#tbl+1]=x end)
    return tbl
end

function Interpreter:run()
    local running = true
    local previous_command = ""
    while running == true do
        cli.print(self.prompt, 15)
        self.commandString = ""
        while kb.peekch() ~= '\x0d' do
            if not kb.keyp(73) and kb.key(73) then
                -- Pressed up arrow
                -- Get previous command entered, currently this only has a history size of 1
                local lines = #self.commandString // 48
                gpu.text.csren(false)
                gpu.text.csrx(0)
                gpu.text.csry(gpu.text.csry()-lines)
                for y=gpu.text.csry(),gpu.text.csry()+lines do
                    for x=0,48 do
                        gpu.text.putc(x, y, 15, 0, ' ')
                    end
                end
                cli.print(self.prompt)
                cli.print(previous_command)
                gpu.text.csren(true)
                self.commandString = previous_command
            end
            local c = kb.keych()
            local cb = string.byte(c)
            if c ~= '\0' then
                if c == '\b' then
                    if #self.commandString > 0 then
                        self.commandString = string.sub(self.commandString, 1, #self.commandString - 1)
                        cli.printc('\b', 15)
                        cli.printc(' ', 15)
                        cli.printc('\b', 15)
                    end
                elseif c == '\n' then
                elseif c == '\t' then
                elseif c == '\x0d' then
                    print("    Return... How did we get here?")
                elseif (cb >= 0x20) and (cb <= 0x7E) then
                    self.commandString = self.commandString .. c
                    cli.printc(c, 15)
                end
            end
            sync()
        end
        cli.printc('\n', 15)
        kb.flush()
        previous_command = self.commandString
        local args = split(self.commandString)
        --print("Args: %s", inspect(args))
        if #args >= 1 then
            local command = args[1]
            if #args >= 2 then
                args = { table.unpack(args, 2) }
            else
                args = {}
            end
            if command == "call" then
                self.currentCommand = Call:new()
                self.currentCommand:run(args)
            elseif command == "cd" then
                self.currentCommand = Cd:new()
                self.currentCommand:run(args)
            elseif command == "cls" then
                self.currentCommand = Cls:new()
                self.currentCommand:run(args)
            elseif (command == "del") or (command == "rm") then
                self.currentCommand = Del:new()
                self.currentCommand:run(args)
            elseif (command == "dir") or (command == "ls") then
                self.currentCommand = Dir:new()
                self.currentCommand:run(args)
            elseif command == "help" then
                self.currentCommand = Help:new()
                self.currentCommand:run(args)
            elseif (command == "mkdir") or (command == "md") then
                self.currentCommand = Mkdir:new()
                self.currentCommand:run(args)
            elseif command == "exit" then
                running = false
            elseif command == "" then
                cli.print("ERROR: No Command\n", 0x9)
            else
                cli.print(string.format("ERROR: Unknown command: '%s'\n", command), 0x9)
            end
        else
            cli.print("ERROR: No Command\n", 0x9)
        end
    end
end

return Interpreter
