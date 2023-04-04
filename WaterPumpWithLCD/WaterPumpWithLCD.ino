/*pini LED:
 *  RS : 12
 *  E : 11
 *  D4 -> D1 : 5 -> 2
 *pin senzor umiditate in sol: A0
 *pin potentiometru pentru citire prag : A1 
 *pin activate pompa apa : 7
 */

#include <LiquidCrystal.h>
#define RS_LED 12
#define E_LED  11
#define D4_LED 5
#define D3_LED 4
#define D2_LED 3
#define D1_LED 2

#define DIRT_SENZ A0
#define POTENT_SENZ A1

#define WATER_PUMP 7

char* command = NULL;
char* timeDelay = NULL;
int timeDel = -1;

int waterThr = -1;
int waterThrMapped = -1;
int waterLvl = -1;
int waterLvlMapped = -1;

LiquidCrystal lcd(RS_LED, E_LED, D4_LED, D3_LED, D2_LED, D1_LED);


void setup() {
  //initializez comunicarea cu calculatorul
  Serial.begin(9600);
  //initializez lcd-ul
  lcd.begin(16, 2);
  //initializezi pinii
  pinMode(DIRT_SENZ, INPUT);
  pinMode(POTENT_SENZ, INPUT);
  pinMode(WATER_PUMP, OUTPUT);
  digitalWrite(WATER_PUMP, LOW);
}

void loop() {
  while(Serial.available() == 0){
    //citesc valoarea potentiometrului si o mapez
    waterThr = analogRead(POTENT_SENZ);
    waterThrMapped = map(waterThr, 0, 1023, 0, 100);
    //afisez pe lcd valoarea citita de la potentiometru
    lcd.setCursor(0,0);
    lcd.print("Water thr:"); lcd.print(waterThrMapped);lcd.print("%");
    checkIfWater();
    lcd.clear();
  }
  ManualStart();
}

void checkIfWater(){
  //citesc valoarea de la senzorul de umiditate si o mapez
  waterLvl = analogRead(DIRT_SENZ);
  waterLvlMapped = map(waterLvl, 0, 1023, 0, 100);
  lcd.setCursor(0,1);
  lcd.print("Dry level:"); lcd.print(waterLvlMapped);lcd.print("%");
  //daca pamantul este prea uscat activam pompa
  if( waterLvlMapped >= waterThrMapped){
    digitalWrite(WATER_PUMP, HIGH);
  } else {
    digitalWrite(WATER_PUMP, LOW);
  }
  delay(50);
}

//functie care prelucreaza comanda data de la calculator
void ManualStart(){
  char aux[100];
  String a = Serial.readString();
  a.toCharArray(aux, 100);
  command = strtok(aux, " \n");
  timeDelay = strtok(NULL, " \n");
  timeDel = atoi(timeDelay);
  if(!strcmp(command, "status")){
    Serial.print("Dry lvl: "); Serial.print(waterLvlMapped);Serial.println("%");
    Serial.print("Water thr: "); Serial.print(waterThrMapped);Serial.println("%");
    return;
  } else if (!strcmp(command, "activate")){
    if(timeDelay == NULL){
      Serial.println("Nu s-a introdus un timp de activate");
      return;
    }
    digitalWrite(WATER_PUMP, HIGH);
    Serial.println("A fost activata pompa de apa");
    delay(timeDel);
    digitalWrite(WATER_PUMP, LOW);
    Serial.println("A fost dezactivata pompa de apa");
    return;
  }
  Serial.println("Comanda necunoscuta");
}
