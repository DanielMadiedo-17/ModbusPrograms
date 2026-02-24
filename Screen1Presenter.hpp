#ifndef SCREEN1PRESENTER_HPP
#define SCREEN1PRESENTER_HPP

#pragma once

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen1View;

class Screen1Presenter : public Presenter, public ModelListener
{
public:
    Screen1Presenter(Screen1View& v);

    void activate() override {}
    void deactivate() override {}

    // View -> Presenter
    void SetModbusRequest(uint16_t func, uint16_t startAddr, uint16_t qty);
    void sendButtonClicked();

    // Model -> Presenter
    void onModbusUpdate(const ModbusUiSnapshot& snap) override;

private:
    Screen1View& view;
};

#endif //SCREEN1PRESENTER_HPP
