#include <Arduino.h>
#include <Wire.h>

// ADRESY SLAVE
/*
1. Pion 1 
adres 1
2. Pion 2
adres 2
3. Pion 3
adres 3
4. Pion 4 
adres 4

opcjonalny adres mastera : 0
*/

#define I2C_ADDR1 (1) //pion 1
#define I2C_ADDR2 (2) //pion 2
#define I2C_ADDR3 (3) //pion 3
#define I2C_ADDR4 (4) //pion 4
#define I2C_ADDR_M (0) //master - opcjonalne

#define I2C_INTERRUPT_1 (2) //pin odczytujący przerwanie od pionu 1
#define I2C_INTERRUPT_2 (3) //pin odczytujący przerwanie od pionu 2
#define I2C_INTERRUPT_3 (18) //pin odczytujący przerwanie od pionu 3
#define I2C_INTERRUPT_4 (19) //pin odczytujący przerwanie od pionu 4

//piny 2-13 mega są PWM
#define STEP_M1 (9) //pin step motoru 1 - KIERUNKOWY
#define STEP_M2 (10) //pin step motoru 2 - PORUSZA SIĘ
// etc dopisać w razie potrzeby

#define MAXLVL 100 //ustawić maksymalną wartość o jaką się porusza

//zmienne
bool flag1 = false; //flaga pionu 1
bool flag2 = false; //flaga pionu 2
bool flag3 = false; //flaga pionu 3
bool flag4 = false; //flaga pionu 4

bool req_down = false; //flaga informująca o poleceniu w dół
bool req_up = false; //flaga informująca o poleceniu w górę

//licznik zdarzeń
int event1_ctr = 0;
int event2_ctr = 0;
int event3_ctr = 0;
int event4_ctr = 0;

//obsługa przerwań od pionów - załączenie flagi
void Isr1()
{
    flag1 = true;
}

void Isr2()
{
    flag2 = true;
}

void Isr3()
{
    flag3 = true;
}

void Isr4()
{
    flag4 = true;
}

//obsługa komunikacji i2c jako master

void start_I2C()
{
    Wire.begin();
    Wire.write("Tutaj master Arduino!");
}

void eventOccured(int pion)
{
    switch(pion)
    {
        case 1:
        while(Wire.available())
        {
            event1_ctr++;
            Wire.requestFrom(1,1); //oczekuje od pionu 1 1 byte
            //?
        }
        break;
        case 2:
        while(Wire.available())
        {
            event2_ctr++;
            Wire.requestFrom(2,1); //oczekuje od pionu 2 1 byte
            //?
        }
        break;
        case 3:
        while(Wire.available())
        {
            event3_ctr++;
            Wire.requestFrom(3,1); //oczekuje od pionu 3 1 byte
            //?
        }
        break;
        case 4:
        while(Wire.available())
        {
            event4_ctr++;
            Wire.requestFrom(4,1); //oczekuje od pionu 4 1 byte
            //?
        }
        break;
        default:
        //?
        break;
    }
    
}

//obsługa silnika krokowego

bool DirState()
{
    return digitalRead(STEP_M1); //zwraca aktualny kierunek
}

bool PulState()
{
    return digitalRead(STEP_M2); //zwraca stan pinu PUL
}

void UpMove()
{
    digitalWrite(STEP_M1, HIGH); //ustalenie kierunku do góry
    for(int i=0; i<MAXLVL; i++)
    {
        digitalWrite(STEP_M2, HIGH);
        delay(10); //lub inna wartość
    }
}

void DownMove()
{
    digitalWrite(STEP_M1, LOW); //ustalenie kierunku na dół
    for(int i=0; i<MAXLVL; i++)
    {
        digitalWrite(STEP_M2, LOW);
        delay(10); //lub inna wartość
    }
}

void MotorRequest()
{
    if(req_up == true)
    {
        UpMove();
    }
    else if(req_down == true)
    {
        DownMove();
    }
    else //jeśli oba na raz lub żaden
    {
        //?
    }
}

void setup() {

//informacja startowa
Serial.begin(115200);
Serial.println("Uruchomiono master Arduino!");

//konfiguracja pinów przerwań
pinMode(I2C_INTERRUPT_1, INPUT_PULLUP);
pinMode(I2C_INTERRUPT_2, INPUT_PULLUP);
pinMode(I2C_INTERRUPT_3, INPUT_PULLUP);
pinMode(I2C_INTERRUPT_4, INPUT_PULLUP);

//konfiguracja pinów ste pmotoru
pinMode(STEP_M1, OUTPUT);
pinMode(STEP_M2, OUTPUT);

//ustanowienie przerwań
attachInterrupt(2,Isr1,FALLING);
attachInterrupt(3,Isr2,FALLING);
attachInterrupt(18,Isr3,FALLING);
attachInterrupt(19,Isr4,FALLING);



}

void loop() {

  
}
