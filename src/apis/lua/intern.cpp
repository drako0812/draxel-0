#include "base.hpp"
#include "machine.hpp"
#include <filesystem>
#include <tuple>
#include <vector>


namespace pxly {

    void RegisterLuaAPI_intern(Machine & machine) {
        auto & lua        = machine.m_Lua;
        auto   mod_intern = lua.create_named_table("intern");
        mod_intern.set_function("get_cwd", [&machine]() {
            machine.synchronize();
            return machine.Cwd();
        });
        mod_intern.set_function("set_cwd", [&machine](const std::string & path) {
            machine.synchronize();
            return machine.Cwd(path);
        });
        mod_intern.set_function("reset_cwd", [&machine]() {
            machine.Cwd((std::filesystem::path(get_home_data_path()) / "storage").string());
            machine.synchronize();
        });
        mod_intern.set_function("get_dir_entries", [&machine]() -> sol::table {
            using namespace std::string_literals;
            sol::table rett = machine.m_Lua.create_table();
            //std::vector<std::pair<std::string, std::string>> ret = {};
            for (const auto & entry : std::filesystem::directory_iterator{machine.m_Cwd}) {
                if (entry.is_directory()) {
                    sol::table temp = machine.m_Lua.create_table();
                    temp.add("directory");
                    temp.add(entry.path().filename().string());
                    //temp.add((*(entry.path().end()--)).string());
                    rett.add(temp);
                    //ret.push_back({"directory"s, (*(entry.path().end()--)).string()});
                } else {
                    sol::table temp     = machine.m_Lua.create_table();
                    auto       filepath = entry.path();
                    if (filepath.has_extension()) {
                        if (filepath.extension() == ".lua") {
                            temp.add("lua");
                            temp.add(filepath.stem().string() + filepath.extension().string());
                            //ret.push_back({"lua"s, filepath.stem().string() + filepath.extension().string()});
                        } else if (filepath.extension() == ".pxly") {
                            temp.add("cart");
                            temp.add(filepath.stem().string() + filepath.extension().string());
                            //ret.push_back({"cart"s, filepath.stem().string() + filepath.extension().string()});
                        } else {
                            temp.add("generic");
                            temp.add(filepath.stem().string() + filepath.extension().string());
                            //ret.push_back({"generic"s, filepath.stem().string() + filepath.extension().string()});
                        }
                    } else {
                        temp.add("generic");
                        temp.add(filepath.stem().string());
                        //ret.push_back({"generic"s, filepath.stem().string()});
                    }
                    rett.add(temp);
                }
            }
            return rett;
        });
        mod_intern.set_function("mkdir", [&machine](const std::string & p) {
            std::filesystem::path path = std::filesystem::weakly_canonical(machine.m_Cwd / p);
            if (std::filesystem::exists(path)) { return false; }
            if (std::filesystem::create_directories(path)) {
                return true;
            } else {
                return false;
            }
        });
        mod_intern.set_function("delete_file", [&machine](const std::string & f) {
            std::filesystem::path path = std::filesystem::weakly_canonical(machine.m_Cwd / f);
            if (!std::filesystem::exists(path)) { return false; }
            if (!std::filesystem::is_regular_file(path)) { return false; }
            return std::filesystem::remove(path);
        });
        mod_intern.set_function("delete_dir", [&machine](const std::string & d) {
            std::filesystem::path path = std::filesystem::weakly_canonical(machine.m_Cwd / d);
            if (!std::filesystem::exists(path)) { return false; }
            if (!std::filesystem::is_directory(path)) { return false; }
            return std::filesystem::remove_all(path) > 0;
        });
    }

} // namespace pxly
