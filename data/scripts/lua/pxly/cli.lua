local class = require("middleclass")

local cli = {}

cli.__DefaultForegroundColor = 15
cli.__DefaultBackgroundColor = 0
cli.__DefaultClearCharacter = " "
cli.__TextModeWidth = 48
cli.__TextModeHeight = 27

---@param char string
---@param fg? integer
---@param bg? integer
---@param clear_ch? string
---
--- Prints a single `char` to Pixelly's internal console
function cli.printc(char, fg, bg, clear_ch)
    fg = fg or cli.__DefaultForegroundColor
    bg = bg or cli.__DefaultBackgroundColor
    clear_ch = clear_ch or cli.__DefaultClearCharacter
    
    local char_byte = string.byte(char)
    if char == "\t" then
        gpu.text.csrx((gpu.text.csrx()+4) & ~(4-1))
    elseif (char == "\b") and (gpu.text.csrx() >= 0) then
        gpu.text.csrx(gpu.text.csrx() - 1)
    elseif char == "\r" then
        gpu.text.csrx(0)
    elseif char == "\n" then
        gpu.text.csrx(0)
        gpu.text.csry(gpu.text.csry()+1)
    elseif (char_byte >= 0x20) and (char_byte <= 0x7E) then
        gpu.text.putc(gpu.text.csrx(), gpu.text.csry(), fg, bg, char)
        gpu.text.csrx(gpu.text.csrx()+1)
    end

    if gpu.text.csrx() >= cli.__TextModeWidth then
        gpu.text.csrx(0)
        gpu.text.csry(gpu.text.csry()+1)
    end

    if gpu.text.csry() >= cli.__TextModeHeight then
        gpu.text.csry(cli.__TextModeHeight - 1)
        gpu.text.scrollu(1, fg, bg, clear_ch)
    end
end

---@param text string
---@param fg? integer
---@param bg? integer
---@param clear_ch? string
---
--- Prints `text` to Pixelly's internal console
function cli.print(text, fg, bg, clear_ch)
    fg = fg or cli.__DefaultForegroundColor
    bg = bg or cli.__DefaultBackgroundColor
    clear_ch = clear_ch or cli.__DefaultClearCharacter
    
    for i = 1, #text do
        cli.printc(string.sub(text, i), fg, bg, clear_ch)
    end
end

---@param fg? integer
---@param bg? integer
---@param clear_ch? string
---
--- Clears Pixelly's internal console
function cli.cls(bg, fg, clear_ch)
    fg = fg or cli.__DefaultForegroundColor
    bg = bg or cli.__DefaultBackgroundColor
    clear_ch = clear_ch or cli.__DefaultClearCharacter

    gpu.text.cls(fg, bg, clear_ch)
end

---@param seconds number
---
--- Pauses execution for `seconds`
function cli.wait(seconds)
    local start = os.clock()
    local now = os.clock()
    while (now - start) < seconds do
        now = os.clock()
    end
end

---@class cli.Command
cli.Command = class("Command")

function cli.Command:initialize() end

---@param args string[]
---@return boolean @true on success; false otherwise
function cli.Command:run(args)
    return true
end

---@class cli.Help : cli.Command
cli.Help = class("Help", cli.Command)

function cli.Help:initialize()
    cli.Command.initialize(self)
end

---@type integer
--- Enabled command color
cli.Help.static.ENABLED_COLOR = 15
---@type integer
--- Disabled command color
cli.Help.static.DISABLED_COLOR = 8

function cli.Help:run(args)
    -- Ingore args
    args = args

    cli.print("List of commands:\n")
    cli.print("- call <script> [args...]\n", cli.Help.DISABLED_COLOR)
    cli.print("- cd [dir]\n", cli.Help.DISABLED_COLOR)
    cli.print("- dir/ls\n", cli.Help.DISABLED_COLOR)
    cli.print("- exit\n", cli.Help.DISABLED_COLOR)
    cli.print("- help\n", cli.Help.ENABLED_COLOR)
    cli.print("- load <cart>\n", cli.Help.DISABLED_COLOR)
    cli.print("- new\n", cli.Help.DISABLED_COLOR)
    cli.print("- run\n", cli.Help.DISABLED_COLOR)
    cli.print("- save [cart]\n", cli.Help.DISABLED_COLOR)
    cli.print("- unload\n", cli.Help.DISABLED_COLOR)

    return true
end

---@class cli.Interpreter
---@field currentCommand cli.Command
---@field prompt string
---@field commandString string
cli.Interpreter = class("Interpreter")

function cli.Interpreter:initialize()
    self.prompt = "> "
end

---@param str string The string to search
---@param start string The string to find
---@return boolean @true if `str` starts with `start`
local function starts_with(str, start)
    return str:sub(1, #start) == start
end

function cli.Interpreter:run()
    local running = true
    while running == true do
        cli.print(self.prompt, 15)
        self.commandString = ""
        while kb.peekch() ~= '\x0d' do
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
        if self.commandString == "help" then
            self.currentCommand = cli.Help:new()
            self.currentCommand:run({})
        elseif self.commandString == "exit" then
            running = false
        end
    end
end

return cli
