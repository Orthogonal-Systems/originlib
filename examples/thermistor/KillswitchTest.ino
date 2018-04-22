int LISTSIZE = 3;
int MAXSIZE = 30;
char* mailingList[MAXSIZE] = {
  "jbohorquez@wisc.edu",
  "j.bohorquez@umiami.edu",
  "juandaman95@gmail.com"
};

//This can change
int killPin = 13;
bool kill = false;
bool killOld;

//Temperature kill condition setup.
int Thermistor = A0;
double T;
int Tkill = 27;
int R1 = 10000;
int Vin = 5;

//Thermistor Temp Calculation Parameters.
int Rref = 10000;
double A = 0.003354016;
double B = 0.000256985;
double C = 0.00000262013;
double D = 0.0000000638309;

void setup() {
  killOld = kill;

  Serial.begin(9600);//for debuging, remove in final version
  pinMode(killPin, OUTPUT);
}

void loop() {
  killOld = kill;
  int condition = -1;
  //check that the temperature is below the setpoint.
  T = ReadT() - 273.15;
  Serial.print("Temperature: ");
  Serial.println(T);
  delay(2000);
  if(T>=Tkill){
    condition = 1;
    kill = true;
  }

//  Serial.print("Temperature: ");
//  Serial.print(T);
//  Serial.print(" C \n");

  //kill or restart the process if the kill status has changed.
  if(kill != killOld){
    if(kill) KillExperiment(condition);
    else RestartExperiment();
  }
}

//This returns the current temperature across the thermistor in
//kelvin.
double ReadT (){

  //the thermistor is connected in a voltage divider with
  //resistance R1 and input voltage Vin.
  float V = 5*(float)analogRead(Thermistor)/1023;
  double R = ((double) R1*V ) /((double)(Vin-V));
  double LRoR = log(R/Rref);
  double invT = A + B*LRoR +C*pow(LRoR,2)+D*pow(LRoR,3);

  Serial.print("Voltage in:");
  Serial.println(V);
  Serial.print("Inverse Temperature:");
  Serial.println(invT);
  
  return (1/invT);
}

//Kills the experiment and sends email alerts to those on the
//email list.
void KillExperiment(int condition){
  //This kills the experiment
  digitalWrite(killPin,HIGH);

  //This alerts anyone that should know about the condition.
  for(int i = 0; i < LISTSIZE; i++){
    Serial.print((SendEmail(mailingList[i],condition)) ? "Email sent to " : "Email failed to send to ");
    Serial.println(sendTo[i]);
  }

}
//Empty for now, if possible way to unlock arduino and
//allow experiment to run.
//For now just restart arduino when conditions are amenable to
//runing an experiment.
void RestartExperiment(){
  digitalWrite(killPin,LOW);
}

//sends an email to the specified address detailing the kill condition.
//This function returns 1 if successful and 0 if unsuccessful. 
byte SendEmail(char address[], int condition){

  switch (condition){
    case 1:
      String why = "Temperature over Threshold";
      break;
    default:
      String why = "lol idk";
      break;
  }

  String Message = "Experiment has been shut down successfully\n" +
                   "Shut down occured because:\n"  + why;

  return 0;
}
