# Smart voice assistant with Gemini AI using ESP32
## Alur kerja sistem
- Voice input menggunakan INMP441
- ESP32 sebagai perangkat utama
- WebSocket untuk komunikasi audio
- Speech-to-Text menggunakan speech recognition
- Gemini AI
- Text-to-Speech menggunakan Piper dengan [model bahasa indonesia](/server/model_tts)
- MQTT untuk komunikasi data sensor

![Alur kerja](/Perancangan/images/alur%20kerja.png)

## Rangkaian ESP32
![Rangkaian](/Perancangan/images/rangkaian.png)
### Library yang digunakan
Code pada ESP32 dibuat menggunakan Platformio dengan [code](/ESP32/src/main.cpp) serta menggunakan [library](/ESP32/platformio.ini)
* [ArduinoWebscoket by gilmaimon](https://github.com/gilmaimon/ArduinoWebsockets)
* [DHT sensor library by adafruit](https://github.com/adafruit/dht-sensor-library)
* [MQSpaceData by abcdaaaaaaaaa](https://github.com/abcdaaaaaaaaa/MQDataScience?utm_source=pioarduino)
* [PubSubClient by knolleary](https://github.com/knolleary/pubsubclient?utm_source=pioarduino)

## Server
Server menggunakan python 3.13 dan Broker MQTT Mosquitto. Pastikan semua library yang ada di [requirements](/server/requirements.txt) sudah terinstall baik secara global atau di virtual environment. Pastikan sudah mengisi API Key Gemini di [kode](/server/config.py) yang bisa didapat di [sini](https://aistudio.google.com/api-keys). Untuk Menjalankan server klik run.bat dengan mode administrator dan ketik server on

## Hardware dan yang lainnya
### PCB
PCB dibuat menggunakan software KiCad untuk [desain](/Perancangan/PCB/TA.kicad_pcb) serta [skematiknya](/Perancangan/PCB/TA.kicad_sch) dan pabrikasi dilakukan dengan metode toner pada PCB single layer

<img src="/Perancangan/images/desain PCB.png" height="300">

### Desain kotak
![Desain kotak](/Perancangan/images/desain%20kotak.png)
[Desain](/Perancangan/3D%20Printing%20Design/Untuk%20TA2.FCStd) dibuat menggunakan FreeCAD 3D Printing menggunakan filamen PLA+ dengan infill 15% dan layer height 0,2mm

<img src="/Perancangan/images/Desain jadi.png" height="300">

## Bug dan Error yang ada
* Koneksi antara ESP32 dan server kadang terputus
* Suara kadang freeze
