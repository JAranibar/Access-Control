/*
//------------------------------------------------------------------------------------------------------//
//-------------------------------------------- Conexiones ----------------------------------------------//
//------------------------------------------------------------------------------------------------------//

.TECLADO
- A0 = Cursor Derecha     
- A1 = Cursor Izquierda   
- A2 = Cursor Arriba
- A3 = Cursor Abajo
- A4 = OK

.Sensor Estado de Puerta
- A5 = Final de Carrera

.LCD
- D13 a D8 = Conexion Paralelo

.LED Indicador de Estado
- D7 = Rojo (cerrado) 
- D6 = Verde (abierto)

.TONO (interior error)
- D5 = Piezoelectrico

.ACTUADOR
- D4 = Seguro Electromagnetico con retorno muelle 

.ALARMA (exterior sirena intruso)
- D3 = Dispositivo Switch

.Deshabilitador de Alarma
- D2 = Entrada Switch

*/
//------------------------------------------------------------------------------------------------------//
//------------------------------------ Declaracion de Variables ----------------------------------------//
//------------------------------------------------------------------------------------------------------//

#include <EEPROM.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

int Intentos = 3;
int Chances;
int Flag = 0;
int Salto = 0;
int Error = 3000;
int Time_Error = 500;
int Delay = 500;
int Cronometro = 0;
int Puntero_Menu = 0;
int Puntero_Alfanumerico = 6;
int Puntero_Flecha = 10;
int Direccion = 0;
byte Valor;

char Clave[4]= {'_','_','_','_'};
char ClaveNueva[4]= {'_','_','_','_'};
char Alfanumerico[11] = {'8','3','2','1','0','_','A','C','J','R','S'};
char Ingreso[4] = {'_','_','_','_'};

byte Select[8] = {B00000, B00100, B00010, B11001, B00010, B00100, B00000, B00000};
byte Flecha[8] = {B00100, B00100, B00100, B00100, B10001, B01010, B00100, B00000};

byte Bloqueado1[8] = {B00000, B00011, B00100, B01000, B01000, B01000, B01000, B01000};
byte Bloqueado2[8] = {B00000, B11000, B00100, B00010, B00010, B00010, B00010, B00010};
byte Bloqueado3[8] = {B11110, B11110, B01110, B01110, B01110, B11110, B11110, B00000};
byte Bloqueado4[8] = {B01111, B01111, B01110, B01110, B01110, B01111, B01111, B00000};

byte Desbloqueado2[8] = {B00000, B11000, B00100, B00010, B00010, B00000, B00000, B00000};

byte Alarma1[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00001};
byte Alarma2[8] = {B00100, B01110, B01110, B11111, B11111, B11011, B11011, B11011};
byte Alarma3[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B10000};
byte Alarma4[8] = {B11000, B11000, B11100, B11100, B11110, B11110, B11111, B00000};
byte Alarma5[8] = {B11011, B11011, B11011, B11111, B11011, B11111, B11111, B00000};
byte Alarma6[8] = {B00011, B00011, B00111, B00111, B01111, B01111, B11111, B00000};

//------------------------------------------------------------------------------------------------------//
//------------------------------------------ Configuracion ---------------------------------------------//
//------------------------------------------------------------------------------------------------------//

void setup() {
  pinMode (2,INPUT);  // Sensor Alarma            
  pinMode (3,OUTPUT);  // Alarma
  pinMode (4,OUTPUT);  // Actuador
  pinMode (5,OUTPUT);  // Tone  
  pinMode (6,OUTPUT);  // Led RGB - Verde
  pinMode (7,OUTPUT);  // Led RGB - Rojo 
  lcd.begin(16, 2);
  if (digitalRead(2) == HIGH){
    Chances = 3;
    EEPROM.write(4, Chances);
  }
  if (EEPROM.read(4) == 0){
    Alarma();}
  Lectura_Sensor();
  if (Salto == 0){
    Presentacion();}   
  Lectura_EEPROM();}

//------------------------------------------------------------------------------------------------------//
//--------------------------------------- Cuerpo del Programa ------------------------------------------//
//------------------------------------------------------------------------------------------------------//

void loop() {
  Menu();
  if (Puntero_Menu == 0){
    Ingreso_Actual();    
    while (Flag == 0){
      while (analogRead(A4)<500){
        Teclado();}
      Analisis_Clave();}
    if (Chances != 3){
      Chances = 3;
      EEPROM.write(4, Chances);} 
    Flag = 0;}      
  else{
    Ingreso_Actual();    
    while (Flag == 0){
      while (analogRead(A4)<500){
        Teclado();}
      if (Clave[0]==Ingreso[0] && Clave[1]==Ingreso[1] && Clave[2]==Ingreso[2] && Clave[3]==Ingreso[3]){
        Flag = 1;
        if (Chances != 3){
          Chances = 3;
          EEPROM.write(4, Chances);}
        digitalWrite(7,LOW);
        digitalWrite(6,HIGH);      
        Ingreso_Nuevo();
        while (analogRead(A4)<500){
          Teclado();}
        ClaveNueva[0]=Ingreso[0];
        ClaveNueva[1]=Ingreso[1];
        ClaveNueva[2]=Ingreso[2];
        ClaveNueva[3]=Ingreso[3];
        Repita_Ingreso();
        while (Intentos > 0){
          while (analogRead(A4)<500){
          Teclado();}
          if (Ingreso[0]==ClaveNueva[0] && Ingreso[1]==ClaveNueva[1] && Ingreso[2]==ClaveNueva[2] && Ingreso[3]==ClaveNueva[3]){
            Escritura_EEPROM();
            Lectura_EEPROM();  
            Operacion_Exitosa();
            Intentos = 0;}
          else{
            tone(5,Error,Time_Error);
            delay(1000);
            Intentos--;
            if (Intentos == 0){
              lcd.noDisplay();
              delay(Delay);
              lcd.clear(); 
              lcd.display();}}}
        Intentos = 3;}
      else{
        Chances--;
        EEPROM.write(4, Chances);
        if (Chances == 0){
          digitalWrite(3,HIGH);
          Alarma();}
        tone(5,Error,Time_Error);
        delay(1000);}}
      Flag = 0;}}         

//------------------------------------------------------------------------------------------------------//
//----------------------------------------- Sub-Funciones ----------------------------------------------//
//------------------------------------------------------------------------------------------------------//

void Menu(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Sign in");
  lcd.setCursor(1,1);
  lcd.print("Change password");
  lcd.createChar(1, Select);
  lcd.setCursor(0,0); 
  lcd.write(1);
  Puntero_Menu = 0;
  while (analogRead(A4)<500){
    if (analogRead(A2)>500){
      lcd.setCursor(0,1); 
      lcd.print(" ");
      lcd.createChar(1, Select);
      lcd.setCursor(0,0); 
      lcd.write(1);
      Puntero_Menu = 0;}
    if (analogRead(A3)>500){
      lcd.setCursor(0,0); 
      lcd.print(" ");      
      lcd.createChar(1, Select);
      lcd.setCursor(0,1); 
      lcd.write(1);
      Puntero_Menu = 1;}}}

//------------------------------------------------------------------------------------------------------

void Operacion_Exitosa(){
  lcd.noDisplay();
  delay(Delay);
  lcd.clear(); 
  lcd.display();
  lcd.setCursor(3,0);  
  lcd.print("Successful");
  lcd.setCursor(4,1);
  lcd.print("Operation");
  delay(Delay);
  digitalWrite(7,LOW);
  digitalWrite(6,HIGH);
  tone(5,3000,100);
  delay(100);
  digitalWrite(6,LOW);    
  delay(50);
  digitalWrite(6,HIGH);  
  tone(5,3000,100);
  delay(100);
  digitalWrite(6,LOW);
  delay(50);
  digitalWrite(6,HIGH); 
  tone(5,3000,100);
  delay(100);
  digitalWrite(6,LOW);
  digitalWrite(7,HIGH);
  delay(3000);   
  lcd.noDisplay();
  delay(Delay);
  lcd.clear(); 
  lcd.display();}

//------------------------------------------------------------------------------------------------------

void Presentacion(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Access Control");
  lcd.setCursor(2,1);
  lcd.print("UNGS - 2015");
  delay(4000);
  lcd.noDisplay();
  delay(Delay);
  lcd.clear(); 
  lcd.display();}

//------------------------------------------------------------------------------------------------------

void Ingreso_Actual(){
  lcd.noDisplay();
  delay(Delay); 
  lcd.clear(); 
  lcd.display();
  Ingreso[0] = '_';
  Ingreso[1] = '_';
  Ingreso[2] = '_';
  Ingreso[3] = '_';
  Puntero_Flecha = 10;
  lcd.print("Enter");
  lcd.setCursor(0,1);
  lcd.print("Password");
  lcd.createChar(1, Flecha);
  lcd.setCursor(10,0); 
  lcd.write(1);
  lcd.setCursor(10,1);
  lcd.print("____");}  
 
//------------------------------------------------------------------------------------------------------

void Ingreso_Nuevo(){
  lcd.noDisplay();
  delay(Delay);
  digitalWrite(6,LOW);
  digitalWrite(7,HIGH);
  lcd.clear(); 
  lcd.display();
  Ingreso[0] = '_';
  Ingreso[1] = '_';
  Ingreso[2] = '_';
  Ingreso[3] = '_';
  Puntero_Flecha = 10;
  lcd.print("Ent. New");
  lcd.setCursor(0,1);
  lcd.print("Password");
  lcd.createChar(1, Flecha);
  lcd.setCursor(10,0); 
  lcd.write(1);
  lcd.setCursor(10,1);
  lcd.print("____");}  

//------------------------------------------------------------------------------------------------------

void Repita_Ingreso(){
  lcd.noDisplay();
  delay(Delay);
  lcd.clear(); 
  lcd.display();
  Ingreso[0] = '_';
  Ingreso[1] = '_';
  Ingreso[2] = '_';
  Ingreso[3] = '_';
  Puntero_Flecha = 10;
  lcd.print("Rep. New");
  lcd.setCursor(0,1);
  lcd.print("Password");
  lcd.createChar(1, Flecha);
  lcd.setCursor(10,0); 
  lcd.write(1);
  lcd.setCursor(10,1);
  lcd.print("____");}  

//------------------------------------------------------------------------------------------------------

void Alarma(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Segurity");
  lcd.setCursor(2,1);
  lcd.print("Alarm");
  lcd.createChar(1, Alarma1);
  lcd.setCursor(11,0); 
  lcd.write(1);
  lcd.createChar(2, Alarma2);
  lcd.setCursor(12,0); 
  lcd.write(2);
  lcd.createChar(3, Alarma3);
  lcd.setCursor(13,0); 
  lcd.write(3);
  lcd.createChar(4, Alarma4);
  lcd.setCursor(13,1); 
  lcd.write(4);
  lcd.createChar(5, Alarma5);
  lcd.setCursor(12,1); 
  lcd.write(5);
  lcd.createChar(6, Alarma6);
  lcd.setCursor(11,1); 
  lcd.write(6); 
  delay(250);
  digitalWrite(3,LOW);
  while (true){   
    lcd.display();
    digitalWrite (7,HIGH);
    tone(5,4000,500);
    delay(500);    
    lcd.noDisplay();
    digitalWrite (7,LOW);
    delay(500);}}

//------------------------------------------------------------------------------------------------------

void Lectura_EEPROM(){
  Direccion = 0;
  while(Direccion < 4){
    Valor=EEPROM.read(Direccion);
    Clave[Direccion]=Alfanumerico[Valor];
    Direccion++;}
  Chances=EEPROM.read(4);
  Serial.println(Chances);}

//------------------------------------------------------------------------------------------------------

void Escritura_EEPROM(){
  Direccion = 0;
  while(Direccion < 4){
    for (int i=0; i <= 10; i++){
      if (ClaveNueva[Direccion]==Alfanumerico[i]){
        Valor = i;}}
    EEPROM.write(Direccion, Valor);
    Direccion++;}}

//------------------------------------------------------------------------------------------------------

void Lectura_Sensor(){
  if (analogRead(A5)<500){
    digitalWrite(7,LOW);
    Desbloqueado();
    delay(200);
    digitalWrite (4,HIGH);    
    digitalWrite(6,HIGH);
    while(analogRead(A5)<500){
    }
    digitalWrite(6,LOW);    
    Bloqueado();
    delay(200);
    digitalWrite (4,LOW);
    digitalWrite(7,HIGH); 
    delay(3000);
    lcd.noDisplay();
    delay(Delay);
    lcd.clear(); 
    lcd.display();
    Salto = 1;}
  digitalWrite(7,HIGH);  
  digitalWrite(6,LOW);}

//------------------------------------------------------------------------------------------------------

void Desbloqueado(){
  lcd.noDisplay();
  delay(Delay);
  lcd.clear(); 
  lcd.createChar(1, Bloqueado1);
  lcd.setCursor(2,0); 
  lcd.write(1);
  lcd.createChar(2, Desbloqueado2);
  lcd.setCursor(3,0); 
  lcd.write(2);
  lcd.createChar(3, Bloqueado3);
  lcd.setCursor(3,1); 
  lcd.write(3);
  lcd.createChar(4, Bloqueado4);
  lcd.setCursor(2,1); 
  lcd.write(4);   
  lcd.setCursor(6,0);
  lcd.print("Unlocked");
  lcd.setCursor(7,1);
  lcd.print("Access");  
  lcd.display();}

//------------------------------------------------------------------------------------------------------

 void Bloqueado(){
  lcd.noDisplay();
  delay(Delay);
  lcd.clear(); 
  lcd.createChar(1, Bloqueado1);
  lcd.setCursor(2,0); 
  lcd.write(1);
  lcd.createChar(2, Bloqueado2);
  lcd.setCursor(3,0); 
  lcd.write(2);
  lcd.createChar(3, Bloqueado3);
  lcd.setCursor(3,1); 
  lcd.write(3);
  lcd.createChar(4, Bloqueado4);
  lcd.setCursor(2,1); 
  lcd.write(4);   
  lcd.setCursor(7,0);
  lcd.print("Locked");
  lcd.setCursor(7,1);
  lcd.print("Access");  
  lcd.display();}
  
//------------------------------------------------------------------------------------------------------  
  
void Teclado(){
  if (analogRead(A0)>500 && Puntero_Flecha != 13) {
    lcd.setCursor(Puntero_Flecha,0); 
    lcd.print(" ");
    Puntero_Flecha++;
    lcd.setCursor(Puntero_Flecha,0); 
    lcd.write(1);
    delay(200);}
        
  if (analogRead(A1)>500 && Puntero_Flecha != 10) {
    lcd.setCursor(Puntero_Flecha,0); 
    lcd.print(" ");
    Puntero_Flecha--;
    lcd.setCursor(Puntero_Flecha,0); 
    lcd.write(1);
    delay(200);}
   
  if (analogRead(A2)>500 && Puntero_Alfanumerico != 10) {
    if (Ingreso[Puntero_Flecha - 10] != Alfanumerico[Puntero_Alfanumerico]){
      Puntero_Alfanumerico=0;
      while(Ingreso[Puntero_Flecha - 10] != Alfanumerico[Puntero_Alfanumerico]){
        Puntero_Alfanumerico++;}}   
    Puntero_Alfanumerico++;
    Ingreso[Puntero_Flecha - 10] = Alfanumerico[Puntero_Alfanumerico];
    lcd.setCursor(Puntero_Flecha,1);
    lcd.print(Alfanumerico[Puntero_Alfanumerico]);
    delay(200);}
       
  if (analogRead(A3)>500 && Puntero_Alfanumerico != 0) {
    if (Ingreso[Puntero_Flecha - 10] != Alfanumerico[Puntero_Alfanumerico]){
      Puntero_Alfanumerico=0;
      while(Ingreso[Puntero_Flecha - 10] != Alfanumerico[Puntero_Alfanumerico]){
        Puntero_Alfanumerico++;}}
    Puntero_Alfanumerico--;
    Ingreso[Puntero_Flecha - 10] = Alfanumerico[Puntero_Alfanumerico];
    lcd.setCursor(Puntero_Flecha,1);
    lcd.print(Alfanumerico[Puntero_Alfanumerico]);
    delay(200);}}
  
//------------------------------------------------------------------------------------------------------  
    
void Analisis_Clave(){
  if (Clave[0]==Ingreso[0] && Clave[1]==Ingreso[1] && Clave[2]==Ingreso[2] && Clave[3]==Ingreso[3]){
    Flag = 1;
    digitalWrite(7,LOW);
    Desbloqueado();
    delay(200);
    digitalWrite (4,HIGH);
    digitalWrite(6,HIGH);
    tone(5,50,5000);      
    while(analogRead(A5)>500 && Cronometro<5000){
      delay(1);
      Cronometro++;}
    Cronometro = 0;
    noTone(5);
    while(analogRead(A5)<500){
    }
    digitalWrite(6,LOW);
    Bloqueado();
    delay(200);
    digitalWrite (4,LOW);
    digitalWrite(7,HIGH);  
    delay(3000);
    lcd.noDisplay();
    delay(Delay);
    lcd.clear(); 
    lcd.display();}
  else{
    Chances--;
    EEPROM.write(4, Chances);   
    if (Chances == 0){
      digitalWrite(3,HIGH);
      Alarma();}
    tone(5,Error,Time_Error);
    delay(1000);}}
