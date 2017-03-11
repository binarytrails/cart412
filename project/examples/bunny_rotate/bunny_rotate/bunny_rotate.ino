#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define TCAADDR 0x70

/* Assign a unique ID to this sensor at the same time */
Adafruit_BNO055 bno1 = Adafruit_BNO055(1);
Adafruit_BNO055 bno2 = Adafruit_BNO055(2);

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

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

void tcaselect(uint8_t i)
{
    if (i > 7) return;

    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << i);
    Wire.endTransmission();
}


void setup(void)
{
    Serial.begin(9600);
    Serial.println("BNO055 Test"); Serial.println("");

    /* Initialise the 1st sensor */
    tcaselect(2);
    if(!bno1.begin())
    {
        Serial.println("Ooops, no BNO055 detected ... Check your wiring!");
        while(1);
    }

    /* Initialise the 2nd sensor */
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
  /* Get a new sensor event */
  sensors_event_t event;
  bno1.getEvent(&event);

  /* Board layout:
         +----------+
         |         *| RST   PITCH  ROLL  HEADING
     ADR |*        *| SCL
     INT |*        *| SDA     ^            /->
     PS1 |*        *| GND     |            |
     PS0 |*        *| 3VO     Y    Z-->    \-X
         |         *| VIN
         +----------+
  */

  /* The processing sketch expects data as roll, pitch, heading */
  Serial.print(F("Orientation: "));
  Serial.print((float)event.orientation.x);
  Serial.print(F(" "));
  Serial.print((float)event.orientation.y);
  Serial.print(F(" "));
  Serial.print((float)event.orientation.z);
  Serial.println(F(""));

  /* Also send calibration data for each sensor. */
  uint8_t sys, gyro, accel, mag = 0;
  bno1.getCalibration(&sys, &gyro, &accel, &mag);
  Serial.print(F("Calibration: "));
  Serial.print(sys, DEC);
  Serial.print(F(" "));
  Serial.print(gyro, DEC);
  Serial.print(F(" "));
  Serial.print(accel, DEC);
  Serial.print(F(" "));
  Serial.println(mag, DEC);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}
