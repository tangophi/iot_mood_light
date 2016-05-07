#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
//#include <SoftwareSerial.h>

#define PIN_LIGHT       6
#define PIN_LEDS        2
#define LED_COUNT     143
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN_LEDS, NEO_GRB + NEO_KHZ800);

enum ledModes
{
  LedModeDefault=0,
  LedModeNegative,
  LedModeFade,
  LedModeRainbow,
  LedModeCycle,
  LedModeCycle2,    // 5
  LedModeCycle3,
  LedModeCycle4,
  LedModeRandom,
  LedModeRandom2,
  LedModeRandomCycle,  // 10
  LedModeRandomCycle2,
  LedModeStrobe,
  LedModeStrobeRandom,
  LedModeStrobeStep,
  LedModeStrobeStep2,       // 15
  LedModeStrobeStepRandom, 
  LedModeStrobeStepRandom2,
  LedModeLarsonScanner,
  LedModeLarsonScannerRandom  //19
};

ledModes ledMode = LedModeDefault;
int ledCommand=0,lightCommand=0;
boolean bColorChanged = true;

boolean bLightOn    = false;
boolean bLedOn = false;
int intLedMode = 0;
int ledSpeed=0;
int colorRed=0,colorGreen=0,colorBlue=255;

boolean prev_bLightOn    = false;
boolean prev_bLedOn = false;
int prev_intLedMode = 0;
int prev_ledSpeed=0;
int prev_colorRed=0,prev_colorGreen=0,prev_colorBlue=255;

int randr=0,randg=0,randb=0, randIndex = 0;
int J=0,K=0,L=0,x=0,y=0,z=0, STEP=0, STEP2=0;

struct rgb
{
  byte r;
  byte g;
  byte b;
};
rgb leds[LED_COUNT];



int colorsR[23] = {255, 255, 255, 255, 255, 255, 255, 255, 205, 137,  34,   0,   0,   0,   0,   0,   0,   0,  78, 120, 170, 222, 255};
int colorsG[23] = {215, 212, 171,  62,   0,   0,   0,   0,   0,   0,   0,   0,  33, 109, 213, 255, 255, 255, 255, 255, 255, 255, 232};
int colorsB[23] = {  0,   0,   0,   0,   0,  68, 122, 184, 255, 255, 255, 255, 255, 255, 255, 176,  35,   0,   0,   0,   0,   0,   0};
      
unsigned long ledUpdateInterval     =   500;
unsigned long sendStatusInterval    = 30000;
unsigned long saveEEPROMInterval    = 60000;

unsigned long ledUpdateTime         =     0;
unsigned long sendStatusTime        = 20000;
unsigned long saveEEPROMTime        =     0;

unsigned long previousMillis = 0, currentMillis = 0, elapsedMillis = 0;

boolean bStarted = false;

// SoftwareSerial debugPort(8,9); // RX, TX


void initGlobals()
{
//  randr=random(256);
//  randg=random(256);
//  randb=random(256);
  randIndex = random(23);
  randr=colorsR[randIndex];
  randg=colorsG[randIndex];
  randb=colorsB[randIndex];
  J=0;
  K=0;
  L=0;
  x=0;
  y=0;
  z=0;
  STEP=0;
  STEP2=0;
}

void turnOnLight()
{
  if (!bLightOn)
  {
    bLightOn = true;
    digitalWrite(PIN_LIGHT, HIGH);
    sendIntToESP8266("ChangedLight", bLightOn?1:0);
  }
}

void turnOffLight()
{
  if (bLightOn)
  {
    bLightOn = false;
    digitalWrite(PIN_LIGHT, LOW);
    sendIntToESP8266("ChangedLight", bLightOn?1:0);
  }
}

void turnOnLed()
{
  if (!bLedOn)
  {
    bLedOn = true;
    sendIntToESP8266("ChangedLed", bLedOn?1:0);
  }
}

void turnOffLed()
{
  if (bLedOn)
  {
    bLedOn = false;
    sendIntToESP8266("ChangedLed", bLedOn?1:0);
  }
  
  for(int i=0; i<strip.numPixels(); i++) 
  {
    leds[i].r=0;
    leds[i].g=0;
    leds[i].b=0;
    strip.setPixelColor(i, 0);
  }
  strip.show();
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<71; i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
      strip.setPixelColor(141-i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }

}

void updateStateFromEEPROM()
{
  if(EEPROM.read(0) != 255)
  {
    if(EEPROM.read(0) == 0)
    {
      bLightOn = false;
      digitalWrite(PIN_LIGHT, LOW);
    }
    else if(EEPROM.read(0) == 1)
    {
      bLightOn = true;
      digitalWrite(PIN_LIGHT, HIGH);
    }
    
  }

  if(EEPROM.read(1) != 255)
  {
    if(EEPROM.read(1) == 0)
    {
      bLedOn = false;
    }
    else if(EEPROM.read(1) == 1)
    {
      bLedOn = true;
    }
  }

  if(EEPROM.read(2) != 255)
  {
    intLedMode = EEPROM.read(2);
    ledMode = (ledModes) intLedMode;
  }

  if(EEPROM.read(3) != 255)
  {
    ledSpeed = EEPROM.read(3);
    ledUpdateInterval = (unsigned long)(pow(2,(12-ledSpeed)));
  }
  
  colorRed   = EEPROM.read(4);
  colorGreen = EEPROM.read(5);
  colorBlue  = EEPROM.read(6);
}

void sendIntToESP8266(char *type, int value)
{
  StaticJsonBuffer<20> jsonBuffer;
  StaticJsonBuffer<50> jsonBuffer2;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& root2 = jsonBuffer2.createObject();

  // Send to ESP8266
  root[type] = value;

  root2["board2"] = root;
  root2.printTo(Serial);
  Serial.println();
}

void sendRGBStateToESP8266()
{
  char buf[20];
  StaticJsonBuffer<100> jsonBuffer;
  StaticJsonBuffer<120> jsonBuffer2;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& root2 = jsonBuffer2.createObject();

  sprintf(buf, "%d,%d,%d", colorRed, colorGreen, colorBlue);;

  root2["ChangedLedColor"] = buf;
  root2["board2"] = root;
  root2.printTo(Serial);
  Serial.println();
}

void sendAllStateToESP8266()
{
  char buf[20];
  StaticJsonBuffer<100> jsonBuffer;
  StaticJsonBuffer<120> jsonBuffer2;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& root2 = jsonBuffer2.createObject();

  sprintf(buf, "%d,%d,%d,%d,%d,%d,%d", bLightOn?1:0, bLedOn?1:0, intLedMode, ledSpeed, colorRed, colorGreen, colorBlue);

  root["StateAll"] = buf;
  root2["board2"] = root;
  root2.printTo(Serial);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
//  debugPort.begin(115200);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
//  rainbow(2);
  
  turnOffLed();

//  Serial.println("Starting...");
//  debugPort.println("Starting...");  
  
  pinMode(PIN_LIGHT, OUTPUT);
  digitalWrite(PIN_LIGHT, HIGH);

  updateStateFromEEPROM();
}

void loop() {
  int i=0,pos=0,opening_brace=0;
  char c, str[256]="";

  currentMillis  = millis();
  elapsedMillis  = currentMillis - previousMillis;
  previousMillis = currentMillis;
  ledUpdateTime  += elapsedMillis;
  sendStatusTime += elapsedMillis;
  saveEEPROMTime += elapsedMillis;

  if(Serial.available())
  {
    while (Serial.available())
    {
      c = (int) Serial.read();
      if (c == 123)
      {
        delay(4);
        break;
      }
    }

    if (c == 123) // {
    {
      str[pos++] = (char)c;
      opening_brace++;

      while(Serial.available())
      {        
        c = (int) Serial.read();
        if ((c >= 32) && (c <= 126))
        {
          str[pos++] = (char)c;
          
          // In case we get more starting braces than closing braces, then we
          // may overflow the str buffer.  As it is, since we dont expect long
          // input, cap the limit to 60.
          if (pos > 60)
          {
            break;
          }
        }

        if (c == 125)  // }
        {
          opening_brace--;
        }

        if (opening_brace == 0 )
        {
          str[pos++] = 0;

//          Serial.print("Received from ESP8266: ");
//          Serial.println(str);
//          debugPort.print("Received from ESP8266: ");
//          debugPort.println(str);

          StaticJsonBuffer<60> jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(str);

          if (root.success())
          {
            if(strstr(str,"LightCommand"))
            {
              lightCommand = root["LightCommand"];
              if (lightCommand == 0)
              {
                turnOffLight();
              }
              else if(lightCommand == 1)
              {
                turnOnLight();
              }
            }
            else if(strstr(str,"LedCommand"))
            {
              ledCommand = root["LedCommand"];

              if (ledCommand == 0)
              {
                turnOffLed();
              }
              else if(ledCommand == 1)
              {
                turnOnLed();
                bColorChanged = true;
              }
            } 
            else if(strstr(str,"LedSpeedCommand"))
            {
              ledSpeed = root["LedSpeedCommand"];
              sendIntToESP8266("ChangedLedSpeed", ledSpeed);
              ledUpdateInterval = (unsigned long)(pow(2,(12-ledSpeed)));
            } 
            else if(strstr(str,"LedModeCommand"))
            {
              intLedMode = root["LedModeCommand"];
              sendIntToESP8266("ChangedLedMode", intLedMode);
              ledMode = (ledModes) intLedMode;
              initGlobals();
              turnOnLed();
              bColorChanged = true;
            } 
            else if(strstr(str,"Red"))
            {
              colorRed   = root["Red"];
              colorGreen = root["Green"];
              colorBlue  = root["Blue"];
              turnOnLed();
              bColorChanged = true;

              sendRGBStateToESP8266();
              
              if ((ledMode != LedModeDefault) && 
                  (ledMode != LedModeNegative) && 
                  (ledMode != LedModeFade) && 
                  (ledMode != LedModeCycle2) && 
                  (ledMode != LedModeCycle2) && 
                  (ledMode != LedModeCycle3) && 
                  (ledMode != LedModeCycle4) && 
                  (ledMode != LedModeStrobe) &&
                  (ledMode != LedModeStrobeStep) &&
                  (ledMode != LedModeStrobeStep2) &&
                  (ledMode != LedModeLarsonScanner))
              {
                intLedMode = 0;
                ledMode = LedModeDefault;
              }
            }
          }
          break;
        }
      }
    }
  }    

  if (ledUpdateTime >= ledUpdateInterval)
  {
//    long randr = 0, randg = 0, randb = 0;
    ledUpdateTime = 0;
    
    if(bLedOn)
    {
      switch (ledMode)
      {
      case LedModeDefault:
        if (bColorChanged)
        {
          for(i=0; i<strip.numPixels(); i++) 
          {
            strip.setPixelColor(i, colorRed, colorGreen, colorBlue);
          }
          strip.show();
          bColorChanged = false;      
        }
        break;
      case LedModeNegative:
        if (bColorChanged)
        {
          for(i=0; i<strip.numPixels(); i++) 
          {
            strip.setPixelColor(i, 255 - colorRed, 255 - colorGreen, 255 - colorBlue);
          }
          strip.show();
          bColorChanged = false;      
        }
        break;
      case LedModeFade:
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, (int)(colorRed * (100-STEP2)/100), (int)(colorGreen * (100-STEP2)/100), (int)(colorBlue * (100-STEP2)/100));
         }
        if(STEP == 0)
        {
          STEP2 = STEP2++;
          if (STEP2 > 99)
          {
            STEP = 1;
          }
        }
        else if(STEP == 1)
        {
          STEP2 = STEP2--;
          if (STEP2 < 1)
          {
            STEP = 0;
          }
        }
        strip.show();
        break;
      case LedModeRainbow:      
        if(J++ > 255)
        {
          J = 0;
        }    
        strip.setPixelColor(LED_COUNT-1, 0, 0, 0);

        for(i=0; i<71; i++) 
        {
          strip.setPixelColor(i, Wheel((i+J) & 255));
          strip.setPixelColor(141-i, Wheel((i+J) & 255));
        }
        strip.show();
        break;
      case LedModeCycle:      
        if (randIndex <22)
        {
          randIndex++;
        }
        else
        {
          randIndex = 0;
        }

        randr=colorsR[randIndex];
        randg=colorsG[randIndex];
        randb=colorsB[randIndex];
      
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, randr, randg, randb);
        }
        strip.show();
        break;
      case LedModeCycle2:      
        if(K==0)
        {
          K = 1;
        }    
        else if(K==1)
        {
          K = 0;
        }    

        for(i=K; i<strip.numPixels(); i=i+2) 
        {
          strip.setPixelColor(i, 0);
          strip.setPixelColor(i+1, colorRed, colorGreen, colorBlue);
        }
        strip.show();
        break;
      case LedModeCycle3:      
        L++;
        if (L==LED_COUNT)
        {
          L=0;
        }

        for(i=0;i<LED_COUNT;i++)
        {        
          if (i==L)
          {
            strip.setPixelColor(i, colorRed, colorGreen, colorBlue);
          }
          else
          {
            strip.setPixelColor(i, 0, 0, 0);
          }
        }
        strip.show();
        break;
/*      case LedModeCycle3:      
        L--;
        if (L==-1)
        {
          L=LED_COUNT-1;
        }

        for(i=LED_COUNT-1;i>=0;i--)
        {        
          if (i==L)
          {
            strip.setPixelColor(i, colorRed, colorGreen, colorBlue);
          }
          else
          {
            strip.setPixelColor(i, 0, 0, 0);
          }
        }
        strip.show();
        break;
*/
      case LedModeCycle4:      
        switch(STEP)
        {
          case 0:
            L++;
            if (L >= (LED_COUNT-1))
            {
              L=0;
              STEP++;
            }

            for(i=0;i<=L;i++)
            {        
              strip.setPixelColor(i, colorRed, colorGreen, colorBlue);
            }
          break;
          case 1:
            L++;
            if (L >= (LED_COUNT-1))
            {
              L=0;
              STEP++;
            }

            for(i=0;i<=L;i++)
            {        
              strip.setPixelColor(i, 0, 0, 0);
            }
          break;
        }
        
        if (STEP>1)
        {
          STEP=0;
        }
        strip.show();
        break;
      case LedModeRandom:  
        randIndex = random(23);
        randr=colorsR[randIndex];
        randg=colorsG[randIndex];
        randb=colorsB[randIndex];
      
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, randr, randg, randb);
        }
        strip.show();
        break;
      case LedModeRandom2:  
        x = random(LED_COUNT);

        for(i=0; i<strip.numPixels(); i++) 
        {
          randIndex = random(23);
          randr=colorsR[randIndex];
          randg=colorsG[randIndex];
          randb=colorsB[randIndex];
          
          if(i==x)
          {
            strip.setPixelColor(i, randr, randg, randb);
          }
          else
          {
            strip.setPixelColor(i, 0, 0, 0);
          }
        }
        strip.show();
        break;
      case LedModeRandomCycle:
        for(i=0; i<strip.numPixels(); i++) 
        {
          randIndex = random(23);
          randr=colorsR[randIndex];
          randg=colorsG[randIndex];
          randb=colorsB[randIndex];
          strip.setPixelColor(i, randr, randg, randb);
        }
        strip.show();
        break;
      case LedModeRandomCycle2:
        for(i=strip.numPixels()-1; i>0; i--) 
        {
         leds[i] = leds[i-1];
         strip.setPixelColor(i, leds[i].r, leds[i].g, leds[i].b);
        }

        randIndex = random(23);
        randr=colorsR[randIndex];
        randg=colorsG[randIndex];
        randb=colorsB[randIndex];
        
        leds[0].r = randr;
        leds[0].g = randg;
        leds[0].b = randb;
        strip.setPixelColor(0, leds[0].r, leds[0].g, leds[0].b);
        strip.show();
        break;
      case LedModeStrobe:
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, colorRed, colorGreen, colorBlue);
        }
        strip.show();
        delay(5);
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, 0, 0, 0);
        }
        strip.show();
        break;
      case LedModeStrobeRandom:
        randIndex = random(23);
        randr=colorsR[randIndex];
        randg=colorsG[randIndex];
        randb=colorsB[randIndex];
        
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, randr, randg, randb);
        }
        strip.show();
        delay(5);
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, 0, 0, 0);
        }
        strip.show();
        break;
      case LedModeStrobeStep:
        if (STEP == 0)
        {
          strip.setPixelColor(142, colorRed, colorGreen, colorBlue);
        }
        else if(STEP == 6)
        {
          strip.setPixelColor(70, colorRed, colorGreen, colorBlue);
          strip.setPixelColor(71, colorRed, colorGreen, colorBlue);
        } 
        for(i=0; i<10; i++) 
        {
          strip.setPixelColor(STEP*10 + i, colorRed, colorGreen, colorBlue);
          strip.setPixelColor(141-(STEP*10 + i), colorRed, colorGreen, colorBlue);
        }
        strip.show();
        delay(5);
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, 0, 0, 0);
        }
        if (++STEP>6)
        {
          STEP=0;
        }
        strip.show();
        break;
      case LedModeStrobeStep2:
        if (STEP2 == 0)
        {
          strip.setPixelColor(142, colorRed, colorGreen, colorBlue);
        }
        else if(STEP2 == 6)
        {
          strip.setPixelColor(70, colorRed, colorGreen, colorBlue);
          strip.setPixelColor(71, colorRed, colorGreen, colorBlue);
        } 
        for(i=0; i<10; i++) 
        {
          strip.setPixelColor(STEP2*10 + i, colorRed, colorGreen, colorBlue);
          strip.setPixelColor(141-(STEP2*10 + i), colorRed, colorGreen, colorBlue);
        }
        strip.show();
        delay(5);
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, 0, 0, 0);
        }
        if (STEP == 0)
        { 
          if(++STEP2 > 5)
          {
            STEP=1;
          }
        }
        else if (STEP == 1)
        { 
          if(--STEP2 < 1)
          {
            STEP=0;
          }
        }
        strip.show();
        break;
      case LedModeStrobeStepRandom:
        STEP = random(7);
        if (STEP == 0)
        {
          strip.setPixelColor(142, colorRed, colorGreen, colorBlue);
        }
        else if(STEP == 6)
        {
          strip.setPixelColor(70, colorRed, colorGreen, colorBlue);
          strip.setPixelColor(71, colorRed, colorGreen, colorBlue);
        } 
        for(i=0; i<10; i++) 
        {
          strip.setPixelColor(STEP*10 + i, colorRed, colorGreen, colorBlue);
          strip.setPixelColor(141-(STEP*10 + i), colorRed, colorGreen, colorBlue);
        }
        strip.show();
        delay(5);
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, 0, 0, 0);
        }
        strip.show();
        break;
      case LedModeStrobeStepRandom2:
        STEP = random(7);
        randIndex = random(23);

        randr=colorsR[randIndex];
        randg=colorsG[randIndex];
        randb=colorsB[randIndex];
        
        if (STEP == 0)
        {
          strip.setPixelColor(142, randr, randg, randb);
        }
        else if(STEP == 6)
        {
          strip.setPixelColor(70, randr, randg, randb);
          strip.setPixelColor(71, randr, randg, randb);
        } 
        for(i=0; i<10; i++) 
        {
          strip.setPixelColor(STEP*10 + i, randr, randg, randb);
          strip.setPixelColor(141-(STEP*10 + i), randr, randg, randb);
        }
        strip.show();
        delay(5);
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, 0, 0, 0);
        }
        strip.show();
        break;
      case LedModeLarsonScanner:
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, 0, 0, 0);
        }

        if (L<80)
          strip.setPixelColor((LED_COUNT-2) - (L -10), (int)(colorRed * 10/100), (int)(colorGreen * 10/100), (int)(colorBlue * 10/100));
        if (L<79)
          strip.setPixelColor((LED_COUNT-2) - (L - 9), (int)(colorRed * 30/100), (int)(colorGreen * 30/100), (int)(colorBlue * 30/100));
        if (L<78)
          strip.setPixelColor((LED_COUNT-2) - (L - 8), (int)(colorRed * 50/100), (int)(colorGreen * 50/100), (int)(colorBlue * 50/100));
        if (L<77)
          strip.setPixelColor((LED_COUNT-2) - (L - 7), (int)(colorRed * 70/100), (int)(colorGreen * 70/100), (int)(colorBlue * 70/100));
        if (L<76)
          strip.setPixelColor((LED_COUNT-2) - (L - 6), (int)(colorRed * 90/100), (int)(colorGreen * 90/100), (int)(colorBlue * 90/100));
        if (L<75)
          strip.setPixelColor((LED_COUNT-2) - (L - 5), colorRed,      colorGreen,      colorBlue);
        if (L<74)
          strip.setPixelColor((LED_COUNT-2) - (L - 4), (int)(colorRed * 90/100), (int)(colorGreen * 90/100), (int)(colorBlue * 90/100));
        if (L<73)
          strip.setPixelColor((LED_COUNT-2) - (L - 3), (int)(colorRed * 70/100), (int)(colorGreen * 70/100), (int)(colorBlue * 70/100));
        if (L<72)
          strip.setPixelColor((LED_COUNT-2) - (L - 2), (int)(colorRed * 50/100), (int)(colorGreen * 50/100), (int)(colorBlue * 50/100));
        if (L<71)
          strip.setPixelColor((LED_COUNT-2) - (L - 1), (int)(colorRed * 30/100), (int)(colorGreen * 30/100), (int)(colorBlue * 30/100));
        if (L<70)
          strip.setPixelColor((LED_COUNT-2) - L,      (int)(colorRed * 10/100), (int)(colorGreen * 10/100), (int)(colorBlue * 10/100));


        if (L<79)
          strip.setPixelColor(L - 7, (int)(colorRed * 70/100), (int)(colorGreen * 70/100), (int)(colorBlue * 70/100));
        if (L<78)
          strip.setPixelColor(L - 6, (int)(colorRed * 90/100), (int)(colorGreen * 90/100), (int)(colorBlue * 90/100));
        if (L<77)
          strip.setPixelColor(L - 5, colorRed,      colorGreen,      colorBlue);
        if (L<76)
          strip.setPixelColor(L - 4, (int)(colorRed * 90/100), (int)(colorGreen * 90/100), (int)(colorBlue * 90/100));
        if (L<75)
          strip.setPixelColor(L - 3, (int)(colorRed * 70/100), (int)(colorGreen * 70/100), (int)(colorBlue * 70/100));

        if(STEP == 0)
        {
          if (++L > 81)
          {
            STEP = 1;
          }
        }
        else if(STEP == 1)
        {
          if (--L < 0)
          {
            STEP = 0;
          }
        }
        strip.show();
        break;
      case LedModeLarsonScannerRandom:
        for(i=0; i<strip.numPixels(); i++) 
        {
          strip.setPixelColor(i, 0, 0, 0);
        }

        if (L<80)
          strip.setPixelColor((LED_COUNT-2) - (L -10), (int)(randr * 10/100), (int)(randg * 10/100), (int)(randb * 10/100));
        if (L<79)
          strip.setPixelColor((LED_COUNT-2) - (L - 9), (int)(randr * 30/100), (int)(randg * 30/100), (int)(randb * 30/100));
        if (L<78)
          strip.setPixelColor((LED_COUNT-2) - (L - 8), (int)(randr * 50/100), (int)(randg * 50/100), (int)(randb * 50/100));
        if (L<77)
          strip.setPixelColor((LED_COUNT-2) - (L - 7), (int)(randr * 70/100), (int)(randg * 70/100), (int)(randb * 70/100));
        if (L<76)
          strip.setPixelColor((LED_COUNT-2) - (L - 6), (int)(randr * 90/100), (int)(randg * 90/100), (int)(randb * 90/100));
        if (L<75)
          strip.setPixelColor((LED_COUNT-2) - (L - 5), randr,      randg,      randb);
        if (L<74)
          strip.setPixelColor((LED_COUNT-2) - (L - 4), (int)(randr * 90/100), (int)(randg * 90/100), (int)(randb * 90/100));
        if (L<73)
          strip.setPixelColor((LED_COUNT-2) - (L - 3), (int)(randr * 70/100), (int)(randg * 70/100), (int)(randb * 70/100));
        if (L<72)
          strip.setPixelColor((LED_COUNT-2) - (L - 2), (int)(randr * 50/100), (int)(randg * 50/100), (int)(randb * 50/100));
        if (L<71)
          strip.setPixelColor((LED_COUNT-2) - (L - 1), (int)(randr * 30/100), (int)(randg * 30/100), (int)(randb * 30/100));
        if (L<70)
          strip.setPixelColor((LED_COUNT-2) - L,       (int)(randr * 10/100), (int)(randg * 10/100), (int)(randb * 10/100));


        if (L<79)
          strip.setPixelColor(L - 7, (int)(randr * 70/100), (int)(randg * 70/100), (int)(randb * 70/100));
        if (L<78)
          strip.setPixelColor(L - 6, (int)(randr * 90/100), (int)(randg * 90/100), (int)(randb * 90/100));
        if (L<77)
          strip.setPixelColor(L - 5, randr,      randg,      randb);
        if (L<76)
          strip.setPixelColor(L - 4, (int)(randr * 90/100), (int)(randg * 90/100), (int)(randb * 90/100));
        if (L<75)
          strip.setPixelColor(L - 3, (int)(randr * 70/100), (int)(randg * 70/100), (int)(randb * 70/100));
        strip.show();

 
        if(STEP == 0)
        {
          if (++L > 81)
          {
            randIndex = random(23);
            randr=colorsR[randIndex];
            randg=colorsG[randIndex];
            randb=colorsB[randIndex];
        
            STEP = 1;
          }
        }
        else if(STEP == 1)
        {
          if (--L < 0)
          {
            randIndex = random(23);
            randr=colorsR[randIndex];
            randg=colorsG[randIndex];
            randb=colorsB[randIndex];
        
            STEP = 0;
          }
        }
        strip.show();
        break;
      }
    }
  }
  
  if (sendStatusTime > sendStatusInterval)
  {
    sendStatusTime = 0;
    
    if(!bStarted)
    {
      bStarted = true;
      sendIntToESP8266("ArduinoStarting", 1);
    }
    sendAllStateToESP8266();
  }
  
  if (saveEEPROMTime > saveEEPROMInterval)
  {
    saveEEPROMTime = 0;
    if (prev_bLightOn != bLightOn)
    {
      prev_bLightOn = bLightOn;
      EEPROM.write(0, bLightOn?1:0);
    }

    if (prev_bLedOn != bLedOn)
    {
      prev_bLedOn = bLedOn;
      EEPROM.write(1, bLedOn?1:0);
    }

    if (prev_intLedMode != intLedMode)
    {
      prev_intLedMode = intLedMode;
      EEPROM.write(2, intLedMode);
    }
    
    if (prev_ledSpeed != ledSpeed)
    {
      prev_ledSpeed = ledSpeed;
      EEPROM.write(3, ledSpeed);
    }

    if (prev_colorRed != colorRed)
    {
      prev_colorRed = colorRed;
      EEPROM.write(4, colorRed);
    }

    if (prev_colorGreen != colorGreen)
    {
      prev_colorGreen = colorGreen;
      EEPROM.write(5, colorGreen);
    }

    if (prev_colorBlue != colorBlue)
    {
      prev_colorBlue = colorBlue;
      EEPROM.write(6, colorBlue);
    }
  }
  delay(1);
}


