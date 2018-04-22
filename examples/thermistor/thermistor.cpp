/*

ZeroMQ Arduino Ethernet Shield example

created 09 Feb 2015 
by Matt Ebert

You need an Ethernet Shield and (optionally) some sensors to be read on analog pins 0 and 1

This example performs the zmtp greeting and handshake for a REQ socket as documented at:

http://rfc.zeromq.org/spec:37

Ethernet is handled by the uIPEthernet library by (Norbert Truchsess) which uses 
the uIP tcp stack by Adam Dunkels.

 */

#include <Arduino.h>
#include <UIPEthernet.h>
#include <UIPClient.h>
#include <TimeLib.h>

#include "origin.h" // monitoringServer interface
#include "datapacket.h"

#define DHCP 1

char buffer[256]={0}; // communication buffer

// initialize the library instance:
EthernetClient client;

// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip    (192,168,1,200);
IPAddress data_server(128,104,160,152);
int reg_port = 5558;
int mes_port = 5559;

// origin data server interface
Origin origin( client
    , data_server
    , reg_port
    , mes_port
    , (char *)"HYBRID_coiltemp"   // stream name
    , 15        // stream name length (max 10)
    , 1         // use fractional seconds
    , 2         // maximum channels
    , 2         // channels used
    , (char *)DTYPE_INT16
    , INT16_TYPE_SIZE
    , buffer
);

// mega pin 13
uint8_t lastTrig = 1;

signed long next;// = 0;          // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 60000;  //delay between updates (in milliseconds)

//This can change
int alarmPin = 10;
int killPin = 13;
bool kill = false;
bool killOld;

//Temperature kill condition setup.
int Thermistor = A5;
float T;
int Twarn = 65;
int Tkill = 70;
int Rp = 7980;
int Vin = -15;
int m = -2.0172;
int b = 0.0488;

//Thermistor Temp Calculation Parameters.
int Rref = 10000;
float A = 0.003354016;
float B = 0.000256985;
float C = 0.00000262013;
float D = 0.0000000638309;

//This returns the current temperature across the thermistor in
//kelvin.
float ReadT (){

  //the thermistor is connected in a voltage divider with
  //resistance R1 and input voltage Vin.
  float Vread = 5*(float)analogRead(Thermistor)/1023;
  float Vact = m*Vread - b;
  float R = ((float) Rp*Vact ) /((float)(Vin-Vact));
  float LRoR = log(R/Rref);
  float invT = A + B*LRoR +C*pow(LRoR,2)+D*pow(LRoR,3);

  Serial.print("Voltage in:");
  Serial.println(Vread);
  Serial.print("Voltage across thermistor :");
  Serial.print(Vact);
  Serial.print("Inverse Temperature:");
  Serial.println(invT);
  
  return (1/invT);
}

//Kills the experiment and sends email alerts to those on the
//email list.
void KillExperiment(int condition){
  //This kills the experiment
  digitalWrite(killPin,HIGH);
  digitalWrite(alarmPin,HIGH);

  //This alerts anyone that should know about the condition.
//  for(int i = 0; i < LISTSIZE; i++){
//    Serial.print((SendEmail(mailingList[i],condition)) ? "Email sent to " : "Email failed to send to ");
//    Serial.println(sendTo[i]);
//  }

}

//Empty for now, if possible way to unlock arduino and
//allow experiment to run.
//For now just restart arduino when conditions are amenable to
//runing an experiment.
void RestartExperiment(){
  digitalWrite(killPin,LOW);
}

void setup_ethernet(){
  // set up ethernet chip
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x00};
#if DHCP && UIP_UDP // cant use DHCP without using UDP
  Serial.println(F("DHCP..."));
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed DHCP"));
    for(;;)
      ;
  }
#else
  Serial.println(F("STATIC..."));
  Ethernet.begin(mac,ip);
#endif

  Serial.println(Ethernet.localIP());
  Serial.println(Ethernet.subnetMask());
  Serial.println(Ethernet.gatewayIP());
  Serial.println(Ethernet.dnsServerIP());
}

void register_stream(){
  // setup request socket
  Serial.println(F("Registering stream with server."));
  uint8_t err;
  do{
    err = origin.registerStream();
    if(err==ERR_SERVER_RESP){
      Serial.println(F("Server error."));
    }
    if(err==ERR_SERVER_LEN){
      Serial.println(F("Invalid Response length from server."));
    }
  } while( err );
  Serial.println(F("Disconnecting REQ socket..."));
}

void setup_data_stream(){
  Serial.println(F("Setting up data stream..."));
  origin.setupDataStream();
  Serial.println(F("Starting"));
  Serial.println(F("Data"));
  Serial.println(F("Stream"));
}


void setup() {
  // minimal SPI bus config (cant have two devices being addressed at once)
  digitalWrite(SS, HIGH); // set ENCJ CS pin high 

  killOld = kill;
  pinMode(killPin, OUTPUT);
  Serial.begin(115200); //Turn on Serial Port for debugging

  setup_ethernet();
  register_stream();

  delay(1000); // increase stability
  setup_data_stream();
  delay(1000);
}

uint32_t nextTrig = 0;
int16_t data[2] = {0};

void loop() {
  Ethernet.maintain();
  now();  // see if its time to sync

  uint8_t newTrig = 1;
  if( millis() > nextTrig ){
    newTrig = 0;
    nextTrig = millis() + postingInterval;
  }
  if( !lastTrig && newTrig ){  // trig on high to low trigger
    uint8_t condition = 0;
    //check that the temperature is below the setpoint.
    float T = ReadT() - 273.15;
    Serial.print("Temperature: ");
    Serial.println(T);
    if(T>=Twarn){
      condition = 1;
    }
    if(T>=Tkill){
      condition = 2;
    }

    data[0] = (int16_t)(T*1000.0);
    data[1] = (int16_t)condition;
    //Send string back to client 
    origin.sendPacket( 0, 0, data );
  }
  lastTrig = newTrig;

  // check for incoming packet, do stuff if we need to
  uint8_t len = client.available();
  if( len ){
    Serial.println("incoming packet");
    client.read((uint8_t*)buffer, len);
    Serial.write(buffer, len);
  }
}

// normal arduino main function
int main(void){
  init();

  setup();

  for(;;){
    loop();
    if (serialEventRun) serialEventRun();
  }
  return 0;
}
