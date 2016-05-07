IMPORTANT:

Used arduino 1.0.5-rc2 to upload this sketch.

When arduino 1.7.7 was used, the Fade mode was not working.  Something in the following code does not work in 1.7.7

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







http://tuanpm.net/rock-solid-esp8266-wifi-mqtt-restful-client-for-arduino/


C:\work\work2\ESP8266\espduino-master\esp8266\release>C:\Python27\python.exe c:\
Espressif\utils\esptool.py -p COM5 write_flash 0x00000 0x00000.bin 0x40000 0x400
00.bin
Entering bootloader...
Connecting...
MAC AP: 1A-FE-34-9D-8F-26
MAC STA: 18-FE-34-9D-8F-26
Erasing flash...
Writing at 0x00008400... (100 %)
Erasing flash...
Writing at 0x0006bc00... (100 %)

Leaving...

C:\work\work2\ESP8266\espduino-master\esp8266\release>










Received on topic: /socket3/LedCommand, data: 1 
{
"LedCommand":1
}TCP: data received 24 bytes
Received on topic: /socket3/LedCommand, data: 0 
{
"LedCommand":0
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 2
 
{
"LedModeCommand":2
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 3
 
{
"LedModeCommand":3
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 4
 
{
"LedModeCommand":4
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 5
 
{
"LedModeCommand":5
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 6
 
{
"LedModeCommand":6
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 7
 
{
"LedModeCommand":7
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 8
 
{
"LedModeCommand":8
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 9
 
{
"LedModeCommand":9
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 0
 
{
"LedModeCommand":0
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 1
 
{
"LedModeCommand":1
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 2
 
{
"LedModeCommand":2
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 3
 
{
"LedModeCommand":3
}TCP: data received 29 bytes
Received on topic: /socket3/LedModeCommand, data: 4
 
{
"LedModeCommand":4
}TCP: data received 99 bytes
Received on topic: /socket3/LedColorCommand, data: setSocket3RGB {{LedColorRed: 255}, {LedColorGreen:0}, {LedColorBlue:3}} 
{
"LedColorRed":255,
"LedColorGreen":0,
"LedColorBlue":3
}TCP: data received 100 bytes
Received on topic: /socket3/LedColorCommand, data: setSocket3RGB {{LedColorRed: 0}, {LedColorGreen:255}, {LedColorBlue:47}} 
{
"LedColorRed":0,
"LedColorGreen":255,
"LedColorBlue":47
}


{
"LedModeCommand":4
}

{
"socket3":
{
"Light":1,
"Led":1,
"LedMode":19,
"LedSpeed":5,
"LedColorRed":33,
"LedColorGreen":66,
"LedColorBlue":99
}
}


{"socket3":{"Light":0,"Led":1,"LedMode":5,"LedSpeed":7,"LedColorRed":255,"LedColorGreen":0,"LedColorBlue":25}}
Received from ESP8266: { "LedModeCommand":4 }
{"socket3":{"ChangedLedMode":4}}
{"socket3":{"Light":0,"Led":1,"LedMode":4,"LedSpeed":7,"LedColorRed":255,"LedColorGreen":0,"LedColorBlue":25}}
Received from ESP8266: { "LedSpeedCommand":4 }
{"socket3":{"ChangedLedSpeed":4}}
Received from ESP8266: { "LedSpeedCommand":4 }
{"socket3":{"ChangedLedSpeed":4}}
Received from ESP8266: { "LedModeCommand":4 }
{"socket3":{"ChangedLedMode":4}}
{"socket3":{"Light":0,"Led":1,"LedMode":4,"LedSpeed":4,"LedColorRed":255,"LedColorGreen":0,"LedColorBlue":25}}
Received from ESP8266: { "LightCommand":1 }
{"socket3":{"ChangedLight":1}}
{"socket3":{"Light":1,"Led":1,"LedMode":4,"LedSpeed":4,"LedColorRed":255,"LedColorGreen":0,"LedColorBlue":25}}

{"socket3":{"Light":1,"Led":0,"LedMode":4,"LedSpeed":4,"LedColorRed":255,"LedColorGreen":0,"LedColorBlue":25}}




{"socket3":{"Light":1,"Led":0,"LedMode":9,"LedSpeed":5,"LedColorRed":255,"LedColorGreen":0,"LedColorBlue":25}}




{"socket3":{"Light":1,"Led":1,"LedMode":9,"LedSpeed":5,"LedColorRed":255,"LedColorGreen":0,"LedColorBlue":25}}

{"socket3":{"Light":1,"LedMode":9,"LedSpeed":5,"LedColorRed":255,"LedColorGreen":0,"LedColorBlue":25,"Led":1}}

