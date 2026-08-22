import asyncio
import websockets
import speech_recognition as sr
import json

from gemini_api import ask
from piper_tts import tts_piper

from mqtt_connect import MQTTClientpub
from mqtt_connect import MQTTClientsub
from config import MQTT_TOPIC_POST
from config import MQTT_TOPIC_RECEIVE

recognizer = sr.Recognizer()
audio_buffer = bytearray()

SAMPLE_RATE = 16000
SAMPLE_WIDTH = 2

sensor={}
def receive_message(topic, message):
    global sensor
    sensor=json.loads(message)
    airPPM = sensor["airPPM"]
    if airPPM <= 50:
        sensor["airQuality"] = "Good"
    elif airPPM <= 100:
        sensor["airQuality"] = "Moderate"
    elif airPPM <= 199:
        sensor["airQuality"] = "Bad"
    elif airPPM <= 299:
        sensor["airQuality"] = "Very Bad"
    else:
        sensor["airQuality"] = "Dangerous"
    print(sensor)


mqtt_sub = MQTTClientsub(topic=MQTT_TOPIC_RECEIVE,callback=receive_message)
mqtt_sub.start_background()

async def handler(websocket):
    global audio_buffer
    print("ESP32 Connected")
    async for message in websocket:
        audio_buffer.extend(message)
        if len(audio_buffer) >= SAMPLE_RATE * SAMPLE_WIDTH * 3:
            try:
                audio = sr.AudioData(bytes(audio_buffer),SAMPLE_RATE,SAMPLE_WIDTH)
                text = await asyncio.to_thread (recognizer.recognize_google,audio,language="id-ID")
                print(text)

                jawaban_gemini=await asyncio.to_thread (ask,text,sensor)
                print(jawaban_gemini)

                packet_size=1024
                pcm_bytes = await asyncio.to_thread(tts_piper,jawaban_gemini)
                await websocket.send(f"AUDIO_START:{len(pcm_bytes)}")
                for i in range (0, len(pcm_bytes), packet_size):
                    await websocket.send(pcm_bytes[i:i+packet_size])
                    await asyncio.sleep(0)

                await websocket.send("AUDIO_END")

            except sr.UnknownValueError:
                print("suara tidak dikenali")

            except Exception as e:
                print(e)

            audio_buffer.clear()

async def main():
    async with websockets.serve(handler, "0.0.0.0", 8765,ping_interval=30, ping_timeout=60):
        print("Server Running")
        await asyncio.Future()

asyncio.run(main())

