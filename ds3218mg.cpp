#include <Servo.h>
#include "ds3218mg.hpp"

ds3218mg::ds3218mg(){}
ds3218mg::ds3218mg(unsigned int pin){
  m_Servo.attach(pin);
}

int ds3218mg::attach(unsigned int pin){
  m_Servo.attach(pin);
}

int ds3218mg::write(int val){
  double degree;
  degree = (double)val/270.0*180.0 + 90.0;
  m_Servo.write((int)degree);
}
