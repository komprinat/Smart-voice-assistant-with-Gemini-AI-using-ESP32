import paho.mqtt.client as mqtt

from config import MQTT_BROKER
from config import MQTT_PORT

class MQTTClientpub:

    def __init__(self):

        self.client = mqtt.Client()
        self.client.connect(MQTT_BROKER, MQTT_PORT, 60)
        self.client.loop_start()

    def publish(self, topic, message):

        self.client.publish(topic, message)

class MQTTClientsub:

    def __init__(self, topic, callback):

        self.client = mqtt.Client()

        self.topic = topic
        self.callback = callback

        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

    def on_connect(self, client, userdata, flags, rc):
        client.subscribe(self.topic)

    def on_message(self, client, userdata, msg):
        self.callback(msg.topic, msg.payload.decode())

    def start_background(self):
        self.client.connect(MQTT_BROKER, MQTT_PORT)
        self.client.loop_start()