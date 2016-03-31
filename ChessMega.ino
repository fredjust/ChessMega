

//-----------------------------------------------------------
// GESTION DE L ECRAN 2x16
//-----------------------------------------------------------
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int keyPress;

byte touche = 0;

const byte droite=3;
const byte gauche=1;
const byte haut=5;
const byte bas=2;
const byte entree=4;
const byte aucune=0;

//-----------------------------------------------------------
// GESTION DE LA CARTE SD
//-----------------------------------------------------------
#include <SPI.h>
#include <SD.h>

File myFile;
//-----------------------------------------------------------
// GESTION DU Pseudo CLAVIER 8x8
//-----------------------------------------------------------

#include "Key.h"
#include "Keypad.h"

const byte ROWS = 8; 
const byte COLS = 8;
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','4','5','6','7','8'},
  {'a','b','c','d','e','f','g','h'},
  {'A','B','C','D','E','F','G','H'},
  {'&','é','#','{','(','-','è','_'},
  {'i','j','k','l','m','n','o','p'},
  {'I','J','K','L','M','N','O','P'},
  {'q','r','s','t','u','v','w','x'},
  {'Q','R','S','T','U','V','W','X'}
};


byte rowPins[ROWS] ={36,37,34,35,32,33,30,31} ; //connect to the row pinouts of the keypad
byte colPins[COLS] ={39,38,41,40,43,42,45,44}; //connect to the column pinouts of the keypad


byte LastbitMap[8]={195,195,195,195,195,195,195,195};
byte NewbitMap[8]={195,195,195,195,195,195,195,195};

//initialize an instance of class NewKeypad
Keypad ChessBoard = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 



unsigned long startTime;

// la position 
String strPos ="195.195.195.195.195.195.195.195.time";
// Nombre de positon dans l'enregistrement
int nbPos=0;
// on enregistre sur la carte si elle est détecté
bool SaveOnSD=false;

//-----------------------------------------------------------
// renvoi le prochain fichier gameXXXX.ard
// la carte ne doit contenir que des fichier gameXXXX.ard
// autrement il ecrira à la suite d'un autre
String nextnumfile()
{  
  String strtemp="";
  bool FileExist=false;
  int nbFile =1;  

  strtemp="GAME" + String(nbFile) + ".ARD";
  char filename1[strtemp.length()+1];
  strtemp.toCharArray(filename1, sizeof(filename1));
  FileExist=SD.exists(filename1);

  while(FileExist) 
  {     
    nbFile ++; 
    strtemp="GAME" + String(nbFile) + ".ARD";
    char filename2[strtemp.length()+1];
    strtemp.toCharArray(filename2, sizeof(filename2)); 
    FileExist=SD.exists(filename2);
  }

 return String(nbFile);
}

//-----------------------------------------------------------
// ferme le fichier precedent
// et ouvre un nouveau fichier pour ecrire une partie
void OpenNewFile()
{
  String strtemp ="";

  //remet le nombre de coup joué à 0
  nbPos=0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NEW GAME ");
  delay(500);
  lcd.print(". ");
  //ferme le dernier fichier
  myFile.close();
  delay(500);
  lcd.print(". ");
  //cherche le numero du fichier suivant
  strtemp=nextnumfile();
  delay(500);
  lcd.print(". ");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME : ");
  lcd.print(strtemp);

  strtemp ="GAME" + strtemp + ".ARD";
  delay(500);

  char filename[strtemp.length()+1];
  strtemp.toCharArray(filename, sizeof(filename));

  // ouvre le fichier
  myFile = SD.open(filename, FILE_WRITE);

  //test l'ouverture
  delay(500);
  if (myFile) {
    
    myFile.println("195.195.195.195.195.195.195.195.start");
    lcd.print(" YES ");
    SaveOnSD=true;
    
  } 
  else {
    lcd.print(" NO ");
    SaveOnSD=false;
  }

  Serial.println("195.195.195.195.195.195.195.195.start");
}


//-----------------------------------------------------------
// facultatif ecrit la derniere action sur l'écran :
// ON e2
void lcdprint() {  
  for(int r=0;r<8;r++) 
  {   
    if (LastbitMap[r]!=NewbitMap[r]) {         
      for(int c=0;c<8;c++) {
        if ( bitRead(LastbitMap[r],c)!=bitRead(NewbitMap[r],c) ) {  
          if (bitRead(NewbitMap[r],c)==0) {             
            
            lcd.print(" OFF ");                              
          }
          else {
            
            lcd.print(" ON  ");                    
          }
          lcd.print(hexaKeys[1][r]);
          lcd.print(c+1);  
          lcd.print(" ");            
        }
      }
    }
    LastbitMap[r]=NewbitMap[r];
  }
}
//-----------------------------------------------------------
// effectue les permutations de bits pour renvoyer toujours 195. ...
// facultatif si on place cela directement sur le prog PC
String permut() {

  byte TempoBitMap=0;
  strPos="";

  if (touche==haut) { 
      for(int i=0;i<8;i++) {
        strPos = strPos+ ChessBoard.bitMap[i];
        strPos = strPos+ ".";
        NewbitMap[i]=ChessBoard.bitMap[i];
      }       
    } else if (touche==droite) {
      
      for(int i=7;i>=0;i--)
      {
        for (int b=7;b>=0;b--)
        {
          bitWrite( TempoBitMap, b, bitRead(ChessBoard.bitMap[b],i) ); 
        } 
        strPos = strPos+ TempoBitMap;
        strPos = strPos + ".";
        NewbitMap[7-i]=TempoBitMap; 
      }

    } else if (touche==gauche) {

      for(int i=0;i<8;i++)
      {
        for (int b=0;b<8;b++)
        {
          bitWrite( TempoBitMap, 7-b, bitRead(ChessBoard.bitMap[b],i) );   
        }
        strPos = strPos+ TempoBitMap;
        strPos = strPos + ".";
        NewbitMap[i]=TempoBitMap;
      }

    } else if (touche==bas) { 
      for(int i=7;i>=0;i--)
      {
        for (int b=7;b>=0;b--)
        {
          bitWrite( TempoBitMap, 7-b, bitRead(ChessBoard.bitMap[i],b) );       
        }
        strPos = strPos+ TempoBitMap;
        strPos = strPos + ".";
        NewbitMap[7-i]=TempoBitMap;
      }      
    }    

    // si c'est la position de départ on change de fichier
    if (strPos=="195.195.195.195.195.195.195.195.")
    {
      if (SaveOnSD) {
        if (nbPos>20) {
          OpenNewFile();
        }        
      }     
      nbPos=0; 
    }    

    return strPos;
    
}

//******************************************************************************************
//                          SETUP
//******************************************************************************************

void setup(){
//----------------------------------------------------------
//initialisation su port serie
    Serial.begin(57600);
//----------------------------------------------------------
  //initialisation de l'ecran LCD
    lcd.begin(16, 2);

    ChessBoard.setDebounceTime(10);
    ChessBoard.setHoldTime(2000);    

//----------------------------------------------------------
//initialisation de l'ecran LCD
  pinMode(53, OUTPUT);

  lcd.print("LOADING PLEASE");
  lcd.setCursor(0, 1);
  lcd.print("WAIT ");

  delay(500);
  lcd.print(". ");

  Serial.flush();    

  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  
  lcd.clear();

  if (!SD.begin(53)) {
    lcd.print("PAS DE CARTE SD");
    SaveOnSD=false;
  }
  else
  {
    myFile = SD.open("GAME0.ARD", FILE_WRITE);
    lcd.print("CARTE SD OK");
    lcd.setCursor(0, 1);
    lcd.print(nextnumfile());
    lcd.print(" FICHIERS");
    delay(2000);
    if(myFile){
      lcd.print(" YES");
      delay(2000);
      SaveOnSD=true;
      OpenNewFile();
    } else {
      lcd.print(" NO ");
      delay(2000);
      SaveOnSD=false;
    };
    
    
  }
//----------------------------------------------------------
//ou sont placé les blancs ?
  lcd.setCursor(0, 1);
  lcd.print("BLANCS ");
   
  touche=0;
  do 
  {
    keyPress = analogRead(0);
        
    if(keyPress < 65){
         touche=droite;
         lcd.print("A DROITE");
      } else if(keyPress < 221){
        touche=haut;
        lcd.print("EN HAUT");
      } else if(keyPress < 395){
         touche=bas;
         lcd.print("EN BAS");
      } else if(keyPress < 602){
       touche=gauche;
       lcd.print("A GAUCHE");
      } else if(keyPress < 873){
       touche=entree;
      } else {
        touche=aucune;
      }    
     delay(100);
  } while (touche==0);

    delay(2000);
    Serial.flush();    
 
  lcd.setCursor(0, 1);
  lcd.print(". . .         ");
  
}

//******************************************************************************************
//                          LOOP
//******************************************************************************************
  
void loop(){

 // ChessBoard.getKeys() renvoie true en cas de changement de l'état du clavier
 if ( ChessBoard.getKeys() ) {
    
    //calcul la chaine de la position
    permut();

    //affiche les infos sur le port serie
    Serial.println( millis()-startTime );   //temps de stabilité de la position
    Serial.println(strPos);                 //la position 195. ...

    //enregistre la position sur la carte
    if (SaveOnSD) {
      myFile.println(millis()-startTime);
      myFile.println(strPos);
    }    

    //affiche le nombre de pos sur ecran
    //facultatif mais permet de voir que l'enregistrement progresse
    lcd.setCursor(0,1);
    lcd.print("POS : "+String(nbPos)); 
    nbPos ++;
    //affiche la case qui vient de changer
    //facultatif mais permet de tester les cases
    lcdprint();
 
    //initialise le compteur pour mesurer le temps
    startTime = millis(); 
  }
}

