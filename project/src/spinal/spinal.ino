#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

extern "C" {
  // from Wire library, so we can do bus scanning
  #include "utility/twi.h"
}

#define TCAADDR 0x70
#define IMU_SAMPLERATE_MS (100)

/* Assign a unique ID to this sensor at the same time */
Adafruit_BNO055 bno1 = Adafruit_BNO055(1);
Adafruit_BNO055 bno2 = Adafruit_BNO055(2);

void tcaselect(uint8_t i)
{
    if (i > 7) return;

    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << i);
    Wire.endTransmission();
}

void findIMU()
{
  Wire.begin();

  for (uint8_t t=0; t<8; t++)
  {
    tcaselect(t);
    Serial.print("TCA Port #"); Serial.println(t);

    for (uint8_t addr = 0; addr<=127; addr++)
    {
      if (addr == TCAADDR)
        continue;

      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1))
      {
        Serial.print("Found I2C 0x");
        Serial.println(addr,HEX);
      }
    }
  }
}

void displaySensorDetails(Adafruit_BNO055 *mag)
{
    sensor_t sensor;
    mag->getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
    Serial.println("------------------------------------");
    Serial.println("");
    delay(500);
}

void setup(void)
{
    Serial.begin(9600);
    Serial.println("BNO055 Test"); Serial.println("");

    findIMU();

    tcaselect(2);
    if(!bno1.begin())
    {
        Serial.println("Ooops, no BNO055 detected ... Check your wiring!");
        while(1);
    }

    tcaselect(6);
    if(!bno1.begin())
    {
        Serial.println("Ooops, no BNO055 detected ... Check your wiring!");
        while(1);
    }

    tcaselect(2);
    displaySensorDetails(&bno1);

    tcaselect(6);
    displaySensorDetails(&bno2);
}

void loop(void)
{
    sensors_event_t event;

    tcaselect(2);
    bno1.getEvent(&event);

    Serial.print("Sensor #1 - ");
    Serial.print("X: "); Serial.print(event.orientation.x, 4);
    Serial.print("\tY: "); Serial.print(event.orientation.y, 4);
    Serial.print("\tZ: "); Serial.print(event.orientation.z, 4);
    Serial.println("");

    tcaselect(6);
    bno2.getEvent(&event);

    Serial.print("Sensor #2 - ");
    Serial.print("X: "); Serial.print(event.orientation.x, 4);
    Serial.print("\tY: "); Serial.print(event.orientation.y, 4);
    Serial.print("\tZ: "); Serial.print(event.orientation.z, 4);
    Serial.println("");

    delay(IMU_SAMPLERATE_MS);
}
