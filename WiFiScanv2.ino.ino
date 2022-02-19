#include <Adafruit_NeoPixel.h>
#include <FirebaseESP32.h>
#include <WiFi.h>

#define WIFI_SSID "PATELS" // your wifi SSID
#define WIFI_PASSWORD "aashay98" //your wifi PASSWORD

const int relay = 26;          // pin d0 as toggle pin
#define NUM_PIXELS 8      //number of rgb led
#define PIN_PIXELS 14     //pin d5 as neopixel input
#define FIREBASE_HOST "toggle-button-8891e-default-rtdb.firebaseio.com" // change here
#define FIREBASE_AUTH "9U20KIkRmlP2uuYzvPLgFFV3ay2bfMq06bZ6FMuf"  // your private key


#define LED_BUILTIN 2
#define SENSOR  27

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
//unsigned int flowMilliLitres;
float flowMilliLitres;
//unsigned long totalMilliLitres;
float totalMilliLitres;
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}


FirebaseData firebaseData;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800);
void setup ()
{
  pinMode(relay, OUTPUT);
  pixels.begin();
  Serial.begin(9600);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ") ;
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);  


  pinMode(SENSOR, INPUT_PULLUP);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);   
}
void loop ()
{
  if(Firebase.getString(firebaseData, "/Led1Status"))
  {
    String ledstatus = firebaseData.stringData();
    if(ledstatus.toInt() == 1){
      digitalWrite(relay, LOW);
      Serial.println("on");

    }
    else {
      digitalWrite(relay, HIGH);
      Serial.println("off");

    }
  }else{
    Serial.print("Error in getInt, ");
    Serial.println(firebaseData.errorReason());
    digitalWrite(relay, HIGH);
  } 

  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) ;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;

    //Send values totalMilliLitres and flowRate
    Firebase.setIntAsync(firebaseData, "/Usage", totalMilliLitres);
    Firebase.setIntAsync(firebaseData, "/Flow", flowRate);
    Serial.println(flowRate);
    Serial.println(totalMilliLitres);
  }
}
