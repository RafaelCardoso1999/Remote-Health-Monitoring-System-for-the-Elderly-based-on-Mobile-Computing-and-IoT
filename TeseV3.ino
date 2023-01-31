/*
 An example digital clock using a TFT LCD screen to show the time.
 Demonstrates use of the font printing routines. (Time updates but date does not.)

 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo.

 This examples uses the hardware SPI only. Non-hardware SPI
 is just too slow (~8 times slower!)

 Based on clock sketch by Gilchrist 6/2/2014 1.0
 Updated by Bodmer
A few colour codes:

code    color
0x0000  Black
0xFFFF  White
0xBDF7  Light Gray
0x7BEF  Dark Gray
0xF800  Red
0xFFE0  Yellow
0xFBE0  Orange
0x79E0  Brown
0x7E0   Green
0x7FF   Cyan
0x1F    Blue
0xF81F  Pink

 */
#include "config.h"


//---------------------------------------------
#include <WiFi.h>

#define BROKER_IP    "10.92.11.145"
#define DEV_NAME     "device_random_name"
#define MQTT_USER    ""
#define MQTT_PW      ""

#define SerialMon                   Serial

const char ssid[] = "ISCTE-IUL-Guest";
const char pass[] = "BEM89su2ced5ido";

#include <MQTT.h>

WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;
//---------------------

TTGOClass *ttgo;
TFT_eSPI *tft;
BMA *sensor;

TinyGPSPlus *gps;

Adafruit_DRV2605 *drv;
uint8_t effect = 16;


boolean possibleFall = 0; //queda Possivel


int acc_1 = 50; //generic number to initialize the acceleration
int nquedas=0; // Number of falls
String lng = (String) 0;  //Longitude
String lat = (String) 0;  //Latitude

String SpO2 = ""; 
String BpM = "";

lv_obj_t * bar1;

int iterador = 0; //iterator to control the vibration process during the 20s period after a possible fall detected, so as not to be constantly vibrating;

uint32_t targetTime = 0;       // for next 1 second timeout

byte omm = 99;
boolean initial = 1;
byte xcolon = 0;
unsigned int colour = 0;

static uint8_t conv2d(const char *p)
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time


//---------------------------
void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("-");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(DEV_NAME, MQTT_USER, MQTT_PW)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe("/BpM"); //SUBSCRIBE TO TOPIC /BpM
  client.subscribe("/SpO2"); //SUBSCRIBE TO TOPIC /SpO2

}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  
  if(topic == "/BpM"){
    Serial.println("Recebi um BpM");
    BpM = payload;
  }
  if(topic == "/SpO2"){
    Serial.println("Recebi um SpO2");
    SpO2 = payload;
  }
}
//---------------------------------------------------


static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        Serial.printf("Falso positivo confirmado\n");
        possibleFall = 0;
    } 
}


void setup(void)
{
  Serial.begin(115200);

  //---------------------------------------------------------

  WiFi.begin(ssid, pass);

  client.begin(BROKER_IP, 1883, net);
  client.onMessage(messageReceived);
  connect();

  //---------------------------------
  
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    ttgo->tft->setTextFont(1);
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour
    targetTime = millis() + 1000;

  nquedas=0;  
  

    //Receive objects for easy writing
    tft = ttgo->tft;
    sensor = ttgo->bma;


    gps = ttgo->gps;
    ttgo->trunOnGPS();
    ttgo->gps_begin();
    gps =  ttgo->gps;

    


     drv = ttgo->drv;
     ttgo->lvgl_begin();

       ttgo->enableDrv2650();

    drv->selectLibrary(1);

    // I2C trigger by sending 'go' command
    // default, internal trigger when sending GO command
    drv->setMode(DRV2605_MODE_INTTRIG);

    // Accel parameter structure
    Acfg cfg;

    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;

    cfg.range = BMA4_ACCEL_RANGE_2G;
 
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
 
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;

    // Configure the BMA423 accelerometer
    sensor->accelConfig(cfg);

    // Enable BMA423 accelerometer
    sensor->enableAccel();

    // You can also turn it off
    // sensor->disableAccel();
   
    lv_obj_set_style_local_bg_color (lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    lv_obj_t *label;

        
        lv_obj_t *btn1 = lv_btn_create(lv_scr_act(), NULL);
        lv_obj_set_event_cb(btn1, event_handler);
        lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, 90);
    
        label = lv_label_create(btn1, NULL);
        lv_label_set_text(label, "NO FALL");


        bar1 = lv_bar_create(lv_scr_act(), NULL);
        lv_obj_set_size(bar1, 150, 20);
        lv_obj_align(bar1, NULL, LV_ALIGN_CENTER, 0, 45);
        lv_bar_set_anim_time(bar1, 1000);
        lv_bar_set_value(bar1, 0, LV_ANIM_ON);
        

    // Some display settings
    
    //tft->setTextColor(random(0xFFFF));
    //tft->drawString("BMA423 accel",  25, 50, 4);
    tft->setTextFont(4);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);

  
}

void loop()
{
//----------------------------


client.loop();
  if (!client.connected()) {
    connect();
  }


showClock(); //Show the current time and date on the watch;

 while (ttgo->hwSerial->available()) {
        int r = ttgo->hwSerial->read();
        ttgo->gps->encode(r);
    }
    display_info();


 Accel acc;


    
    // Get acceleration data
    bool res = sensor->getAccel(acc);

    if (res == false) {
        Serial.println("getAccel FAIL");
    } else {
        // Show the data
         
        int new_x= acc.x;
        new_x = map(new_x, -1200, 1200, 0, 100);
        client.publish("/x", (String) new_x);
        
        int new_y= acc.y;
        new_y = map(new_y, -1200, 1200, 0, 100);
        client.publish("/y", (String) new_y);
        
        int new_z= acc.z;
        new_z = map(new_z, -1200, 1200, 0, 100);
        client.publish("/z", (String) new_z);
    

     //Magnitude of Acceleration
        tft->setCursor(80, 100); 
        int acc_avg = sqrt((new_x * new_x) + (new_y * new_y) + (new_z * new_z));
        int diff = abs(acc_avg-acc_1);
        lv_bar_set_value(bar1, diff, LV_ANIM_ON);
        client.publish("/acc", (String) diff); //PUBLISH TO TOPIC /acc
        acc_1 = acc_avg;

       
       client.publish("/fall", (String) nquedas); //PUBLISH TO TOPIC /nquedas
        
        if( diff < 70){
          
          }else{
          
           ConfirmFall();
       
          }
           
     lv_task_handler();


     if(possibleFall == 1){
          iterador = iterador + 1;
          if(iterador % 10 == 0){
            drv->go();
            Serial.println("Vibracao");
          // wait a bit
            delay(100);     
             drv->setWaveform(1, 0);       // end waveform

          }
          
          if(iterador >= 99){ //+- 20s
              nquedas = nquedas + 1;
              client.publish("/Fall Alert!!", "O utilizador sofreu uma queda!"); //PUBLISH TO TOPIC /FALL ALERT
              possibleFall = 0;
          }
     }else{ // User carrega no botão
           iterador = 0;
     }
     
     

     delay(200);

        tft->fillRect(0, 85, 250, 53, TFT_BLACK);


        tft->setCursor(50, 85);
        ttgo->tft->setTextColor(TFT_ORANGE, TFT_BLACK); // Orange
        tft->print("SpO2:"); 
        ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK); // White
        tft->println(SpO2);
        ttgo->tft->setTextColor(TFT_ORANGE, TFT_BLACK); // Orange
        tft->setCursor(50, 110);
        tft->print("BpM:"); 
        ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK); // White
        tft->println(BpM);



     
    }
    
}



  void ConfirmFall(){


    // set the effect to play
          drv->setWaveform(0, effect);  // play effect

          // play the effect!
            drv->go();
            Serial.println("AQUI");
      
          // wait a bit
            delay(200);
      

        drv->setWaveform(1, 0);       // end waveform


        possibleFall=1;
        

    }



void showClock(){

      //-----------------------------
    if (targetTime < millis()) {
        targetTime = millis() + 1000;
        ss++;              // Advance second
        if (ss == 60) {
            ss = 0;
            omm = mm;
            mm++;            // Advance minute
            if (mm > 59) {
                mm = 0;
                hh++;          // Advance hour
                if (hh > 23) {
                    hh = 0;
                }
            }
        }

        if (ss == 0 || initial) {
            initial = 0;
            ttgo->tft->setTextColor(TFT_GREEN, TFT_BLACK);
            ttgo->tft->setCursor (8, 52);
            ttgo->tft->print(__DATE__); // This uses the standard ADAFruit small font

      
        }

        // Update digital time
        byte xpos = 6;
        byte ypos = 0;
        if (omm != mm) { // Only redraw every minute to minimise flicker
            // Uncomment ONE of the next 2 lines, using the ghost image demonstrates text overlay as time is drawn over it
            ttgo->tft->setTextColor(0x39C4, TFT_BLACK);  // Leave a 7 segment ghost image, comment out next line!
            ttgo->tft->setTextColor(TFT_BLACK, TFT_BLACK); // Set font colour to black to wipe image
            // Font 7 is to show a pseudo 7 segment display.
            // Font 7 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
            ttgo->tft->drawString("88:88", xpos, ypos, 7); // Overwrite the text to clear it
            ttgo->tft->setTextColor(0xFBE0, TFT_BLACK); // Orange
            omm = mm;

            if (hh < 10) xpos += ttgo->tft->drawChar('0', xpos, ypos, 7);
            xpos += ttgo->tft->drawNumber(hh, xpos, ypos, 7);
            xcolon = xpos;
            xpos += ttgo->tft->drawChar(':', xpos, ypos, 7);
            if (mm < 10) xpos += ttgo->tft->drawChar('0', xpos, ypos, 7);
            ttgo->tft->drawNumber(mm, xpos, ypos, 7);
        }    
    }

    }





    static void display_info(void)
{
    static uint32_t updatePeriod;
    if (millis() - updatePeriod < 20000) { // Está em 20 segundos
        return;
    }
    updatePeriod = millis();
    if (gps->location.isUpdated()) {
         
         float latsmall= gps->location.lat();
         lat = String(latsmall,6);

         float lngsmall= gps->location.lng();
         lng = String(lngsmall,6);
         
         Serial.println("Latitude pls: ");
         Serial.println((String) lat);
         Serial.println("Longitude pls: ");
         Serial.println((String) lng);

         String coord = ((String) lat ) + ',' + ((String) lng );
         Serial.println("Coordinates:");
         Serial.println(coord);
         
         client.publish("/coord", coord); //PUBLISH TO TOPIC /coord


    }
    delay(20);
}
