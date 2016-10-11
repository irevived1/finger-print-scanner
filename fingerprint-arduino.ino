//Author: 	H. Wu
//Class:	MAT 400 Section 1
//Instructor:	Prof. Dietrich
// include the library code:
#include <Bounce.h>
#include <LiquidCrystal.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

uint8_t id;
uint8_t getFingerprintEnroll(uint8_t id);
uint8_t deleteFingerprint(uint8_t id);
int getFingerprintIDez();

// Software serial for when you dont have a hardware serial port
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(7, 6);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


//bounce buttons
Bounce b8 = Bounce(8,5);
Bounce b9 = Bounce(9,5);
Bounce b10 = Bounce(10,5);
Bounce b13 = Bounce(13,5);

//int for ID:
uint8_t fin_id = 1;
//int for mode;
int m0de = 1;
boolean ex = false;
int ind = -1;
boolean good = false;

//timer
unsigned long currMill;
unsigned long prevMill;
long inte = 5000; //five second



void setup()  
{
  // set up the number of columns and rows on the LCD
  lcd.begin(16, 2);
  //pin modes, 4 buttons
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(10,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);


  
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(500);
  Serial.begin(9600);
  // set the data rate for the sensor serial port
  finger.begin(57600);

  
  if (finger.verifyPassword()) {
    lcd.print("Found sensor!");
  } else {
    lcd.print("404 sensor :(");
    while (1);
  }
}


void loop()                     // run over and over again
{
  //this button is to rotate between the modes, enrollment mode, deletion mode and scanning mode

  //switch statement to rotate the mode
  switch ( m0de )
  {
    case 1:
      lcd.clear();
      lcd.print("Scanning Mode");
      while(1)
      {
          b13.update();
          b8.update();
          if ( b13.fallingEdge() )
          {
            m0de++;
              if ( m0de == 4 )
              m0de = 1;
            ex = true;
            break;
          }
          if( b8.fallingEdge() )
            break;
          
      }
        if ( ex )
        {
            ex = false;          
            break;
          }
      lcd.clear();
      lcd.print("Scanning");
      prevMill = millis();
      currMill = millis();
      while (  (currMill - prevMill < inte ) )
      {
      currMill = millis();
      //debugging statment, print random stuff on screen lol
      //lcd.print("lol");
      
      ind = getFingerprintIDez();
      delay(50);
      //if ( ind >= 1 && ind <= 5  );
      //  break;
      if ( good )
      break;
      }
      lcd.setCursor(0,1);
      if ( ind >= 1 && ind <= 5 )
      {  lcd.print("Match Found #");
          lcd.print(ind);
      }
      else
        lcd.print("Match Not Found");
      delay(2000);
    break;

    case 2:
    //case 2 for enrollment
     lcd.clear();
     lcd.print("Select the ID # ");

        lcd.setCursor(0,1);
        lcd.print("ID # ");
        lcd.print(fin_id);
        while(1) {
          //update status of the buttons
          b8.update();
          b9.update();
          b10.update();
          b13.update();
      
          lcd.setCursor(5,1);
          lcd.print(fin_id);
      
          if ( b9.fallingEdge() )
          {
            fin_id --;
            if ( fin_id == 0 )
              fin_id = 5;
            }
          if ( b10.fallingEdge() )
          {
            fin_id ++;
            if ( fin_id == 6 )
              fin_id = 1;
            }
          
          if ( b8.fallingEdge() )
            break;
          if ( b13.fallingEdge() )
          {
            ex = true;
            m0de++;
              if ( m0de == 4 )
              m0de = 1;
            break;
          }
            }
        if ( ex )
        {
            ex = false;          
            break;
          }

      lcd.setCursor(0,1); 
      lcd.print("Enrolling ID #");
      lcd.print(fin_id);
      while (!  getFingerprintEnroll(fin_id) );    
      break;

    //case 3 for deleting
    case 3:

        lcd.clear();
        lcd.print("Select Delete");
        lcd.setCursor(0,1);
        lcd.print("ID # ");
        lcd.print(fin_id);
        while(1) {
          //update status of the buttons
          b8.update();
          b9.update();
          b10.update();
          b13.update();
      
          lcd.setCursor(5,1);
          lcd.print(fin_id);
      
          if ( b9.fallingEdge() )
          {
            fin_id --;
            if ( fin_id == 0 )
              fin_id = 5;
            }
          if ( b10.fallingEdge() )
          {
            fin_id ++;
            if ( fin_id == 6 )
              fin_id = 1;
            }
          
          if ( b8.fallingEdge() )
            break;
          if ( b13.fallingEdge() )
          {
            ex = true;
            m0de++;
              if ( m0de == 4 )
              m0de = 1;
            break;
          }
            }
        if ( ex )
        {
            ex = false;          
            break;
          }
    lcd.clear();
    lcd.print("deleting ID #");
    lcd.print(fin_id);
  
    deleteFingerprint(fin_id);

    break;

    default:
      lcd.clear();
      lcd.print("Mode not found");
      break;
    }

}
//end of main loop

//this whole section is enrollment
uint8_t getFingerprintEnroll(uint8_t id) {

  int p = -1;
  lcd.clear();
  lcd.print("Waiting...."); 
  lcd.print(id);
  lcd.setCursor(0,1);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    lcd.clear();
    switch (p) {
    case FINGERPRINT_OK:
      lcd.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      lcd.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      lcd.print("Imaging error");
      break;
    default:
      lcd.print("Unknown error");
      break;
    }
  }

  // OK success!
  
  p = finger.image2Tz(1);
  lcd.clear();
  switch (p) {
    case FINGERPRINT_OK:
      lcd.print("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.print("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.print("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.print("Could not find fingerprint features");
      return p;
    default:
      lcd.print("Unknown error");
      return p;
  }

  lcd.clear();
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  lcd.clear();
  
  lcd.print("ID "); lcd.println(id);
  p = -1;
  lcd.setCursor(0,1);
  lcd.print("Place same finger again");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    lcd.clear();
    switch (p) {
    case FINGERPRINT_OK:
      lcd.print("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      lcd.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      lcd.print("Imaging error");
      break;
    default:
      lcd.print("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    lcd.clear();
    case FINGERPRINT_OK:
      lcd.print("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.print("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.print("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.print("Could not find fingerprint features");
      return p;
    default:
      lcd.print("Unknown error");
      return p;
  }
  
  // OK converted!
  lcd.clear();
  lcd.print("Creating #");  lcd.print(id);
  
  p = finger.createModel();
  lcd.setCursor(0,1);
  if (p == FINGERPRINT_OK) {
    lcd.print("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.print("Fingerprints did not match");
    return p;
  } else {
    lcd.print("Unknown error");
    return p;
  }   

  lcd.clear();
  lcd.print("ID "); lcd.print(id);
  lcd.setCursor(0,1);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.print("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.print("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.print("Error writing to flash");
    return p;
  } else {
    lcd.print("Unknown error");
    return p;
  }   
}
//enrollment ends here

//deleting
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  lcd.setCursor(0,1);
  p = finger.deleteModel(id);
  

  if (p == FINGERPRINT_OK) {
    lcd.print("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.print("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.print("Error writing to flash");
    return p;
  } else {
    lcd.print("Unknown error: 0x"); Serial.println(p, HEX); 
    return p;
  }   
  delay(2000);
}
//end of deleting


//scanning

int getFingerprintIDez() {
  uint8_t p = finger.getImage();

  if (p != FINGERPRINT_OK){ good = false; return -1; }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK){ good = false;  return -1 ;}

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) { good = false; return -1; }
  
  // found a match!
  //Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  //Serial.print(" with confidence of "); Serial.println(finger.confidence);
  good = true;
  return finger.fingerID; 
}
