#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Unicode.hpp>
#include <stdio.h>

Screen1View::Screen1View()
:uiStartAddr(0), uiQty(1), uiFunc(3)
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    refreshRequestFields();

    presenter->SetModbusRequest(uiFunc,uiStartAddr, uiQty);
}

void Screen1View::refreshRequestFields()
{
	Unicode::snprintf(Address_DisplayBuffer, 6, "%05u", uiStartAddr);
	Unicode::snprintf(Quantity_DisplayBuffer, 4, "%03u", uiQty);
	Unicode::snprintf(Function_TypeBuffer, 3, "%02", uiFunc);

	Address_Display.invalidate();
	Quantity_Display.invalidate();
	Function_Type.invalidate();
}
void Screen1View::updateFromSnapshot(const ModbusUiSnapshot& snap)
{
	refreshRegsText(snap);
}

void Screen1View::refreshRegsText(const ModbusUiSnapshot& snap)
{
	//building the header of the registers box

	//using a small ASCII temp and convert to unicode at the end
	char tmp[512];
	int pos = 0;

	const char* status = snap.result ? "OK": "ERR";

	pos+= snprintf(&tmp[pos], sizeof(tmp)-pos, "Registers:start= %05u qty= %03u %s exc= %u",
					snap.startAddr, snap.regCount, status, snap.exceptionCode);

	//converting to unicode buffer
	Unicode::strncpy(Register_DumpBuffer, tmp, sizeof(Register_DumpBuffer)/sizeof(Register_DumpBuffer[0]));

	Register_Dump.invalidate();


	//setting up the text box with the registers in it.

	uint16_t pos2 = 0;

	for (uint16_t i = 0; i < snap.regCount && pos2 < DISPLAY_REGISTERS_SIZE - 1; i++)
	{
	    uint16_t before = pos2;

	    Unicode::snprintf(&Display_RegistersBuffer[pos2],
	                      DISPLAY_REGISTERS_SIZE - pos2,
	                      "[%02u] %02u ",
	                      i,
	                      snap.regs[i]);

	    pos2 += Unicode::strlen(&Display_RegistersBuffer[before]);
	}

	Display_Registers.invalidate();

}

void Screen1View::Address_plus_clicked()
{
	if(uiStartAddr < 65535) uiStartAddr++;
	refreshRequestFields();
	presenter->SetModbusRequest(uiFunc,uiStartAddr, uiQty);
}

void Screen1View::Address_minus_Clicked()
{
	if(uiStartAddr > 0) uiStartAddr--;
	refreshRequestFields();
	presenter->SetModbusRequest(uiFunc,uiStartAddr, uiQty);
}

void Screen1View::qty_plus_clicked()
{
	if(uiQty<MB_UI_MAX_REGS) uiQty++;
	refreshRequestFields();
	presenter->SetModbusRequest(uiFunc,uiStartAddr, uiQty);
}

void Screen1View::qty_minus_clicked()
{
	if(uiQty>1) uiQty--;
	refreshRequestFields();
	presenter->SetModbusRequest(uiFunc,uiStartAddr, uiQty);
}

void Screen1View::func_plus_clicked()
{
	if(uiFunc == 3) uiFunc = 5;
	if(uiFunc == 5) uiFunc =10;
	refreshRequestFields();
	presenter->SetModbusRequest(uiFunc,uiStartAddr, uiQty);
}

void Screen1View::func_minus_clicked()
{
	if(uiFunc == 5) uiFunc = 3;
	if(uiFunc == 10) uiFunc = 5;
	refreshRequestFields();
	presenter->SetModbusRequest(uiFunc, uiStartAddr, uiQty);
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::SendButtonPressed()
{
	presenter->SetModbusRequest(uiFunc, uiStartAddr, uiQty);
	presenter->sendButtonClicked();
}

