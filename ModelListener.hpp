#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#pragma once
#include <gui/model/ModbusUiSnapshot.hpp>

class Model;

class ModelListener
{
public:
    ModelListener() : model(0) {}
    virtual ~ModelListener() {}

    void bind(Model* m) { model = m; }

    virtual void onModbusUpdate(const ModbusUiSnapshot& snap) {}

protected:
    Model* model;
};

#endif //MODELLISTENER_HPP
