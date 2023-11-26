#!/bin/python3

import paho.mqtt.client as mqtt
import time

i1=0
i2=0
i3=1
i4=1

def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server """
    print("Connected to MQTT server (result code "+str(rc)+")")
    client.subscribe("j/npanel/e/#")


def on_message(client, userdata, msg):
    global i1,i2,i3,i4
    """ The callback for when a PUBLISH message is received from the server """
    print(msg.topic+" "+str(msg.payload))

    if (msg.topic=="j/npanel/e/k"):
        if (msg.payload.decode('utf-8')=="50"):
            i1=i1-1
        if (msg.payload.decode('utf-8')=="51"):
            i1=i1+1
        if (msg.payload.decode('utf-8')=="52"):
            i2=i2-1
        if (msg.payload.decode('utf-8')=="53"):
            i2=i2+1
        if (msg.payload.decode('utf-8')=="37"):
            i3=-1*i3
        if (msg.payload.decode('utf-8')=="38"):
            i3=-1*i3
        if (msg.payload.decode('utf-8')=="16"):
            i4=-1*i4

    if (msg.topic=="j/npanel/e/s"):
        if (msg.payload.decode('utf-8')=="1"):
            # resend page if connection has been lost
            draw();
    return

def publish(topic, payload):
    global client,alivetime

    client.publish(topic,payload)
    alivetime=time.time()   # the keepalive ticker

def draw():
    publish("j/npanel/c/d","N,Test script")

    publish("j/npanel/c/d","B,01,LSK1")         #button
    publish("j/npanel/c/d","T,02,98,0,3,TEXT1") #text
    publish("j/npanel/c/d","T,03,99,1,3,   0")
    publish("j/npanel/c/d","T,04,99,0,3,   0")
    publish("j/npanel/c/d","B,05,LSK5")
    publish("j/npanel/c/d","B,06,LSK6")
    publish("j/npanel/c/d","T,07,99,1,2,LSK7")
    publish("j/npanel/c/d","B,08,LSK8")

    publish("j/npanel/c/d","B,13,LCK1")
    publish("j/npanel/c/d","T,16,94,1,5,Y/D")  #switch
    publish("j/npanel/c/d","B,26,LCK1")
    publish("j/npanel/c/d","B,36,LCK1")

    publish("j/npanel/c/d","B,31,RSK1")
    publish("j/npanel/c/d","B,32,RSK2")
    publish("j/npanel/c/d","S,33,98,0,0")
    publish("j/npanel/c/d","B,34,RSK4")
    publish("j/npanel/c/d","S,35,99,1,0")

    publish("j/npanel/c/d","L,1")  #command led

if __name__ == "__main__":
    # MQTT borcker connection on localhost
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("192.168.1.248", 1883, 60)
    client.loop_start()

    alivetime=time.time()   # the keepalive ticker

    draw();

    _i1=0
    _i2=0
    _i3=0
    _i4=0
    _led=0

    while True:

        if (i1!=_i1):
            _i1=i1
            publish("j/npanel/c/d","S,33,98,0,"+str(i1))
            publish("j/npanel/c/d","S,35,99,1,"+str(-i1))
            #client.publish("j/npanel/c/d","T,03,99,1,"+str(i1)+"  ")
        if (i2!=_i2):
            _i2=i2
            publish("j/npanel/c/d","T,04,99,0,"+str(i2)+"  ")

        if (i4!=_i4):
            _i4=i4
            if (i4==-1):
                publish("j/npanel/c/d","T,16,94,1,4,Y/D")
            else:
                publish("j/npanel/c/d","T,16,94,1,5,Y/D")


        if (i3!=_i3):
            _i3=i3
            if (i3==-1):
                publish("j/npanel/c/d","T,37,98,1,3,AVEC")
                publish("j/npanel/c/d","T,38,97,0,3,SANS")
            else:
                publish("j/npanel/c/d","T,37,97,0,3,SANS")
                publish("j/npanel/c/d","T,38,98,1,3,AVEC")


        if ((time.time() - alivetime) > 2):
            if (_led==0):
                publish("j/npanel/c/d","L,1")
                _led=1
            else:
                publish("j/npanel/c/d","L,0")
                _led=0
            publish("j/npanel/c/reqstatus", "1")
            publish("j/npanel/c/reqdiag", "1")
            alivetime=time.time()
