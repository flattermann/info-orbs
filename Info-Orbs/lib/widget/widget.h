#ifndef WIDGET_H
#define WIDGET_H

#include <screenManager.h>
#include <config.h>

class Widget {
public:
    Widget(ScreenManager& manager);
    virtual ~Widget() = default;
    virtual void setup() = 0;
    virtual void update(bool force = false) = 0;
    virtual void draw(bool force = false) = 0;
    virtual void changeMode() = 0;
    virtual void changeModeLongpress() = 0;
    void setBusy(bool busy);

protected:
    ScreenManager& m_manager;
};
#endif // WIDGET_H