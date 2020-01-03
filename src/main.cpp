#include <Adafruit_CCS811.h>
#include <DHT.h>
#include <Config.h>
#include <ThingSpeak.h>
#include <WiFi.h>
#include <Measures.h>

WiFiClient client;
Adafruit_CCS811 ccs; // SDA 21, SCL 22
DHT dht(4, DHT22);
const char *server = "api.thingspeak.com";
int retries = 0;

void WifiConnect()
{
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PSK);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}

void Co2SensorInit()
{
  Serial.println("CCS811 Reading CO2 and VOC");
  while (!ccs.begin())
  {
    Serial.println("Failed to start sensor! Please check your wiring.");
    sleep(1000);
  }
  
  //calibrate temperature sensor
  while (!ccs.available())
    ;
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);
}

void setup()
{
  Serial.begin(9600);

  WifiConnect();
  Co2SensorInit();
  dht.begin();

  ThingSpeak.begin(client);
}

void SendToThingSpeak(uint16_t co2, uint16_t tvoc, float temperature, float humidity)
{
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(4, co2);
  ThingSpeak.setField(5, tvoc);
  int returnCode = ThingSpeak.writeFields(ChannelId, ApiKey);
  if (returnCode == 200)
  {
    Serial.print("Channel was update: Co2 ");
    Serial.print(co2);
    Serial.print(", Tvoc ");
    Serial.print(tvoc);
    Serial.print(", Temperature ");
    Serial.print(temperature);
    Serial.print(", Humidity ");
    Serial.println(humidity);
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(returnCode));
  }
}

int WarmSensorCounter = 0;
int WarmCount = 10;
Co2Measure IsCo2Prepare()
{
  Co2Measure measure;
  measure.IsPrepare = false;

  if (ccs.available())
  {
    if (!ccs.readData())
    {
      WarmSensorCounter++;
      measure.Co2 = ccs.geteCO2();
      measure.Tvoc = ccs.getTVOC();

      Serial.print("Co2 ");
      Serial.print(measure.Co2);
      Serial.print(", Tvoc ");
      Serial.print(measure.Tvoc);
      Serial.print(", Warm counter ");
      Serial.println(WarmSensorCounter);

      if (WarmSensorCounter > WarmCount)
      {
        measure.IsPrepare = true;
      }
    }
    else
    {
      Serial.println("Co2 sensor read ERROR!");
      ccs.readData();
    }
  }

  return measure;
}

DhtMeasure IsDhtPrepare()
{
  DhtMeasure measure;
  measure.IsPrepare = true;

  delay(100);
  float calibration = 3;
  measure.Temperature = dht.readTemperature() - calibration;
  delay(100);
  measure.Humidity = dht.readHumidity();

  return measure;
}

void loop()
{
  Co2Measure co2Measure = IsCo2Prepare();
  DhtMeasure dhtMeasure = IsDhtPrepare();
  if (co2Measure.IsPrepare && dhtMeasure.IsPrepare)
  {
    SendToThingSpeak(co2Measure.Co2, co2Measure.Tvoc, dhtMeasure.Temperature, dhtMeasure.Humidity);

    WarmSensorCounter = 0;
    ESP.deepSleep(DeepSleepTime);
  }

  delay(1000);
}