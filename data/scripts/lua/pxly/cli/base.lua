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

return cli
