//振動センサを接続するピン
//A0ピンに接続します
const int sPin = A0;

void setup() {
  //9600bpmでシリアル通信
  Serial.begin(9600);
}

void loop() {
  //振動センサの値を取得
  int sValue = analogRead(sPin);

  //シリアルモニタに振動センサからの値を表示
  Serial.println(sValue);

 delay(750);
}
