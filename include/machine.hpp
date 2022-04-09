#pragma once

#include "base.hpp"
#include "gpu.hpp"
#include "input.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <sol/sol.hpp>

namespace pxly {

    struct Machine {
        // Internal Resources
        Gpu      m_Gpu;
        Keyboard m_Kb;

        // External Resources
        sf::RenderWindow  m_Win;
        sf::RenderTexture m_RTex;
        sf::Font          m_Font;
        bool       m_Running;
        int        m_ExitStatus;
        sol::state m_Lua;
        sol::protected_function m_Script;
        bool m_InScript;

        Machine();
        ~Machine();
        bool Initialize();
        bool Run();
        void Update();
        void Render();

        static Machine * Instance;

      protected:
        void synchronize();
        void registerLuaAPI();
    };

} // namespace pxly
