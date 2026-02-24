#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <gui/model/ModbusUiSnapshot.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    //These update the wildcard text+invalidate
    //void setStartAddr(uint16_t addr);
    //void setQty(uint16_t qty);

    //called by presenter when model has new modbus snap
    //void updateModbus(const ModbusUiSnapshot& snap);
    void updateFromSnapshot(const ModbusUiSnapshot& snap);

    //these are the virtual callbacks for buttons
    virtual void Address_plus_clicked();
    virtual void Address_minus_Clicked();
    virtual void qty_plus_clicked();
    virtual void qty_minus_clicked();
    virtual void SendButtonPressed();
    virtual void func_plus_clicked();
    virtual void func_minus_clicked();
protected:
    //helper for formatting
   //void buildDumpText(const ModbusUiSnapshot& snap);
private:
    void refreshRequestFields();
    void refreshRegsText(const ModbusUiSnapshot& snap);

    uint16_t uiStartAddr;
    uint16_t uiQty;
    uint16_t uiFunc;

    //Wildcard Buffers (must match what designer expects
    touchgfx::Unicode::UnicodeChar startAddrBuffer[6]; // "00000" + null
    touchgfx::Unicode::UnicodeChar qtyBuffer[4];  //000 + null
    touchgfx::Unicode::UnicodeChar funcBuffer[3]; //00 + nul

    touchgfx::Unicode::UnicodeChar Reg_DumpBuffer;
};

#endif // SCREEN1VIEW_HPP
