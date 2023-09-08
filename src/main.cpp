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

//piny od mega 2,3,18,19,20,21 są od przerwań
//20 SDA, 21 SCL

#define SDA (20) //SDA master default
#define SCL (21) //SCL master default

#define I2C_ADDR1 (1) //pion 1 - wartości zawsze 7bitowe od 0 do 127
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

//komunikacja od mastera do rpi - USB (UART 0)

void MasterToRpi()
{
    Serial.begin(115200); //baudrate między master a rpi
    Serial.println("Otwarto komunikację Master - RaspberryPi");
}

//obsługa komunikacji i2c jako master

void start_I2C()
{
    Wire.begin(); //tutaj adres mastera w razie czego
    Wire.write("Tutaj master Arduino!");
}

//reset magistrali w razie problemów

void ResetI2C()
{
    TWCR = 0;
    Wire.begin();
}

//wykrycie zdarzenia od slave

void eventOccured(int pion)
{
    switch(pion)
    {
        case 1:
        event1_ctr++;
        while(Wire.available()!=0) //wykonuje JEŚLI są bajty do odczytu
        {
            char mssg = Wire.read(); //odczytuje i przechowuje wiadomość
            Serial.println(mssg); //przekazuje wiadomość do rpi
        }
        flag1 = false; //opuszczenie flagi po odczycie wszystkiego
        break;
        case 2:
        event2_ctr++;
        while(Wire.available()!=0) //wykonuje JEŚLI są bajty do odczytu
        {
            char mssg = Wire.read(); //odczytuje i przechowuje wiadomość
            Serial.println(mssg); //przekazuje wiadomość do rpi
        }
        flag2 = false; //opuszczenie flagi po odczycie wszystkiego
        break;
        case 3:
        event3_ctr++;
        while(Wire.available()!=0) //wykonuje JEŚLI są bajty do odczytu
        {
            char mssg = Wire.read(); //odczytuje i przechowuje wiadomość
            Serial.println(mssg); //przekazuje wiadomość do rpi
        }
        flag3 = false; //opuszczenie flagi po odczycie wszystkiego
        break;
        case 4:
        event4_ctr++;
        while(Wire.available()!=0) //wykonuje JEŚLI są bajty do odczytu
        { 
            char mssg = Wire.read(); //odczytuje i przechowuje wiadomość
            Serial.println(mssg); //przekazuje wiadomość do rpi
        }
        flag4 = false; //opuszczenie flagi po odczycie wszystkiego
        break;
        default:
        Serial.println("Wykryto inne zdarzenie na magistrali I2C");
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
    else //jeśli oba na raz lub żaden ustawiam oba na stan niski
    {
        digitalWrite(STEP_M1, LOW);
        digitalWrite(STEP_M2, LOW);
    }
}

void setup() {

//informacja startowa do podglądu
Serial.begin(115200);
Serial.println("Uruchomiono master Arduino!");

//konfiguracja pinów przerwań
pinMode(I2C_INTERRUPT_1, INPUT_PULLUP);
pinMode(I2C_INTERRUPT_2, INPUT_PULLUP);
pinMode(I2C_INTERRUPT_3, INPUT_PULLUP);
pinMode(I2C_INTERRUPT_4, INPUT_PULLUP);

//konfiguracja pinów step motoru
pinMode(STEP_M1, OUTPUT);
pinMode(STEP_M2, OUTPUT);

//wartość początkowa dla pinów step motoru
digitalWrite(STEP_M1, LOW);
digitalWrite(STEP_M2, LOW);

//ustanowienie przerwań - w razie potrzeby zmienić logike
attachInterrupt(2,Isr1,FALLING);
attachInterrupt(3,Isr2,FALLING);
attachInterrupt(18,Isr3,FALLING);
attachInterrupt(19,Isr4,FALLING);

//start komunikacji
MasterToRpi(); //do rpi
start_I2C(); //do peryferiów
}

void loop() {

//oczekiwanie na polecenie dotyczące step motoru
if(req_up == true)
{
    UpMove();
    req_up = false;
    Serial.println("Wykonano polecenie w górę");
}

if(req_down == true)
{
    DownMove();
    req_down = false;
    Serial.println("Wykonano polecenie w dół");
}

//oczekiwanie na sygnały od slave
if(flag1 == true)
{
    Serial.println("Wykryto żądanie od pionu 1"); //komunikat dla rpi
    eventOccured(1); //obsługa żądania od slave
}
else if(flag2 == true)
{
    Serial.println("Wykryto żądanie od pionu 2"); //komunikat dla rpi
    eventOccured(2); //obsługa żądania od slave
}
else if(flag3 == true)
{
    Serial.println("Wykryto żądanie od pionu 3"); //komunikat dla rpi
    eventOccured(3); //obsługa żądania od slave
}
else if(flag4 == true)
{
    Serial.println("Wykryto żądanie od pionu 4"); //komunikat dla rpi
    eventOccured(4); //obsługa żądania od slave
}
else
{
    Serial.println("Brak wiadomości od peryferiów"); //tymczasowo w celach debug
}

}
