#include <AirQuality.h>
#include <Correction.h>
#include <GasSensor.h>
#include <SensorDefinitions.h>

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include "driver/i2s_std.h"
#include <DHT.h>
#include <PubSubClient.h>

using namespace websockets;

//Wifi
const char* ssid = "SSID WIFI"; //wifi name your use
const char* password = "Password WIFI"; // password for the wifi
//Server Websocket 
const char* websocket_server = "IP server"; //ip for websocket server
const uint16_t websocket_port = 8765; //port server default 8765
//server MQTT
const char *mqtt_broker = websocket_server; //IP broker mqqtt, if its in the same computer as websocket server, then the ip is same
const char *topic_send = "esp32/data_sensor"; // Topic to send data sensor
const int mqtt_port = 1883; //mqtt port defaul 1883

WebsocketsClient ws;
WiFiClient espClient;
PubSubClient client(espClient);

//pin INMP441
#define I2S_WS      21
#define I2S_SD      18
#define I2S_SCK     19

//pin MAX98357A
#define SPK_LRC 27
#define SPK_BCLK 26
#define SPK_DIN 25

//pin sensor 
#define ADC_BIT_RESU 12
#define MQ135_PIN    35
#define DHT_PIN  33
#define DHTTYPE DHT11 

#define SAMPLE_RATE        16000
#define BUFFER_SAMPLES     1024

int16_t pcmBuffer[BUFFER_SAMPLES];

DHT dht(DHT_PIN, DHTTYPE);

GasSensor sensor(ADC_BIT_RESU, MQ135_PIN);
SensorModel* sensorModel = nullptr;

unsigned long waktusensor=0;
unsigned long reloadsensor=10000;

unsigned long waktumqtt=0;
unsigned long reloadmqtt=5000;

i2s_chan_handle_t rx_chan = NULL;
i2s_chan_handle_t tx_chan = NULL;

bool playingaudio=false;
size_t totalAudioSize = 0;
size_t receivedAudioSize = 0;

void setupI2Smic(){
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(
    I2S_NUM_0,
    I2S_ROLE_MASTER
  );
  ESP_ERROR_CHECK(i2s_new_channel(
    &chan_cfg,
    NULL,
    &rx_chan
  ));
  i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(
      I2S_DATA_BIT_WIDTH_16BIT,
      I2S_SLOT_MODE_MONO
    ),
    .gpio_cfg = {
      .mclk = I2S_GPIO_UNUSED,
      .bclk = (gpio_num_t)I2S_SCK,
      .ws   = (gpio_num_t)I2S_WS,
      .dout = I2S_GPIO_UNUSED,
      .din  = (gpio_num_t)I2S_SD,
      .invert_flags = {
        .mclk_inv = false,
        .bclk_inv = false,
        .ws_inv = false,
      },
    },
  };
  ESP_ERROR_CHECK(
    i2s_channel_init_std_mode(
      rx_chan,
      &std_cfg
    )
  );

  ESP_ERROR_CHECK(
    i2s_channel_enable(rx_chan)
  );
}

void setupI2SSpeaker(){
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(
    I2S_NUM_1,
    I2S_ROLE_MASTER
  );
  ESP_ERROR_CHECK(i2s_new_channel(
    &chan_cfg,
    &tx_chan,
    NULL
  ));
  i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(22050),
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(
      I2S_DATA_BIT_WIDTH_16BIT,
      I2S_SLOT_MODE_MONO
    ),
    .gpio_cfg = {
      .mclk = I2S_GPIO_UNUSED,
      .bclk = (gpio_num_t)SPK_BCLK,
      .ws   = (gpio_num_t)SPK_LRC,
      .dout = (gpio_num_t)SPK_DIN,
      .din  = I2S_GPIO_UNUSED,

      .invert_flags = {
        .mclk_inv = false,
        .bclk_inv = false,
        .ws_inv = false,
      },
    },
  };
  ESP_ERROR_CHECK(
    i2s_channel_init_std_mode(
      tx_chan,
      &std_cfg
    )
  );

  ESP_ERROR_CHECK(
    i2s_channel_enable(tx_chan)
  );
}

void setupMQTT(){
  while (!client.connected()){ 
    if (client.connect("ESP32_Client")){ 
    }
    else{ 
      delay(1000); 
    } 
  }
}

void readsensor() {
    if(millis() - waktusensor >= reloadsensor){
      waktusensor = millis();
      float hum = dht.readHumidity();
      float temp = dht.readTemperature();
      float sensorValue = sensor.read();
      float correction = calculateCorrection(temp, hum, "MQ135");
      float airPPM = airConcentration("MQ135", sensorValue) * correction;

      char payload[100];
      snprintf(payload, sizeof(payload),
         "{\"suhu\":%.2f,\"kelembaban\":%.2f,\"airPPM\":%.2f}",
         temp, hum, airPPM);
      client.publish(topic_send, payload);
    }
}

//WIFI
void connectWiFi()
{
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
  }
}

void onMessageCallback(websockets::WebsocketsMessage message){
  if(message.isText()){
    String msg =message.data();
    if (msg.startsWith("AUDIO_START:")){
      totalAudioSize=msg.substring(12).toInt();
      receivedAudioSize=0;
      playingaudio = true;
      i2s_channel_disable(tx_chan);
      i2s_channel_enable(tx_chan);
    }
    else if (msg =="AUDIO_END"){
      playingaudio = false;
    }
    return;
  }
  if (!message.isBinary()|| !playingaudio)
  return;

  size_t written = 0;

  esp_err_t result = i2s_channel_write(
    tx_chan,
    message.c_str(),
    message.length(),
    &written, 
    100
  );
  if (result == ESP_OK){
    receivedAudioSize += written;
  }
    if (receivedAudioSize>=totalAudioSize){
      playingaudio = false;
    }
}


//WEBSOCKET
void connectWebsocket()
{
  while(!ws.connect(websocket_server, websocket_port, "/")){
    delay(2000);
  }
  ws.onMessage(onMessageCallback);
}

//SETUP
void setup()
{
  connectWiFi();
  client.setServer(mqtt_broker,mqtt_port);
  setupMQTT();
  setupI2Smic();
  setupI2SSpeaker(); 
  connectWebsocket();
  dht.begin();
  sensor.begin();
  sensorModel = getSensorModel("MQ135");
}

//loop
void loop()
{
  ws.poll();
  if(!ws.available()){
    connectWebsocket();
    return;
  }
  
  if(!client.connected()){
    setupMQTT();
    return;
  }
  if(client.connected()){
    client.loop();
  }

  if(playingaudio){
    return;
  }

  //read mic
  size_t bytesRead = 0;
  esp_err_t result = i2s_channel_read(
    rx_chan,
    pcmBuffer,
    sizeof(pcmBuffer),
    &bytesRead,
    portMAX_DELAY
  );

  //kirim ke websocket
  if(result == ESP_OK && bytesRead > 0){
    ws.sendBinary((const char*)pcmBuffer,bytesRead);
  }

  readsensor();
}