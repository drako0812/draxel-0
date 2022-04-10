-- This script set's up the user functionality of Pixelly. It is run in app mode, not cart mode.

-- Set up package path
local package_path = package.path
if package_path ~= "" then
    package_path = package_path .. ";"
end
package_path = package_path .. "data/scripts/lua/?.lua;data/scripts/lua/?/init.lua;data/scripts/lua/pxly/?.lua;data/scripts/lua/pxly/?/init.lua;data/scripts/lua/libs/?.lua;data/scripts/lua/libs/?/init.lua"
package.path = package_path
local cli = require("cli")

local color_ramp = { 9, 10, 11, 12, 13, 14, 15 }
local function print_rainbow(text, index)
    local i = index % #color_ramp
    local col = color_ramp[i+1]

    for ii = 1, #text do
        cli.printc(string.sub(text, ii), col)
        i = (i + 1) % #color_ramp
        col = color_ramp[i+1]
    end
end

--- Entry point of the boot code.
function boot_main()
    gpu.text.csren(false)
    cli.cls()
    print_rainbow(">>>===[ PIXELLY ]===<<<\n", 0)
    print_rainbow("version 0.1.0\n", 1)
    print_rainbow("https://github.com/drako0812/Pixelly\n", 2)
    cli.wait(2.0)
    gpu.text.csren(true)

    local interpreter = cli.Interpreter:new()
    interpreter:run()
end

boot_main()
