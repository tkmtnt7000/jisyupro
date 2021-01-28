#include <Wire.h>
#include <VarSpeedServo.h>

VarSpeedServo myservo;
VarSpeedServo yourservo;

// デバイスアドレス(スレーブ)
uint8_t DEVICE_ADDRESS = 0x1D;  
 
// XYZレジスタ用のテーブル(6byte)
uint8_t RegTbl[6];  

// 一つ前の値を入れるためのグローバル変数
int16_t old_x = 0;
int16_t old_y = 0;
int16_t old_z = -9;
int16_t a = 0;

void setup() {
  // サーボモータのセットアップ
  myservo.attach(9);
  yourservo.attach(6);

  // 加速度センサのセットアップ
  // シリアル通信
  Serial.begin(9600); 
  
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
}
 
void loop() {
  
  sensor();
  
}

void sensor()
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

  // x,yの加速度変化の２乗ノルム
  int16_t a = 100*(pow(0.039226*(x-old_x), 2) + pow(0.039226*(y-old_y), 2));

  
  // 各XYZ軸の加速度(m/s^2)を出力する
  Serial.print("X : ");
  Serial.print( x * 0.0392266 );
  Serial.print(" Y : ");
  Serial.print( y * 0.0392266 );
  Serial.print(" Z : ");
  Serial.print( z * 0.0392266 );
  Serial.println(" m/s^2");
  Serial.println( a );

  // 一つ前の値を保持
  old_x = x;
  old_y = y;
  old_z = z;
  delay(100);
  
}

// 水平方向の移動
void horizontal_move(int left_speed, int right_speed){
  myservo.write(170, left_speed, true);
  myservo.write(5, right_speed, true);
}

// 上下方向の移動
void vertical_move(int up_speed, int down_speed){
  yourservo.write(170, down_speed, true);
  yourservo.write(5, up_speed, true);
}

//　特定の位置(角度)に尻尾を持っていく
void tail_position(int my_degree, int your_degree){
  myservo.write(my_degree, 80, true);
  yourservo.write(your_degree, 80, true);
}
