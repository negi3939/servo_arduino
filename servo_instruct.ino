#include <Servo.h>
#include <TimerThree.h>
#include "ds3218mg.hpp"

int pwm;
int sign;
uint8_t sendbyte[5];
uint8_t recvbyte[5];
uint8_t crc8ccit_table[256];

ds3218mg myServo;

void gen_crc8ccit_table(){
    uint8_t crc8ccit_poly = 0x8D;
    uint8_t val;
    
    for(uint16_t cnt = 0;cnt < 256;cnt++){
        val = (uint8_t)cnt;
        for(uint16_t cnt2 = 0;cnt2 < 8;cnt2++){
            if(val & 0x80){
                val <<= 1;
                val ^= crc8ccit_poly;
            }else{
                val <<= 1;
            }
        }
        crc8ccit_table[cnt] = val;
    }
}

uint8_t calc_crc8ccit(uint8_t *data,uint8_t len){
    uint8_t crc8ccit = 0x00;
    
    for(uint16_t cnt = 0;cnt < len;cnt++){
        crc8ccit = crc8ccit_table[crc8ccit ^ data[cnt]];
    }
    return crc8ccit;
}

void serialcom() {
 uint8_t l_recv[5];
 uint16_t buff;
 int avebyte = Serial.available();
 if(avebyte>4){
  for(int ii=0;ii<5;ii++){
    l_recv[ii]=Serial.read();
  }
  if(l_recv[4] == calc_crc8ccit(sendbyte,4)){
    for(int ii=0;ii<5;ii++){
      recvbyte[ii] = l_recv[ii];
    }
  }
  for(int ii=avebyte;ii>5;ii--){
    buff = Serial.read();
  }
  
 }
  sendbyte[3] += 0x01;
  sendbyte[4] = calc_crc8ccit(sendbyte,4);
  for(int ii=0; ii<5;ii++){
    Serial.write(sendbyte[ii]);
  }
}

void setup() {
  pwm = 0;
  sign=0;
  pinMode(13,OUTPUT);
  myServo.attach(13);
  gen_crc8ccit_table();
  Serial.begin(115200);
  sendbyte[0] = 0x0A;
  sendbyte[1] = 0x01;
  sendbyte[2] = 0x02;
  sendbyte[3] = 0x00;
  for(int ii=0;ii<5;ii++){
      recvbyte[ii] = 0x00;
  }
  Timer3.initialize(10*1000);
  Timer3.attachInterrupt(serialcom);
}
void loop() {
  pwm = 0;
  myServo.write(pwm);
  long c=0;
  while(c<10000){
    pwm = 90;
    myServo.write(pwm);
    c++;
  }
  c=0;
  while(c<10000){
    pwm = -90;
    myServo.write(pwm);
    c++;
  }
  pwm = 0;
  myServo.write(pwm);
}
