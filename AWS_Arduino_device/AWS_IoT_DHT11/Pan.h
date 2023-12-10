#include <Arduino.h>

#define PAN_OFF 0
#define PAN_ON 1

class Pan {
  private:
    int relay;
    byte state;

  public:
    Pan(int relay);
    void init();
    void on();
    void off();
    byte getState();
};
