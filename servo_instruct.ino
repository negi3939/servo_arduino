#include <Servo.h>
#include <TimerThree.h>
#include "ds3218mg.hpp"
#define SENDBYTE (3)
#define RECVBYTE (16)
#define MOTORNUM (7)

int t_angle[MOTORNUM];
int sign;
uint8_t sendbyte[SENDBYTE];
uint8_t recvbyte[RECVBYTE];
uint8_t crc8ccit_table[256];
ds3218mg myServo[MOTORNUM];

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
 for(int jj=0;jj<(avebyte-(RECVBYTE-1));jj++){
   if(l_recv[avebyte-1-jj] == calc_crc8ccit(l_recv+avebyte-RECVBYTE-jj,RECVBYTE-1)){
     sendbyte[1] += 0x01;
     for(int ii=0;ii<RECVBYTE;ii++){
       recvbyte[ii] = l_recv[avebyte-RECVBYTE-jj+ii];
     }
     break;
    }
   }
  sendbyte[SENDBYTE-1] = calc_crc8ccit(sendbyte,SENDBYTE-1);
  for(int ii=0; ii<SENDBYTE;ii++){
    Serial.write(sendbyte[ii]);
  }
  for(int ii=0;ii<MOTORNUM;ii++){
    t_angle[ii] = (int)((int16_t)((recvbyte[2*ii+1] << 8) + recvbyte[2*ii+2]));
  }
  delete[] l_recv;
}

void setup() {
  t_angle[0] = 0;
  t_angle[1] = 0;
  sign=0;
  gen_crc8ccit_table();
  Serial.setTimeout(5);
  Serial.begin(115200);
  sendbyte[0] = 0x0A;
  sendbyte[1] = 0x01;
  for(int ii=0;ii<5;ii++){
      recvbyte[ii] = 0x00;
  }
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(6,OUTPUT);
  for(int ii=0;ii<MOTORNUM;ii++){
    myServo[ii].attach(12-ii);
  }
  Timer3.initialize(5*1000);
  Timer3.attachInterrupt(serialcom);
}
void loop() {
  for(int ii=0;ii<MOTORNUM;ii++){
    myServo[ii].write(t_angle[ii]);
  }
}
