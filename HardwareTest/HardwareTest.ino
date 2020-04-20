#include <SoftwareSerial.h>
#include <Wire.h>

#define MC3413_ADDRESS 0x4C
void MC3413_init();
int getValues(unsigned char);
void MC3413_I2C_SEND(unsigned char, unsigned char);

const char colorR_Pin =  9;
const char colorG_Pin =  10;
const char colorB_Pin =  8;
const char SysTX_Pin =  6;
const char SysRX_Pin =  7;
const char PB_Pin =  3;
const char WiFiVcc_Pin = 5;
const char WiFiEN_Pin = 17;
const char WiFiRes_Pin = 16;
//const char BatVolt_Pin = A0;

SoftwareSerial SysSerial(SysRX_Pin, SysTX_Pin);      // System Serial RX, TX

void setup() {
  SysSerial.begin(57600);
  SysSerial.println("");
  SysSerial.println("System Boot -> OK");
  Serial.begin(9600);                                  // WiFi Serial
  //Serial.begin(74880);                                  // WiFi Serial display WiFi-module information
  
  pinMode(colorR_Pin, OUTPUT);
  pinMode(colorG_Pin, OUTPUT);
  pinMode(colorB_Pin, OUTPUT);
  pinMode(WiFiVcc_Pin, OUTPUT);
  pinMode(WiFiEN_Pin, OUTPUT);
  pinMode(WiFiRes_Pin, OUTPUT);
  analogReference(EXTERNAL);
  pinMode(A0, INPUT);

  // G-Sensor 初始化, 初始化失敗會卡死
  Wire.begin();
  Wire.setClock(50000);                     // I2C 設為50KHz
  MC3413_init();
  SysSerial.println("MC3413 Initail -> OK");

  // RGB LED 測試 
  digitalWrite(colorR_Pin, HIGH);
  delay(500);
  digitalWrite(colorR_Pin, LOW);
  digitalWrite(colorG_Pin, HIGH);
  delay(500);
  digitalWrite(colorG_Pin, LOW);
  digitalWrite(colorB_Pin, HIGH);
  delay(500);
  digitalWrite(colorB_Pin, LOW);
  
  //WiFi Module 初始化, 
  digitalWrite(WiFiEN_Pin, LOW);      // WiFi 除能
  digitalWrite(WiFiRes_Pin, LOW);     // WiFi Reset 拉低
  digitalWrite(WiFiVcc_Pin, HIGH);    // WiFi 斷電
  delay(50);
  
  digitalWrite(WiFiVcc_Pin, LOW);    // WiFi 上電
  //delay(10);                         // Datasheet 指示上電後要Delay一小段時間
  digitalWrite(WiFiEN_Pin, HIGH);    // WiFi 致能
  digitalWrite(WiFiRes_Pin, HIGH);   // WiFi Reset 拉高完成
  delay(10);
}

void loop() {

    // G-Sensor 測試, Serial輸出結果
    /*int X,Y,Z;
    X = (getValues(0x0E)*256) + (getValues(0x0D));
    Y = (getValues(0x10)*256) + (getValues(0x0F));
    Z = (getValues(0x12)*256) + (getValues(0x11));
    SysSerial.print("X:");
    SysSerial.print(X);
    SysSerial.print(", Y:");
    SysSerial.print(Y);
    SysSerial.print(", Z:");
    SysSerial.println(Z);
    delay(500);*/

    // 測試按鍵, 按下放開後紅色LED閃一下 並丟"AT"給WiFi Module
    if(digitalRead(PB_Pin)==HIGH){
      while(digitalRead(PB_Pin)==HIGH);
      digitalWrite(colorR_Pin, HIGH);
      delay(200);
      digitalWrite(colorR_Pin, LOW);
      digitalWrite(colorG_Pin, HIGH);
      delay(200);
      digitalWrite(colorG_Pin, LOW);
      digitalWrite(colorB_Pin, HIGH);
      delay(200);
      digitalWrite(colorB_Pin, LOW);
      
      int X,Y,Z;
      X = (getValues(0x0E)*256) + (getValues(0x0D));
      Y = (getValues(0x10)*256) + (getValues(0x0F));
      Z = (getValues(0x12)*256) + (getValues(0x11));
      SysSerial.print("X:");
      SysSerial.print(X);
      SysSerial.print(", Y:");
      SysSerial.print(Y);
      SysSerial.print(", Z:");
      SysSerial.println(Z);
    
      // 測試ADC, 顯示電池電壓
      double anaVal = analogRead(A0);
      anaVal = (anaVal*12)/1024;
      anaVal = (anaVal*15)/12/10;
      SysSerial.print("Bat: ");
      SysSerial.print(anaVal);
      SysSerial.print("V / ");
      SysSerial.print((anaVal/1.5)*100);
      SysSerial.println("%");
    
      //Serial.print("AT\r\n");
      Serial.print("AT+GMR\r\n");
      //Serial.print("AT+UART=9600,8,1,0,0\r\n");
      
    }

    // 測試ADC, 顯示電池電壓
    /*double anaVal = analogRead(A0);
    anaVal = (anaVal*12)/1024;
    anaVal = (anaVal*15)/12/10;
    SysSerial.print("Bat: ");
    SysSerial.print(anaVal);
    SysSerial.print("V / ");
    SysSerial.print((anaVal/1.5)*100);
    SysSerial.println("%");
    delay(500);*/
    
    // 顯示WiFi Module 輸出資料
    if (Serial.available()) {
      SysSerial.write(Serial.read());
    }
    if (SysSerial.available()) {
      Serial.write(SysSerial.read());
    }
}

void MC3413_init()
{
  MC3413_I2C_SEND(0x07,0x00);
  MC3413_I2C_SEND(0x08,0x0A);
  MC3413_I2C_SEND(0x20,0x02);
  MC3413_I2C_SEND(0x07,0x01);
}

int getValues(unsigned char REG_ADDRESS)
{
  int DATA=0;
  Wire.beginTransmission(MC3413_ADDRESS);
  Wire.write(REG_ADDRESS);  // register to read
  Wire.endTransmission();
  Wire.requestFrom(MC3413_ADDRESS, 1); // read a byte
  if(Wire.available()){
    DATA = Wire.read();
  }
  return(DATA);
}

void MC3413_I2C_SEND(unsigned char REG_ADDRESS, unsigned  char DATA){
  Wire.beginTransmission(MC3413_ADDRESS);
  Wire.write(REG_ADDRESS);
  Wire.write(DATA);
  Wire.endTransmission();
}

