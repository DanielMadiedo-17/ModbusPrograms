
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/model/Model.hpp>

Screen1Presenter::Screen1Presenter(Screen1View& v) : view(v) {}

void Screen1Presenter::SetModbusRequest(uint16_t func, uint16_t startAddr, uint16_t qty)
{
    if (model) model->SetModbusRequest(func, startAddr, qty);
}

void Screen1Presenter::sendButtonClicked()
{
    if (model) model->triggerSend();
}

void Screen1Presenter::onModbusUpdate(const ModbusUiSnapshot& snap)
{
    view.updateFromSnapshot(snap);
}
