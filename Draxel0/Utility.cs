using System.Runtime.CompilerServices;
using SFML.System;

namespace Draxel {

    public static class Utility {
        public delegate string AssertFunction();
        public static void Assert(bool Condition, AssertFunction Func, [CallerMemberName] string callerName = "", [CallerFilePath] string callerFile = "", [CallerLineNumber] int callerLine = -1) {
            if(!Condition) {
                var emsg = $"{callerName} @\"{callerFile}\" #{callerLine}: {Func()}";
                throw new ApplicationException(emsg);
            }
        }

        public static Vector3f Cross(this Vector3f v1, Vector3f v2) {
            return new Vector3f(
                v1.Y * v2.Z - v1.Z * v2.Y,
                v1.Z * v2.X - v1.X * v2.Z,
                v1.X * v2.Y - v1.Y * v2.X
            );
        }

        public static Vector3f Barycentric(int x0, int y0, int x1, int y1, int x2, int y2, int Px, int Py) {
            var u = new Vector3f(x2 - x0, x1 - x0, x0 - Px).Cross(new Vector3f(y2 - y0, y1 - y0, y0 - Py));
            if(MathF.Abs(u.Z) < 1) { return new Vector3f(-1, 1, 1); }
            return new Vector3f(1 - (u.X + u.Y) / u.Z, u.Y / u.Z, u.X / u.Z);
        }
    }

}
