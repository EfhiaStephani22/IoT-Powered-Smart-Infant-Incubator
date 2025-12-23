#include "MAX30100.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define POLL_PERIOD_US                      1E06 / 100
#define SAMPLING_RATE                       MAX30100_SAMPRATE_100HZ
#define IR_LED_CURRENT                      MAX30100_LED_CURR_50MA
#define RED_LED_CURRENT                     MAX30100_LED_CURR_27_1MA
#define PULSE_WIDTH                         MAX30100_SPC_PW_1600US_16BITS
#define HIGHRES_MODE                        true
MAX30100 sensor;
uint32_t tsLastPollUs = 0;
int gaspin = A0;
int gas = 0;
int buz = 11;
int voicepin=A1;
int voice=0;
int fan=3;
int peltier=12;

#include <Wire.h>
#include "DHT.h"
#include "MAX30100.h"
#define DHTPIN 4
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(buz, OUTPUT);
    pinMode(fan, OUTPUT);
      pinMode(peltier, OUTPUT);
  dht.begin();
    lcd.begin();
  lcd.setCursor(0, 0);
  lcd.print("    BABY ");
  lcd.setCursor(0, 1);
  lcd.print(" MONITORING");
  delay(1000);
  if (!sensor.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
    sensor.setMode(MAX30100_MODE_SPO2_HR);
    sensor.setLedsCurrent(IR_LED_CURRENT, RED_LED_CURRENT);
    sensor.setLedsPulseWidth(PULSE_WIDTH);
    sensor.setSamplingRate(SAMPLING_RATE);
    sensor.setHighresModeEnabled(HIGHRES_MODE);
  delay(1000);

}

void loop() {
  gas = analogRead(gaspin);
  voice = analogRead(voicepin);
  int hum = dht.readHumidity();
  int temp = dht.readTemperature();
  temp= ( temp * 1.8 ) + 41;
   gas = map(gas, 300, 1024, 0, 100);
  Serial.print("  Temperature:  ");
 Serial.print(temp);
 Serial.println("°C ");
  Serial.print("  Humidity:  ");
 Serial.print(hum);
 Serial.println("% ");

  Serial.print("Raw Gas Value = ");
  Serial.println(gas);
  Serial.print("Voice = ");
  Serial.println(voice);

   if (micros() < tsLastPollUs || micros() - tsLastPollUs > POLL_PERIOD_US) {
        sensor.update();
        tsLastPollUs = micros();

        Serial.print(sensor.rawIRValue);
        Serial.print('\t');
        Serial.println(sensor.rawRedValue);
    }
  int bpm=sensor.rawIRValue*0.0013;
  int spo2 = sensor.rawRedValue*0.0043;
  Serial.print("SPO2=");
  Serial.println(spo2);
  Serial.print("BPM=");
  Serial.println(bpm);
if(gas<0)
{
gas=0;
}
else
{
  gas=gas;
}
  if (gas > 55) 
  {
    digitalWrite(buz, HIGH);
    digitalWrite(fan, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!   GAS    !    ");
    lcd.setCursor(0, 1);
    lcd.print("!  ALERT   !    ");
    delay(1000);
  } 
  else if(temp>101)
{
    digitalWrite(buz,1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!  TEMPERATURE   !    ");
    lcd.setCursor(0, 1);
    lcd.print("!    IS HIGH     !    ");
    delay(1000);
  }
    else if(temp<80)
{
    digitalWrite(buz,1);
    digitalWrite(peltier, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!  TEMPERATURE   !    ");
    lcd.setCursor(0, 1);
    lcd.print("!    IS LOW      !    ");
    delay(100);
    digitalWrite(peltier, LOW);
    delay(500);
  }
   else if(bpm>120)
{
    digitalWrite(buz,1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!   ABNORMAL  !    ");
    lcd.setCursor(0, 1);
    lcd.print("!  HEARTRATE  !    ");
    delay(1000);
  }
   else if(voice>60)
{
    digitalWrite(buz,1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!   BABY   !    ");
    lcd.setCursor(0, 1);
    lcd.print("!  CRYING  !    ");
    delay(1000);
  }
  else 
  {
    digitalWrite(buz, LOW);
    digitalWrite(fan, LOW);
    digitalWrite(peltier, LOW);
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("G:");
  lcd.setCursor(3, 0);
  lcd.print(gas);
  lcd.print(" % ");
  lcd.setCursor(8, 0);
  lcd.print("T: ");
  lcd.setCursor(11, 0);
  lcd.print(temp);
  lcd.print(" F ");
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.setCursor(3, 1);
  lcd.print(hum);
  lcd.print(" % ");
  lcd.setCursor(8, 1);
  lcd.print("BPM: ");
  lcd.setCursor(13, 1);
  lcd.print(bpm);

  }
  
  delay(1000);
}
