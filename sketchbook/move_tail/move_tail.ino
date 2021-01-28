
#include "Arduino.h"

// ROS
#include <ros.h>
ros::NodeHandle nh; // RAM容量確保のためにros.hを書き換えた
//typedef NodeHandle_<ArduinoHardware,10,15,100,256> NodeHandle;

#include <std_msgs/UInt16.h>

//#include <geometry_msgs/Accel.h>

// accelaration sensor
#include <Wire.h>
// Device Adress (slave)
int8_t DEVICE_ADDRESS = 0x1D;  
 
// XYZ register table(6byte)
uint8_t RegTbl[6];  

// servo motor
#include <VarSpeedServo.h> // サーボの速度制御が可能なライブラリ

VarSpeedServo h_servo;
VarSpeedServo v_servo;

#define H_PIN 9
#define V_PIN 6
#define DIN_PIN_LEFT 3
#define DIN_PIN_RIGHT 12
#define SOUND_PIN 10

// setup servo motor
void servo_setup(){
  h_servo.attach(H_PIN);
  v_servo.attach(V_PIN);
}

//geometry_msgs::Accel accel_msg; // これを用いるとRAMが足りない
std_msgs::UInt16 h_jerk_msg;
//std_msgs::UInt16 v_jerk_msg;

//std_msgs::UInt16 y_accel;
//std_msgs::UInt16 z_accel;
//std_msgs::Float32 jerk;

ros::Publisher h_jerk_pub("arduino/h_jerk", &h_jerk_msg);
//ros::Publisher v_jerk_pub("arduino/v_jerk", &v_jerk_msg);
//ros::Publisher y_pub("y_accel", &y_accel);
//ros::Publisher z_pub("z_accel", &z_accel);


// setup accelaration sensor
void accel_setup(){
  // シリアル通信
  //Serial.begin(9600); 
  
  // マスタとしてI2Cバスに接続する
  Wire.begin();       
 
  // DATA_FORMAT(データ形式の制御) 
  Wire.beginTransmission(DEVICE_ADDRESS);  
  // DATA_FORMATのアドレス    
  Wire.write(0x31);
  // 「最大分解能モード」 及び 「±16g」 (0x0B == 1011)
  Wire.write(0x0B);  
  // 「10bit固定分解能モード」 及び　「±16g」にする場合 (0x03 == 0011)
  // Wire.write(0x03);
  Wire.endTransmission();
 
  // POWER_TCL(節電機能の制御) 
  Wire.beginTransmission(DEVICE_ADDRESS);  
  // POWER_CTLのアドレス    
  Wire.write(0x2d);
  // 測定モードにする
  Wire.write(0x08);  
  Wire.endTransmission();

  nh.advertise(h_jerk_pub);
 // nh.advertise(v_jerk_pub);
}


// 一つ前の値を入れるためのグローバル変数
// 初期値は水平においた場合のおおよその値となる
int16_t old_accel[3] = {0, 0, -10}; // {x,y,z}

//int16_t old_x = 0;
//int16_t old_y = 0;
//int16_t old_z = -10;
int16_t h_jerk = 0; // 水平方向の加加速度の大きさ
int16_t v_jerk = 0; // 垂直方向の加加速度の大きさ


// 加速度センサから３軸の加速度の値を読む
void read_accel_sensor()
{
  // XYZの先頭アドレスに移動する
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x32);
  Wire.endTransmission();
  
  // デバイスへ6byteのレジスタデータを要求する
  Wire.requestFrom(DEVICE_ADDRESS, 6);
   
  // 6byteのデータを取得する
  int i;
  for (i=0; i< 6; i++){
    while (Wire.available() == 0 ){}
    RegTbl[i] = Wire.read();
  }
 
  // データを各XYZの値に変換する(LSB単位)
  int16_t x = (((int16_t)RegTbl[1]) << 8) | RegTbl[0];
  int16_t y = (((int16_t)RegTbl[3]) << 8) | RegTbl[2];
  int16_t z = (((int16_t)RegTbl[5]) << 8) | RegTbl[4];  

  // x,yの加速度変化の２乗ノルム(加加速度の大きさ)
  h_jerk = 100*(pow(0.039226*(x-old_accel[0]), 2) + pow(0.039226*(y-old_accel[1]), 2));
  h_jerk_msg.data = h_jerk;
  // z軸方向の加速度変化
  v_jerk = 100*(pow(0.039226*(z-old_accel[2]), 2));
  //v_jerk_msg.data = v_jerk;
  
  // Rosを入れるとシリアルモニタに表示はできない
  /*
  // 各XYZ軸の加速度(m/s^2)を出力する
  Serial.print("X : ");
  Serial.print( x * 0.0392266 );
  Serial.print(" Y : ");
  Serial.print( y * 0.0392266 );
  Serial.print(" Z : ");
  Serial.print( z * 0.0392266 );
  Serial.println(" m/s^2");
  */
  
  h_jerk_pub.publish(&h_jerk_msg);
  //v_jerk_pub.publish(&v_jerk_msg);  

  // 一つ前の値を保持
  old_accel[0] = x;
  old_accel[1] = y;
  old_accel[2] = z;

  // read sensor value every 1 sec
  delay(1000);
  
}

// 水平方向の移動(小)
void horizontal_move(int left_speed, int right_speed){
  h_servo.write(150, left_speed, true);
  h_servo.write(30, right_speed, true);
}

// 水平方向の移動(大)
void dynamic_horizontal_move(int left_speed, int right_speed){
  h_servo.write(180, left_speed, true);
  h_servo.write(5, right_speed, true);
}

// 垂直方向の移動(小)
void vertical_move(int up_speed, int down_speed){
  v_servo.write(150, down_speed, true);
  v_servo.write(30, up_speed, true);
}

// 垂直方向の移動(大)
void dynamic_vertical_move(int up_speed, int down_speed){
  v_servo.write(170, down_speed, true);
  v_servo.write(5, up_speed, true);
}

//　特定の位置(角度)に尻尾を持っていく
void tail_position(int horizontal_degree, int vertical_degree){
  h_servo.write(horizontal_degree, 80, false);
  v_servo.write(vertical_degree, 80, true);
}


void speech_cb(const std_msgs::UInt16& msg);
ros::Subscriber<std_msgs::UInt16> speech_sub("speech", speech_cb);
std_msgs::UInt16 speech_return_msg;
ros::Publisher speech_pub("arduino/speech_return", &speech_return_msg);
uint16_t speech_value = 10;

void speech_setup(){
  nh.subscribe(speech_sub);
  nh.advertise(speech_pub);
}

void speech_loop(){
  //if (speech_return_msg.data != speech_value){
    speech_return_msg.data = speech_value;  
    speech_pub.publish(&speech_return_msg);
  //}
}

void speech_cb(const std_msgs::UInt16& msg){
  speech_value = msg.data;
}

uint16_t pin_left = 0;
uint16_t pin_right = 0;

void pin_setup(){
  pinMode(DIN_PIN_LEFT, INPUT_PULLUP);
  pinMode(DIN_PIN_RIGHT, INPUT_PULLUP);
}

void pin_loop(){
  // スイッチ押すと1
  pin_left = digitalRead(DIN_PIN_LEFT);
  pin_right = digitalRead(DIN_PIN_RIGHT);
  //v_jerk_msg.data = pin_left;
  //v_jerk_pub.publish(&v_jerk_msg);  
}

void setup() {
  // setup ros functions  
  nh.initNode();

  // setup Servo motor
  servo_setup();

  // setup Accelaration sensor
  accel_setup();

  speech_setup();

  pin_setup();
}
 
void loop() {

  // 加速度センサの値を読む
  read_accel_sensor();
  
  // 音声認識ノード
  speech_loop();

  // スイッチの値を読む
  pin_loop();

  //tone(SOUND_PIN,262,500);
  // 初期位置に戻すときはスイッチ２つ押し込む
  if(pin_left==1 && pin_right==1){
    tail_position(90, 90);
    tone(SOUND_PIN,442,500);
  }

  // 左側を撫でると右側を擦るように動く
  if (pin_left==1 && pin_right==0) {
    tail_position(175,90);
    dynamic_vertical_move(60, 60);
  }

  // 右側を撫でると左側をさするように動く
  if (pin_right==1 && pin_left==0) {
    tail_position(5,90);
    dynamic_vertical_move(60, 60);
  }

  // "疲れた"と呼びかけ
  if (speech_value == 90) {
    tone(SOUND_PIN, 380, 500);
    tail_position(90, 100);
    tail_position(90,85);
    delay(500);
    int i = 0;
    for (i=0;i<=1;i++){
      horizontal_move(80, 80);
    }
    speech_value = 10;
  }
  
  // "Qoobo"と呼びかけ
  if (speech_value == 45) {
    
    tail_position(90, 90);
    tone(SOUND_PIN, 442, 400);
    tail_position(90, 85);
    int i = 0;
    for (i=0;i<=1;i++){
      dynamic_horizontal_move(40,40);
    }
    
    speech_value = 10;
  }

  
  if (speech_value == 60) {
    tail_position(90, 90);
    delay(1000);
    tone(SOUND_PIN, 380, 400);
    tail_position(90, 85);
    //dynamic_horizontal_move(25, 25);
    speech_value = 10;
  }
  
  // 弱くなでた時
  if(h_jerk>50 && h_jerk<=100){
    tail_position(90, 85); 
    horizontal_move(30, 30);
  }

  if(h_jerk>100 && h_jerk<=200){
    tail_position(90, 85); 
    dynamic_vertical_move(30, 30);
  }

  if(h_jerk>200 && h_jerk<=300){
    tail_position(90, 85); 
    dynamic_horizontal_move(40, 40);
  }
  
  if(h_jerk>300 && h_jerk<=500){
    tail_position(90, 85);
    int i = 0; 
    for(i=0;i<=1;i++){
      dynamic_horizontal_move(80, 80);
    }
  }
  
  // 少し強くなでた時、横方向に2往復する
  if(h_jerk>500 && h_jerk <=600){
    tail_position(90, 85);
    int i = 0;
    for (i=1; i<=2; i++){ 
      horizontal_move(60, 60);
    }
  }

  if(h_jerk>5000){
    tone(SOUND_PIN, 262, 500);
  }
  
  nh.spinOnce();
}
