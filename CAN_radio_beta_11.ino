//
//

#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 200;

#define CAN0_INT 2        // Set INT to pin 2
MCP_CAN CAN0(10);         // Set CS to pin 10
#define LIST 3  // List button on steering wheel, corresponds to the "play/pause"
#define PREV 4
#define NEXT 5

void setup()
{
  Serial.begin(115200);
  
  if(CAN0.begin(MCP_STDEXT, CAN_125KBPS, MCP_16MHZ) == CAN_OK); // STDEXT mode makes filtering of data bytes possible, STD only filters IDs
   Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
 
  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
    
  CAN0.init_Mask(0,0,0x07FFFF00);                // Init first mask...
  CAN0.init_Filt(0,0,0x021F0100);                // Init first filter...
  CAN0.init_Filt(1,0,0x021F4000);                // Init second filter...
  
  CAN0.init_Mask(1,0,0x07FFFF00);                // Init second mask... 
  CAN0.init_Filt(2,0,0x021F8000);                // Init third filter...
//  CAN0.init_Filt(3,0,0x021F0000);                // Init fouth filter...
//  CAN0.init_Filt(4,0,0x021F0000);                // Init fifth filter...
//  CAN0.init_Filt(5,0,0x021F0000);                // Init sixth filter... 
  
  CAN0.setMode(MCP_LISTENONLY);                // Change to normal mode to allow messages to be transmitted
  
  pinMode(LIST, OUTPUT);
  pinMode(NEXT, OUTPUT);
  pinMode(PREV, OUTPUT);    

  delay(2000); //wait for CSR8635 to booutp, probably too long but who cares
  digitalWrite(LIST, HIGH); // power up CSR8635 which is set to connect on bootup
  delay(1200);
  digitalWrite(LIST, LOW);
  delay(1000);
  digitalWrite(LIST, HIGH); // play
  delay(200);
  digitalWrite(LIST, LOW);
}

void loop()
{
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // &len probably not needed
 
if (rxId == 543) {
    if(rxBuf[0] == 1) {         // 1 = 0x01
      previousMillis = millis();
//      Serial.println("list");   // debug only
      digitalWrite(LIST, HIGH);
    }
    if(rxBuf[0] == 64) {        // 64 = 0x40
      previousMillis = millis();
//      Serial.println("prev");
      digitalWrite(PREV, HIGH);
    }
    if(rxBuf[0] == 128) {       // 128 = 0x80
      previousMillis = millis();
//      Serial.println("next");
      digitalWrite(NEXT, HIGH);
    }
  }
 }
else {
currentMillis = millis();
if(currentMillis - previousMillis > interval) {       // about 80ms press required to be recorded
digitalWrite(NEXT, LOW);
digitalWrite(PREV, LOW);
digitalWrite(LIST, LOW);
}
}
}
