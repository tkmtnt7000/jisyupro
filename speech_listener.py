#!/usr/bin/env python
# coding=utf-8
import rospy
from std_msgs.msg import UInt16
from speech_recognition_msgs.msg import *
from speech_recognition_msgs.srv import *

def word_motor_cb(msg):
    pub = rospy.Publisher('speech', UInt16, queue_size=1)
    word_msg = UInt16()
    
    rospy.loginfo(msg.transcript[0])
    if msg.transcript[0] == 'Ok Google':
        word_msg.data = 0
        print "Ok Google"
        pub.publish(word_msg)
        
    elif msg.transcript[0] == 'Alexa':
        word_msg.data = 180
        print "Alexa"
        pub.publish(word_msg)
    elif '疲れた' in msg.transcript[0]:
        word_msg.data = 90
        print "疲れた"
        pub.publish(word_msg)
    elif '空母' or '久保'  in msg.transcript[0]:
        word_msg.data = 45
        print "Qoobo"
        pub.publish(word_msg)
    elif 'おはよう' in msg.transcript[0]:
        word_msg.data = 60
        print "おはよう"
        pub.publish(word_msg)
    
        
def main():
    rospy.init_node('speech_listener', anonymous=True)
    #rospy.Subscriber("/speech_to_text", SpeechRecognitionCandidates, word_motor_cb)
    #rospy.Subscriber("/arduino/v_jerk", UInt16, v_jerk_cb) 
    rospy.Subscriber("/Tablet/voice", SpeechRecognitionCandidates, word_motor_cb)
    rospy.loginfo("Setting up vocabulary...")
    #rospy.wait_for_service('/Tablet/voice')
    try:
        rospy.loginfo("good")
    except rospy.ServiceException, e:
        print "Service call failed: %s"%e
        rospy.loginfo("OK")
            
    rate =  rospy.Rate(10)
    
    rospy.spin()
    rate.sleep()
    
if __name__ == '__main__':
    main()
