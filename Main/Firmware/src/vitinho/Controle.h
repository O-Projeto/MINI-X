#include <IRremote.hpp> // include the library

#pragma once

class Controle{
  private:
    int pin;
    int button_read;
  public:
    Controle();
    Controle(int pin);
    // int pin();
    void iniciar();
    int ler();
    
};

inline Controle::Controle()
{
}

Controle::Controle(int PIN)
{
    pin = PIN;
}

void Controle::iniciar(){
  IrReceiver.begin(pin);
}

int Controle::ler(){
  if (IrReceiver.decode()) {
    IrReceiver.resume(); 
    button_read = IrReceiver.decodedIRData.command; 
    // Serial.println(IrReceiver.decodedIRData.command);  //Print do valor do controle
  }
  return button_read;
}