#include "machine.hpp"
#include "base.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sol/sol.hpp>

namespace pxly {

    Machine * Machine::Instance = nullptr;

    Machine::Machine() : m_Win{}, m_RTex{}, m_Running{false}, m_ExitStatus{0}, m_Lua{} {}

    Machine::~Machine() {}

    bool Machine::Initialize() {
        m_Win.create(sf::VideoMode(1152, 648),
                     "Pixelly",
                     sf::Style::Close | sf::Style::Resize | sf::Style::Titlebar,
                     sf::ContextSettings(0, 0, 0, 3, 2));
        pxly_assert(m_RTex.create(GpuPixelWidth, GpuPixelHeight, sf::ContextSettings(0, 0, 0, 3, 2)),
                    "Failed to create Render Texture");
        pxly_assert(m_Font.loadFromFile("data/fonts/unscii-8-mod.otf"),
                    "Unable to load font 'data/fonts/unscii-8-mod.otf'");
        pxly_assert(m_Gpu.Initialize(), "Failed to initialize Pixelly GPU");
        pxly_assert(m_Kb.Initialize(), "Failed to initialize Pixelly Keyboard");
        pxly_assert(m_Gp.Initialize(), "Failed to initialize Pixelly GamePad");
        pxly_assert(m_Mouse.Initialize(), "Failed to initialize Pixelly Mouse");
        m_IKb.m_CharPresses.clear();

        m_Lua = sol::state();

        registerLuaAPI();

        m_Running    = true;
        m_ExitStatus = 0;
        Instance     = this;

        sol::load_result script = m_Lua.load_file("data/scripts/lua/boot.lua");
        if (!script.valid()) {
            sol::error err = script;
            pxly_assert(
              false, fmt::format("Failed to load boot script 'data/scripts/lua/boot.lua', with error: {}", err.what()));
        }

        m_Cwd = std::filesystem::weakly_canonical((std::filesystem::path(pxly::get_home_data_path()) / "storage"));
        if (!std::filesystem::exists(m_Cwd)) {
            try {
                std::filesystem::create_directories(m_Cwd);
            } catch (std::filesystem::filesystem_error & e) {
                pxly_assert(
                  false,
                  fmt::format("Failed to create storage directory '{}', exception: {}", m_Cwd.string(), e.what()));
            }
            // pxly_assert(std::filesystem::create_directory(m_Cwd),
            //             fmt::format("Failed to create storage directory '{}'", m_Cwd.string()));
        }

        m_Script   = script;
        m_InScript = false;

        return true;
    }

    bool Machine::Run() {
        if (!m_InScript && m_Script.valid()) {
            m_InScript                            = true;
            sol::protected_function_result result = m_Script();
            if (!result.valid()) {
                sol::error err = result;
                pxly_assert(false, fmt::format("Execution of boot script failed: {}", err.what()));
            }
            //m_Script = nullptr;
            m_Script.abandon();
            m_InScript = false;
            m_Running  = false;
        }

        while (m_IKb.m_CharPresses.size() > 32) { m_IKb.m_CharPresses.pop_front(); }
        m_Kb.StartFrame();
        m_Gp.StartFrame();
        m_Mouse.StartFrame();

        sf::Event event;
        while (m_Win.pollEvent(event)) {
            m_Kb.HandleEvent(&event);
            m_Gp.HandleEvent(&event);
            m_Mouse.HandleEvent(&event);
            switch (event.type) {
            case sf::Event::Closed: return false; break;
            case sf::Event::TextEntered:
                //fmt::print(std::cout, "MACHINE INPUT: {}, {:x}\n", static_cast<char>(event.text.unicode), event.text.unicode);
                std::cout.flush();
                m_IKb.m_CharPresses.push_back(event.text.unicode);
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                case sf::Keyboard::Escape: return false; break;
                default: break;
                }
                break;
            default: break;
            }
        }

        Update();

        if (!m_Running) { return false; }

        Render();

        return true;
    }

    void Machine::Update() {
        // Nothing just yet.
    }

    void Machine::Render() {
        m_Win.clear(sf::Color(0x202020FF));
        m_Gpu.Render(this);
        m_Win.display();
    }

    void Machine::synchronize() {
        static sf::Clock clock{};
        if (clock.getElapsedTime().asMicroseconds() >= 16667) {
            Run();
            clock.restart();
        }
    }

    std::string Machine::Cwd() const { return m_Cwd.string(); }

    bool Machine::Cwd(const std::string & path) {
        auto p = std::filesystem::path(path);

        if (p.is_absolute()) {
            p = std::filesystem::weakly_canonical(p);
            if (!std::filesystem::is_directory(p)) { return false; }
            m_Cwd = p;
            return true;
        } else if (p.is_relative()) {
            p = m_Cwd / p;
            p = std::filesystem::weakly_canonical(p);
            if (!std::filesystem::is_directory(p)) { return false; }
            m_Cwd = p;
            return true;
        }

        return false;
    }

} // namespace pxly
