local class = require("middleclass")
local cli = require("cli/base")
local Command = require("cli/command")

---@class Help : Command
local Help = class("Help", Command)

function Help:initialize()
    Command.initialize(self)
end

---@type integer
--- Enabled command color
Help.static.ENABLED_COLOR = 15
---@type integer
--- Disabled command color
Help.static.DISABLED_COLOR = 8

function Help:run(args)
    -- Ingore args
    args = args

    cli.print("List of commands:\n")
    cli.print("- call <script> [<args>...]\n", Help.ENABLED_COLOR)
    cli.print("- cd [<dir>|--reset|-r]\n", Help.ENABLED_COLOR)
    cli.print("- cls\n", Help.ENABLED_COLOR)
    cli.print("- del/rm <file>|(--dir|-d) <directory>\n", Help.ENABLED_COLOR)
    cli.print("- dir/ls\n", Help.ENABLED_COLOR)
    cli.print("- exit\n", Help.ENABLED_COLOR)
    cli.print("- help\n", Help.ENABLED_COLOR)
    cli.print("- load <cart>\n", Help.DISABLED_COLOR)
    cli.print("- mkdir/md <dir>\n", Help.ENABLED_COLOR)
    cli.print("- new\n", Help.DISABLED_COLOR)
    cli.print("- run\n", Help.DISABLED_COLOR)
    cli.print("- save [cart]\n", Help.DISABLED_COLOR)
    cli.print("- unload\n", Help.DISABLED_COLOR)

    return true
end

return Help
