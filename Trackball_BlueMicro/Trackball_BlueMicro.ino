/* Trackball_BlueMicro.ino
    Sketch that is an adaptation of the pimoroni trackball's Leonardo trackball example
    and nRF52 hid_mouse example.

    Written by William Schaffer
    Created: 9/26/2022
    Last Modified: 9/26/2022
*/

#include <bluefruit.h>
#include <Wire.h>
#include <pimoroniTrackball.h>  //From https://github.com/ncmreynolds/pimoroniTrackball
#include <DailyStruggleButton.h>

BLEDis bledis;
BLEHidAdafruit blehid;

// Define buttons
DailyStruggleButton buttonA;
DailyStruggleButton buttonB;

// Define button pushing variables
unsigned int longTime = 1000;
unsigned int multTime = 400;
byte multiHit = 2; // May change to allow for various multi-hits

// Define trackball variables
uint8_t mouseSpeed = 10; //Change this to alter the mouse speed
int16_t x = 0;
int16_t y = 0;

void setup() 
{
  // Enable Vout
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  delay(1000);
   
  // Initialize I2C and trackball
  Wire.begin();
  trackball.begin();
  
  while(!trackball.isConnected()) delay(100);

  // Initialize buttons
  buttonA.set(38, buttonA_Event, INT_PULL_UP);
  buttonB.set(43, buttonB_Event, INT_PULL_UP);

  // Add multi press and long press later

  // Bluetooth stuff
  Bluefruit.begin();
  
  // HID Device can have a min connection interval of 9*1.25 = 11.25 ms
  Bluefruit.Periph.setConnInterval(9, 16); // min = 9*1.25=11.25 ms, max = 16*1.25=20ms
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit52");

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather 52");
  bledis.begin();

  // BLE HID
  blehid.begin();

  // Set up and start advertising
  startAdv();
}

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_MOUSE);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for 'Name' in the advertising packet
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void loop() 
{    
  // Poll buttons
  buttonA.poll();
  buttonB.poll();
  
  // Trackball checking
  if(trackball.changed()) {
    x = (trackball.right() - trackball.left()) * mouseSpeed;
    y = (trackball.down() - trackball.up()) * mouseSpeed;
    if(x != 0 || y != 0) {
      blehid.mouseMove(x, y);
    }
    if(trackball.click())
    {
      blehid.mouseButtonPress(MOUSE_BUTTON_LEFT);
    }
    else if(trackball.release())
    {
      blehid.mouseButtonRelease();
    }
  }
}

// Button A event handling
void buttonA_Event (byte btnStatus)
{
  // We can use switch/case to run through what happens for each event
  switch (btnStatus){

    case onPress:
      blehid.mouseButtonPress(MOUSE_BUTTON_RIGHT);
      break;
      
    case onRelease:
      blehid.mouseButtonRelease();
      break;
      
    case onLongPress:
      
      break;

    case onMultiHit:
      
      break;
  }
}

// Button B event handling
void buttonB_Event (byte btnStatus)
{
  // We can use switch/case to run through what happens for each event
  switch (btnStatus){

    case onPress:
      blehid.mouseButtonPress(MOUSE_BUTTON_MIDDLE);
      break;
      
    case onRelease:
      blehid.mouseButtonRelease();
      break;
      
    case onLongPress:
      
      break;

    case onMultiHit:
      
      break;
  }
}
