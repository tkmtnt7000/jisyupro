# 自主プロ
## 使用環境
- Ubuntu 18.04
- Python 2.7
- Arduino IDE 1.8.13
- ROS Melodic

## 起動方法
1. move_tail.inoをArduinoに書き込む

2. 音声認識ノードを起ち上げる
`$ roslaunch ros_speech_recognition speech_recognition.launch language:=ja-JP device:=hw:0,0 sample_rate:=44100 n_channel:=2 launch_sound_play:=false continuous:=true'
or
'$ roslaunch julius_ros julius.launch julius_output:=screen julius_args:="-debug"'

3. Rosserialを介した通信を開始する
'$ roslaunch mechatrobot mechatrobot_driver.launch'

4. 必要に応じて音声認識操作プログラムを起動する
'$ python speech_listener.py'