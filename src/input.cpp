#include "input.hpp"
#include "machine.hpp"
#include <SFML/Window.hpp>
#include <cstring>
#include <fmt/format.h>

namespace drxl {

    bool getBit(int index, const uint8 * arr, int size) {
        auto byte_index = index / 8;
        auto bit_index  = index % 8;

        drxl_assert(byte_index < size, fmt::format("byte_index ({}) out of bounds, array size {}", byte_index, size));

        auto value = arr[byte_index];
        auto bit   = 1 & (value >> bit_index);
        return bit == 1;
    }

    void setBit(int index, uint8 * arr, int size, bool value) {
        auto byte_index = index / 8;
        auto bit_index  = index % 8;

        drxl_assert(byte_index < size, fmt::format("byte_index ({}) out of bounds, array size {}", byte_index, size));

        auto ovalue     = arr[byte_index];
        auto mask       = ~(1 << bit_index);
        ovalue          = (ovalue & mask) | ((value ? 1 : 0) << bit_index);
        arr[byte_index] = ovalue;
    }

    Keyboard::Keyboard() {}

    bool Keyboard::Initialize() {
        memset(m_KeyP, 0, sizeof(m_KeyP));
        memset(m_Key, 0, sizeof(m_Key));
        return true;
    }

    void Keyboard::StartFrame() { memcpy(m_KeyP, m_Key, sizeof(m_KeyP)); }

    void Keyboard::HandleEvent(void * evt) {
        sf::Event * ev = (sf::Event *)evt;
        switch (ev->type) {
        case sf::Event::KeyPressed: setBit(ev->key.code, m_Key, sizeof(m_Key), true); break;
        case sf::Event::KeyReleased: setBit(ev->key.code, m_Key, sizeof(m_Key), false); break;
        default: break;
        }
    }

    bool Keyboard::Key(int key) const { return getBit(key, m_Key, sizeof(m_Key)); }

    bool Keyboard::KeyP(int key) const { return getBit(key, m_KeyP, sizeof(m_KeyP)); }

    GamePad::GamePad() {}

    bool GamePad::Initialize() {
        memset(&m_GpP, 0, sizeof(m_GpP));
        memset(&m_Gp, 0, sizeof(m_Gp));
        return true;
    }

    void GamePad::StartFrame() { memcpy(&m_GpP, &m_Gp, sizeof(m_GpP)); }

    void GamePad::HandleEvent(void * evt) {
        sf::Event * ev = (sf::Event *)evt;
        switch (ev->type) {
        case sf::Event::KeyPressed:
            switch (ev->key.code) {
            case sf::Keyboard::Up: m_Gp.m_Up = true; break;
            case sf::Keyboard::Down: m_Gp.m_Down = true; break;
            case sf::Keyboard::Left: m_Gp.m_Left = true; break;
            case sf::Keyboard::Right: m_Gp.m_Right = true; break;
            case sf::Keyboard::Z: m_Gp.m_A = true; break;
            case sf::Keyboard::X: m_Gp.m_B = true; break;
            case sf::Keyboard::A: m_Gp.m_X = true; break;
            case sf::Keyboard::S: m_Gp.m_Y = true; break;
            case sf::Keyboard::Enter: m_Gp.m_Start = true; break;
            case sf::Keyboard::RShift: m_Gp.m_Select = true; break;
            default: break;
            }
            break;
        case sf::Event::KeyReleased:
            switch (ev->key.code) {
            case sf::Keyboard::Up: m_Gp.m_Up = false; break;
            case sf::Keyboard::Down: m_Gp.m_Down = false; break;
            case sf::Keyboard::Left: m_Gp.m_Left = false; break;
            case sf::Keyboard::Right: m_Gp.m_Right = false; break;
            case sf::Keyboard::Z: m_Gp.m_A = false; break;
            case sf::Keyboard::X: m_Gp.m_B = false; break;
            case sf::Keyboard::A: m_Gp.m_X = false; break;
            case sf::Keyboard::S: m_Gp.m_Y = false; break;
            case sf::Keyboard::Enter: m_Gp.m_Start = false; break;
            case sf::Keyboard::RShift: m_Gp.m_Select = false; break;
            default: break;
            }
            break;
        default: break;
        }
    }

    bool GamePad::Btn(int btn) const {
        switch (btn) {
        case 0: return m_Gp.m_Up; break;
        case 1: return m_Gp.m_Down; break;
        case 2: return m_Gp.m_Left; break;
        case 3: return m_Gp.m_Right; break;
        case 4: return m_Gp.m_A; break;
        case 5: return m_Gp.m_B; break;
        case 6: return m_Gp.m_X; break;
        case 7: return m_Gp.m_Y; break;
        case 8: return m_Gp.m_Start; break;
        case 9: return m_Gp.m_Select; break;
        default: return false;
        }
    }

    bool GamePad::BtnP(int btn) const {
        switch (btn) {
        case 0: return m_GpP.m_Up; break;
        case 1: return m_GpP.m_Down; break;
        case 2: return m_GpP.m_Left; break;
        case 3: return m_GpP.m_Right; break;
        case 4: return m_GpP.m_A; break;
        case 5: return m_GpP.m_B; break;
        case 6: return m_GpP.m_X; break;
        case 7: return m_GpP.m_Y; break;
        case 8: return m_GpP.m_Start; break;
        case 9: return m_GpP.m_Select; break;
        default: return false;
        }
    }

    Mouse::Mouse() {}

    bool Mouse::Initialize() {
        memset(&m_MouseP, 0, sizeof(m_MouseP));
        memset(&m_Mouse, 0, sizeof(m_Mouse));
        return true;
    }

    void Mouse::StartFrame() {
        memcpy(&m_MouseP, &m_Mouse, sizeof(m_MouseP));
        m_Mouse.m_Scroll = 0;
    }

    void Mouse::HandleEvent(void * evt) {
        sf::Event * ev = (sf::Event *)evt;
        switch (ev->type) {
        case sf::Event::MouseButtonPressed:
            switch (ev->mouseButton.button) {
            case sf::Mouse::Left: m_Mouse.m_Buttons.m_Left = true; break;
            case sf::Mouse::Right: m_Mouse.m_Buttons.m_Right = true; break;
            case sf::Mouse::Middle: m_Mouse.m_Buttons.m_Middle = true; break;
            case sf::Mouse::XButton1: m_Mouse.m_Buttons.m_X1 = true; break;
            case sf::Mouse::XButton2: m_Mouse.m_Buttons.m_X2 = true; break;
            default: break;
            }
            break;
        case sf::Event::MouseButtonReleased:
            switch (ev->mouseButton.button) {
            case sf::Mouse::Left: m_Mouse.m_Buttons.m_Left = false; break;
            case sf::Mouse::Right: m_Mouse.m_Buttons.m_Right = false; break;
            case sf::Mouse::Middle: m_Mouse.m_Buttons.m_Middle = false; break;
            case sf::Mouse::XButton1: m_Mouse.m_Buttons.m_X1 = false; break;
            case sf::Mouse::XButton2: m_Mouse.m_Buttons.m_X2 = false; break;
            default: break;
            }
            break;
        case sf::Event::MouseMoved:
            m_Mouse.m_X = ev->mouseMove.x;
            m_Mouse.m_Y = ev->mouseMove.y;
            break;
        case sf::Event::MouseWheelScrolled: {
            auto & scroll = ev->mouseWheelScroll;
            if (scroll.wheel == sf::Mouse::VerticalWheel) {
                float d = scroll.delta;
                if ((d < 0) && (d > -1.0f)) { d = -1.0f; }
                if ((d > 0) && (d < 1.0f)) { d = 1.0f; }
                m_Mouse.m_Scroll += d;
            }
            break;
        }
        default: break;
        }
    }

    int16 Mouse::MxP() const { return m_MouseP.m_X; }

    int16 Mouse::Mx() const { return m_Mouse.m_X; }

    int16 Mouse::MyP() const { return m_MouseP.m_Y; }

    int16 Mouse::My() const { return m_Mouse.m_Y; }

    bool Mouse::MBtnP(int btn) const {
        switch (btn) {
        case 0: return m_MouseP.m_Buttons.m_Left; break;
        case 1: return m_MouseP.m_Buttons.m_Right; break;
        case 2: return m_MouseP.m_Buttons.m_Middle; break;
        case 3: return m_MouseP.m_Buttons.m_X1; break;
        case 4: return m_MouseP.m_Buttons.m_X2; break;
        default: return false; break;
        }
    }

    bool Mouse::MBtn(int btn) const {
        switch (btn) {
        case 0: return m_Mouse.m_Buttons.m_Left; break;
        case 1: return m_Mouse.m_Buttons.m_Right; break;
        case 2: return m_Mouse.m_Buttons.m_Middle; break;
        case 3: return m_Mouse.m_Buttons.m_X1; break;
        case 4: return m_Mouse.m_Buttons.m_X2; break;
        default: return false; break;
        }
    }

    int8 Mouse::MScrollP() const { return m_MouseP.m_Scroll; }

    int8 Mouse::MScroll() const { return m_Mouse.m_Scroll; }

} // namespace drxl
