#include "Pan.h"

Pan::Pan(int relay) {
  // Use 'this->' to make the difference between the
  // 'pin' attribute of the class and the 
  // local variable 'pin' created from the parameter.
  this->relay = relay;
  init();
}
void Pan::init() {
  pinMode(relay, OUTPUT);
  // Always try to avoid duplicate code.
  // Instead of writing digitalWrite(pin, LOW) here,
  // call the function off() which already does that
  on();
  state = PAN_ON;
}
void Pan::on() {
  digitalWrite(relay, HIGH);
  state = PAN_ON;
}
void Pan::off() {
  digitalWrite(relay, LOW);
  state = PAN_OFF;
}

byte Pan::getState() {
  return state;
}
