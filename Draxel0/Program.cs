using System;
using SFML;
using SFML.Graphics;
using SFML.Window;
using static Draxel.Utility;

namespace Draxel {
    static class Program {
        static void Main(string[] args) {
            unsafe {
                Console.WriteLine($"Size of Machine = {sizeof(Machine.Internal)}");
            }

            var machine = new Machine();
            Assert(machine.Initialize(), () => "Failed to create draxel-0 machine");
            while(machine.Run()) {}
        }
    }
}
