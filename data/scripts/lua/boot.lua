-- This script set's up the user functionality of Pixelly. It is run in app mode, not cart mode.

gpu.text.cls(15,0,' ')
gpu.text.puts(0,0,15,0,"Booting Pixelly...")
gpu.text.puts(0,1,15,0,"Done.")

local i = 0
while not kb.keyp(36) do
    gpu.text.puts(0,2,11,0,"                ")
    gpu.text.puts(0,2,11,0,tostring(i))
    i = i + 1
end
