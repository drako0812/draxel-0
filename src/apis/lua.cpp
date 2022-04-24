#include "machine.hpp"
#include <filesystem>
#include <map>
#include <sol/sol.hpp>
#include <tuple>
#include <vector>

namespace drxl {

    void RegisterLuaAPI_gpu(Machine & machine);
    void RegisterLuaAPI_input(Machine & machine);
    void RegisterLuaAPI_intern(Machine & machine);

    void Machine::registerLuaAPI() {
        auto & lua = m_Lua;

        lua.open_libraries(sol::lib::base,
                           sol::lib::string,
                           sol::lib::math,
                           sol::lib::table,
                           sol::lib::bit32,
                           sol::lib::utf8,
                           sol::lib::os,
                           sol::lib::io,
                           sol::lib::package,
                           sol::lib::debug,
                           sol::lib::coroutine);

        lua.set_function("sync", [this]() { synchronize(); });
        lua.set_function("exec", [this, &lua](const std::string & path) {
            auto spath = std::filesystem::weakly_canonical((m_Cwd / std::filesystem::path(path)));
            if (std::filesystem::exists(spath)) {
                auto result = lua.do_file(spath.string());
                if (result.valid()) { return true; }
            }
            return false;
        });
        RegisterLuaAPI_intern(*this);
        RegisterLuaAPI_input(*this);
        RegisterLuaAPI_gpu(*this);
    }

} // namespace drxl
