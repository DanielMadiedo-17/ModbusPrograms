#ifndef MODEL_HPP
#define MODEL_HPP

#include <gui/model/ModbusUiSnapshot.hpp>

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }
    void tick();

    void SetModbusRequest(uint16_t func, uint16_t start, uint16_t qty);
    void triggerSend();

protected:
    ModelListener* modelListener;

private:
	uint32_t lastSeq;
};


#endif // MODEL_HPP
