# 自主プロ JisaQoobo
<img src="https://user-images.githubusercontent.com/67531577/106209150-dd2dee00-6207-11eb-8466-f9a885bbe399.JPG" width=50%>

## 使用環境
- Ubuntu 18.04
- Python 2.7
- Arduino IDE 1.8.13
- Arduino UNO
- ROS Melodic

## 必要な環境の導入
- Julius ROS

`$ sudo apt install ros-melodic-julius-ros`

- ros speech recognition

`$ sudo apt install ros-melodic-ros-speech-recognition`

## 起動方法
1. move_tail.inoをArduinoに書き込む

2. 音声認識ノードを起ち上げる

`$ roslaunch ros_speech_recognition speech_recognition.launch language:=ja-JP device:=hw:0,0 sample_rate:=44100 n_channel:=2 launch_sound_play:=false continuous:=true`

or

`$ roslaunch julius_ros julius.launch julius_output:=screen julius_args:="-debug"`

3. Rosserialを介した通信を開始する


`$ roslaunch mechatrobot mechatrobot_driver.launch`

>[jsk-enshu/robot-programming](https://github.com/jsk-enshu/robot-programming.git)内のmechatrobotパッケージを利用


4. 必要に応じて音声認識操作プログラムを起動する

`$ python speech_listener.py`

## 動作結果
以下のプレイリストにまとめてある

[https://youtube.com/playlist?list=PLnE36RnaDnLsUefg1Ihpq5fz-qoc_AKe0](https://youtube.com/playlist?list=PLnE36RnaDnLsUefg1Ihpq5fz-qoc_AKe0)
