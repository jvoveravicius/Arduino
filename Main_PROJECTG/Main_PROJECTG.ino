#define WATERMOTOR 11
#define SUN 8
#define ALERT0 2
#define AlERT1 3
#define AlERT2 4
#define TEMPERATURE 5
#define HYGROMETER A0


boolean SystemCheck = false;
boolean TetMode = false;
int MaxWater = 90;
int MinWater = 60;
int CheckTime = 1000;


void setup() {
    
  pinMode(HYGROMETER, INPUT);
  pinMode(WATERMOTOR, OUTPUT);
  pinMode(SUN, OUTPUT);
  pinMode(ALERT0, OUTPUT);
  pinMode(AlERT1, OUTPUT);
  pinMode(AlERT2, OUTPUT);
  
  Serial.begin(9600);  
  
}


class Info {


  private:
    int NotDelay = 100;
    int HardTimes = 10;
    int HardDelay = 50;

  public:
   void Notification(int, int);
   void HardwareWarning();
   
};



void Info::Notification (int Led, int Times) {

  for (int i=0;i<Times;i++){

    for (int k=1; k>-1; k--){
      
      digitalWrite(Led, k);
      delay(NotDelay);
      
    }
      delay(NotDelay);
    
   } 
}

void Info::HardwareWarning () {

  int i = 0;


    while(HardTimes>i){

      int h = 1;

      do{
        
        digitalWrite(ALERT0, h);
        digitalWrite(AlERT1, h);
        digitalWrite(AlERT2, h);
        delay(HardDelay);
        h--;
        
      }while(h>-1);

      i++;
    
    };

   
}


class Hygro {

  private:
    Info inf;
    short MaxVal = 100;
    short MinValue = 1023;
    short MaxPerc = 100;
    short output_value ;
    
  public:
    int ret_values (int);
    
};



int Hygro::ret_values (int DelayTime) {
  
  output_value= analogRead(HYGROMETER);
  output_value = map(output_value,MinValue,MaxVal,0,MaxPerc);
  delay(DelayTime);
  
  return output_value;
}


class WaterPomp {

  private:
    Hygro dry;
    Info info;
    int CheckTimes = 10;
    int DelayCheckTimes = 9000;
    int RunningMotorTime = 5000;
    
  public:
    void DuneMode();

    
};


void WaterPomp::DuneMode () {
  
  int HygroVal = 0;
  int i = 0;
  
  repeat:
  boolean ActivateMotor = true;

  do{
    delay(DelayCheckTimes);
    int HygroVal = dry.ret_values(1);
    Serial.println(HygroVal);

     if (HygroVal<=0){
          info.HardwareWarning();
          ActivateMotor = false;
          break;
     }
     else{

          
        if (HygroVal<=MinWater){
          info.Notification(ALERT0, 4);
          i++; 
        }
        else{
          ActivateMotor = false;
          info.Notification(AlERT1, 4);
          break;
        }
      
     }
  
    
  }while(i<CheckTimes);


  if (ActivateMotor){
    digitalWrite(WATERMOTOR, HIGH);
    delay(RunningMotorTime);
    digitalWrite(WATERMOTOR, LOW);
    i = i-2;
    goto repeat;
  }


  
}




/*
void temperature(){

  float temp;
  
  for(int i=0;i<2;i++){
     
     temp = analogRead(TEMPERATURE); 
   }
 
  temp = temp * 0.48828125;
  Serial.print("TEMPRATURE = ");
  Serial.print(temp);
  Serial.print("*C");
  Serial.println();
  delay(1000);

}
*/

Info info;
Hygro hygro;
WaterPomp pump;

void SelfCheckDigit(){

  
  int Modules [] = {WATERMOTOR, ALERT0, AlERT1, AlERT2};
  int l = (sizeof(Modules)/sizeof(*Modules));

  for (int i=0;i<l;i++){
    
    int z = 1;

    do{
      
      digitalWrite(Modules[i], z);
      delay(1000);      
      z--; 
      
     }while(z>-1);
     
    
  }


  info.HardwareWarning();

}

void SelfCheckAnalog(){

  int AnalogCheckDelay = 1000;
  int TimeToCheck = 10;

  delay(AnalogCheckDelay);


  for (int i=0; i<TimeToCheck; i++){

    delay(AnalogCheckDelay);

    if (hygro.ret_values(0)>0){
      info.Notification(AlERT1, 2);
      break;
    }
    else if (i==TimeToCheck-1){
      
      info.Notification(ALERT0, 100);
    }
    
  }

  info.HardwareWarning();

}

int SunTime = 0;
int DelayTime = 0;
boolean EnableSun = false;

void loop() {

  
    if (SystemCheck){
         SelfCheckDigit();
         SelfCheckAnalog();
         SystemCheck = false;
    }

    Stop:


    int HygroVal = hygro.ret_values(CheckTime);
    Serial.println(HygroVal);



    if (!TetMode){

      
      if (HygroVal<=0){
        info.HardwareWarning();
        goto Stop;
       }
       
  
      if (HygroVal>=MaxWater){ //nuo 90
  
          info.Notification(AlERT2, 2);
       }
       else if (HygroVal>MinWater){ //nuo 89 iki 31
        
          info.Notification(AlERT1, 2);
       }
       else if (HygroVal<=MinWater){ //maziau arba lygu 30
         pump.DuneMode();
         info.Notification(ALERT0, 2);
         
       }
  
      
      Serial.println(HygroVal);

    }
    
  
    
    if (SunTime>=1800)
    {

      Serial.println("OFFTime: ");
      Serial.println(DelayTime);
      DelayTime++;


      if (!EnableSun)
      {
        digitalWrite(SUN, HIGH);
        EnableSun = true;
      }
      else if (DelayTime>=1800)
      {
        digitalWrite(SUN, LOW);
        SunTime = 0;
        DelayTime = 0;
        EnableSun = false;
      }

      
    }
    else
    {
      
        SunTime++;
        Serial.println("ONTime: ");
        Serial.println(SunTime);
    }


  
 }
