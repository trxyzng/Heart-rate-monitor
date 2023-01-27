
import paho.mqtt.client as mqtt 
import pyrebase 
import time
from datetime import datetime



config = {
  "apiKey": "AIzaSyB1eOxXlPpUht1GB_dkZFjwyK8CHrXxO98",
  "authDomain": "esp32-af5ad.firebaseapp.com",
  "databaseURL": "https://esp32-af5ad-default-rtdb.asia-southeast1.firebasedatabase.app",
  "projectId": "esp32-af5ad",
  "storageBucket": "esp32-af5ad.appspot.com",
  "messagingSenderId": "1082673292807",
  "appId": "1:1082673292807:web:100fedc39363c6eb59aca2"
};

firebase = pyrebase.initialize_app(config)

db = firebase.database()



def on_connect(client, userdata, flags, rc):  #get call when the client try to connect
    if rc == 0:
        print("Client connected successful")      #print to console
    else:
        print("Client connected fail with code {rc}")   #print to console
        client.on_connect

def on_message(client, userdata, message):
    s  = round(time.time() * 1000)
    msg = str(message.payload.decode("utf-8"))
    if msg == "no":
        time.localtime()
        now = datetime.now()
        hmsdmy = time.strftime("%H%M%S%d%m%y")
        print("Message:")
        print("no")
        print("Device off")
        print("Push to firebase: ")
        print(hmsdmy+"0000")
        print("..............................................")
        db.child("User").child("Data").push(hmsdmy+"0000")
    elif msg == "on":
        time.localtime()
        now = datetime.now()
        hmsdmy = time.strftime("%H%M%S%d%m%y")
        print("Message:")
        print("on")
        print("Device on")
        print("Push to firebase: ")
        print(hmsdmy+"AAAA")
        print("..............................................")
        db.child("User").child("Data").push(hmsdmy+"AAAA")
    elif msg == "":
        print("Message:")
        print("null")
        print("Data error")
        print("Skip push to Firebase")
        print("..............................................")
    else:
        #1050333007228996
        hour = msg[0:2]
        minute = msg[2:4]
        second = msg[4:6]
        day = msg[6:8]
        month = msg[8:10]
        year = msg[10:12]
        heartbeat = msg[12:len(msg)-2]
        spo2 = msg[len(msg)-2:len(msg)]
        print("Message:")
        print(msg)
        print("Data is correct")
        print("Push to firebase: ")
        print(msg)
        db.child("User").child("Data").push(msg)
        v =  round(time.time() * 1000)
        print("Time to send data to Firebase: ")
        print(v-s)
        print("..............................................")


                   
broker_address = "broker.hivemq.com"	#address of cloud mqtt server 
port = 1883      			#port to connect to mqtt server
user = "test"      			#user name to connect to server
password = "test"  			#password to connect to server

client = mqtt.Client(client_id="subscriber_hoacchitrung_nguyendinhhongquan", clean_session=True, userdata=None, protocol=mqtt.MQTTv311, transport="tcp") #create new client
client.username_pw_set(user,password)  #user and password for connect
print("Connecting to broker...")       #print to console
client.connect(broker_address, port, 10)   #connect to broker with keep alive time = 10s
client.on_connect = on_connect             #check if connect or not
client.subscribe("hoacchitrung_nguyendinhhongquan", qos=2)      	    #subcribe to topic data
client.on_message = on_message             #if receive message, call on_message
client.loop_forever()
