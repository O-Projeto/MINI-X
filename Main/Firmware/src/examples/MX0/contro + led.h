#include "controle_juiz.h"
#include "led_rgb.h"

class Juiz {
  private:

    int _pincontrole; // Pino do sensor controle
    int _pinled; // Pino dos LEDs
    int _numled; // Número de LEDs
    int _numcores; // Quantidade de cores
    int _tempo; // Tempo de piscar
    int _paremetro = -1;
    int _control_read = -1;
    controle_juiz contro;
    led_rgb led1;

  public:
    Juiz(int pincontrole, int pinled, int numled, int numcores, int tempo):contro(pincontrole),led1(pinled, 4, 4,tempo){
        _pincontrole = pincontrole;
        _pinled = pinled;
        _numled = numled;
        _numcores = numcores;
        _tempo = tempo;
    }
    void init();
    int comando();
};


void Juiz::init()
{
    contro.init();
    led1.init();
}

int Juiz::comando(){
  _control_read = contro.read();
  Serial.print("cr:  ");
  Serial.print(_control_read);
  if (_control_read == 0){
    if (_paremetro == -1){
      led1.blink(_tempo);
      Serial.print(" PB: ");
      Serial.print(_paremetro);
      return 1;
    }
    else {
      return 2;
    }
  }

  if (_control_read == 1){
    led1.parametro_pisca = 1;
    led1.green();
    _paremetro = 1;
    Serial.print(" PR:");
    Serial.print(_paremetro);
    return 2;
  }

  if (_control_read == 2){
    led1.red();
    _paremetro = -1;
    Serial.print(" PG:");
    Serial.print(_paremetro);
    return 3;
  }
}