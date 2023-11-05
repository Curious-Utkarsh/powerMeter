#include "EmonLib.h"             
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char * ssid = "powerMeter";
const char * password = "powerMeter2023";

String server = "http://maker.ifttt.com";
String eventName = "powerMeter";
String IFTTT_Key = "3GqyNyUZdnuDiXl7UYrLQP3V41fMDXwysEuNDAm7BR";
String IFTTTUrl="http://maker.ifttt.com/trigger/powerMeter/with/key/3GqyNyUZdnuDiXl7UYrLQP3V41fMDXwysEuNDAm7BR";

LiquidCrystal_I2C lcd(0x27,16,2);

EnergyMonitor emon1;          

int c = 0;
int k = 0;
float avgVolt = 0.0;
float avgCurr = 0.0;
float sumVolt = 0.0;
float sumCurr = 0.0;

String strVolt = "";
String strCurr = "";

void setup()
{  
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Hurray, Connected !!!");

  lcd.begin();
  lcd.clear();         
  lcd.backlight();

  lcd.setCursor(1,0);
  lcd.print("Energy-Monitor");
  
  lcd.setCursor(2,1);
  lcd.print("Power-Meter");

  delay(4000);
  
  emon1.voltage(35, 592, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(34, 1000);       // Current: input pin, calibration.
}

void loop()
{
  emon1.calcVI(20,100);         // Calculate all. No.of half wavelengths (crossings), time-out
  
  //float realPower       = emon1.realPower;        //extract Real Power into variable
  //float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  //float powerFactor     = emon1.powerFactor;      //extract Power Factor into Variable
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  float Irms            = emon1.Irms;             //extract Irms into Variable

  sumVolt = sumVolt + supplyVoltage;
  sumCurr = sumCurr + Irms;
  c++;
  k++;

  if(c == 10)
  {
    avgVolt = float(sumVolt/10);
    avgCurr = float(sumCurr/10);

    if(avgVolt > 100)
    {
      Serial.println(String(avgVolt)+"  "+String(avgCurr));

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Voltage: "+String(avgVolt)+"V");
      lcd.setCursor(0,1);
      lcd.print("Current: "+String(avgCurr)+"mA");

      strVolt = String(String(avgVolt)+"V");
      strCurr = String(String(avgCurr)+"mA");

      if(k == 50)
      {
        sendDataToSheeton();
        k = 0;
      }

      c = 0;
      sumVolt = 0.0;
      sumCurr = 0.0;
      avgVolt = 0.0;
      avgCurr = 0.0;
      strVolt = "";
      strCurr = "";
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Voltage : 0.00V");
      lcd.setCursor(0,1);
      lcd.print("Current : 0.00mA");

      sendDataToSheetoff();

      c = 0;
      k = 0;
      sumVolt = 0.0;
      sumCurr = 0.0;
      avgVolt = 0.0;
      avgCurr = 0.0;
      strVolt = "";
      strCurr = "";
    }
  }
}
