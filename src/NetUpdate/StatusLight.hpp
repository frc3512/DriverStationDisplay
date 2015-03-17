// =============================================================================
// File Name: StatusLight.hpp
// Description: Shows green, yellow, or red circle depending on its internal
//             state
// Author: FRC Team 3512, Spartatroniks
// =============================================================================

#ifndef STATUS_LIGHT_HPP
#define STATUS_LIGHT_HPP

#include "Drawable.hpp"
#include "NetUpdate.hpp"
#include "Text.hpp"

#include <string>

class StatusLight : public Drawable, public NetUpdate {
public:
    enum Status {
        active,
        standby,
        inactive
    };

    StatusLight(std::wstring message, bool netUpdate,
                const QPoint& position = QPoint());

    void setActive(Status newStatus);
    Status getActive();

    void setPosition(const QPoint& position);
    void setPosition(short x, short y);

    void setString(const std::wstring& message);
    const std::wstring& getString();

    static void setColorBlind(bool on);
    static bool isColorBlind();

    void paintEvent(QPaintEvent* event);

    void updateValue();

private:
    Status m_status;

    Text m_text;

    static bool m_isColorBlind;
};

#endif // STATUS_LIGHT_HPP
