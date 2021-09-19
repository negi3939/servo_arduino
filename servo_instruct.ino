#include <Servo.h>
#include <TimerThree.h>
#include "ds3218mg.hpp"

int t_angle,pre_angle;
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
 uint8_t *l_recv;
 int avebyte = Serial.available();
 l_recv = new uint8_t[avebyte];
 for(int ii=0;ii<avebyte;ii++){
   l_recv[ii]=Serial.read();
 }
 for(int jj=0;jj<(avebyte-4);jj++){
   if(l_recv[avebyte-1-jj] == calc_crc8ccit(l_recv+avebyte-5-jj,4)){
     sendbyte[3] += 0x01;
     for(int ii=0;ii<5;ii++){
       recvbyte[ii] = l_recv[avebyte-5-jj+ii];
     }
     break;
    }
   }
  sendbyte[1] = recvbyte[1];
  sendbyte[2] = recvbyte[2];
  sendbyte[4] = calc_crc8ccit(sendbyte,4);
  for(int ii=0; ii<5;ii++){
    Serial.write(sendbyte[ii]);
  }
  t_angle = (int)((int16_t)((recvbyte[1] << 8) + recvbyte[2]));

  delete[] l_recv;
}

void setup() {
  t_angle = 0;
  sign=0;
  pinMode(13,OUTPUT);
  myServo.attach(13);
  gen_crc8ccit_table();
  Serial.setTimeout(5);
  Serial.begin(115200);
  sendbyte[0] = 0x0A;
  sendbyte[1] = 0x01;
  sendbyte[2] = 0x02;
  sendbyte[3] = 0x00;
  for(int ii=0;ii<5;ii++){
      recvbyte[ii] = 0x00;
  }
  Timer3.initialize(5*1000);
  Timer3.attachInterrupt(serialcom);
}
void loop() {
  myServo.write(t_angle);
}
