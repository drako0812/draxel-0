#pragma once

#include "base.hpp"
#include "gpu.hpp"
#include "input.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <sol/sol.hpp>
#include <filesystem>

namespace pxly {

    struct Machine {
        // Internal Resources
        Gpu      m_Gpu;
        Keyboard m_Kb;
        GamePad m_Gp;
        Mouse m_Mouse;

        // External Resources
        sf::RenderWindow  m_Win;
        sf::RenderTexture m_RTex;
        sf::Font          m_Font;
        bool       m_Running;
        int        m_ExitStatus;
        sol::state m_Lua;
        sol::protected_function m_Script;
        bool m_InScript;
        std::filesystem::path m_Cwd;
        InternalGpu m_IGpu;
        InternalKeyboard m_IKb;

        Machine();
        ~Machine();
        bool Initialize();
        bool Run();
        void Update();
        void Render();
        std::string Cwd() const;
        bool Cwd(const std::string & path);
        void synchronize();

        static Machine * Instance;

      protected:
        void registerLuaAPI();
    };

} // namespace pxly
