# MeteoStar
PlatformIO project within Visual Studio Code for temperature, huminidy, co2 and tvoc measurements. Data are sending to ThingSpeak.

# HW
- NodeMcu-32s
- DHT22
- MCU 811

# Configuration in Config.h
```
const char *SSID = "...";
const char *PSK = "...";
const char *ApiKey = "...";
unsigned long ChannelId = 0;

// 15e6 is 15 seconds, 900e6 is 15 minutes
uint64_t DeepSleepTime = 900e6;
```
