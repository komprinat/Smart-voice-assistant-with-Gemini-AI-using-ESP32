# Smart voice assistant with Gemini AI using ESP32
## Alur kerja sistem
- Voice input menggunakan INMP441
- ESP32 sebagai perangkat utama
- WebSocket untuk komunikasi audio
- Speech-to-Text
- Gemini AI
- Text-to-Speech menggunakan Piper
- MQTT untuk komunikasi data sensor

![Alur kerja](/Perancangan/images/alur%20kerja.png)

## Rangkaian ESP32
![Rangkaian](/Perancangan/images/rangkaian.png)
### Library yang digunakan
* [ArduinoWebscoket by gilmaimon](https://github.com/gilmaimon/ArduinoWebsockets)
* [DHT sensor library by adafruit](https://github.com/adafruit/dht-sensor-library)
* [MQSpaceData by abcdaaaaaaaaa](https://github.com/abcdaaaaaaaaa/MQDataScience?utm_source=pioarduino)
* [PubSubClient by knolleary](https://github.com/knolleary/pubsubclient?utm_source=pioarduino)

## Server
Server menggunakan python 3.13 dan Broker MQTT Mosquitto. Pastikan semua library yang ada di requirements sudah terinstall baik secara global maupun di virtual environment. Untuk Menjalankan server klik run.bat dengan mode administrator

## Hardware dan yang lainnya
### PCB
PCB dibuat menggunakan software KiCad dan pabrikasi manual dengan metode toner pada PCB single layer
<img src="/Perancangan/images/desain PCB.png" height="500">
### Desain kotak
![Desain kotak](/Perancangan/images/desain%20kotak.png)
3D Printing menggunakan filamen PLA+ dengan infill 15% dan layer height 0,2mm

![Hasil jadi](/Perancangan/images/Desain%20jadi.png)

## Bug dan Error yang ada
* Koneksi antara ESP32 dan server kadang terputus
* Suara kadang freeze