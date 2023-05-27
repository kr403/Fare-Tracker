//gps library
#include <TinyGPS.h>

//rfid library
#include <MFRC522.h>
#include <SPI.h>

//display library
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//sd card library
#include <SD.h>
#include <SPI.h>

#define CS_RFID 53  //rfid
#define RST_RFID 49  //rfid
#define CS_SD 7  //sd card


File myFile1;
File myFile2;
File myFile3;
File temp;

int c = 0;
int flag = 0;
float dist = 0.000000;
float latp, lonp;
float dlat, dlon;

MFRC522 rfid(CS_RFID, RST_RFID);  // Instance of the class
float lat, lon; // create variable for latitude and longitude object
TinyGPS gps; // create gps object
LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {
  pinMode(8 , OUTPUT);
  pinMode(9 , OUTPUT);
  pinMode(10 , OUTPUT);
  Serial.begin(115200); // connect serial
  Serial2.begin(9600); // connect gps sensor

  lcd.begin();

  SPI.begin(); // Init SPI bus

  rfid.PCD_Init(); // Init MFRC522

  delay(2000);

  rfid.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  // Setup for the SD card
  Serial.print("Initializing SD card...");
  // Turn on the blacklight and print a message.

  lcd.backlight();
  lcd.print("Initializing ");
  lcd.setCursor(0, 1);
  lcd.print("SD card...");

  if (!SD.begin(CS_SD)) {
    lcd.clear();
    Serial.println("initialization failed!");
    lcd.print("Initializing ");
    lcd.setCursor(0, 1);
    lcd.print("failed!");
    return;
  }
  Serial.println("initialization done.");
  lcd.clear();
  lcd.print("Initialization ");
  lcd.setCursor(0, 1);
  lcd.print("Done...");
}


void loop()
{

  if (Serial2.available()) {
    if (gps.encode(Serial2.read())) {
      gps.f_get_position(&lat, &lon); // get latitude and longitude

      //    total distance

      if (dist == 0.000000)
      {
        lonp = lon;
        latp = lat;
        dlat = lat - latp;
        dlon = lon - lonp;

        dist = (sin(dlat / 2)) * (sin(dlat / 2)) + (cos(latp) * cos(lat) * ((sin(dlon / 2)) * (sin(dlon / 2))));

        dist = 2 * (asin(sqrt(dist)));
        long int R = 6371;
        dist *= R;
        Serial.println(dist, 6);
      }
      else
      {
        dlat = lat - latp;
        dlon = lon - lonp;

        dist = (sin(dlat / 2)) * (sin(dlat / 2)) + (cos(latp) * cos(lat) * ((sin(dlon / 2)) * (sin(dlon / 2))));

        dist = 2 * (asin(sqrt(dist)));
        long int R = 6371;
        dist *= R;
        Serial.println(dist, 6);
      }

      digitalWrite(8, LOW);
      digitalWrite(10, LOW);
      digitalWrite(9, HIGH);
      // display position
      Serial.println("Position");
      //Latitude
      Serial.print("Latitude: ");
      Serial.print(lat, 6);
      Serial.print(",");
      //Longitude
      Serial.print("Longitude: ");
      Serial.println(lon, 6);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Lat->");
      lcd.print(lat, 6);
      lcd.setCursor(0, 1);
      lcd.print("Lon->");
      lcd.print(lon, 6);

      if (rfid.PICC_IsNewCardPresent() & rfid.PICC_ReadCardSerial()) {
        //rfid.PICC_DumpToSerial(&(rfid.uid));
        digitalWrite(9, LOW);
        digitalWrite(10, HIGH);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("UID: ");
        String content = "";
        byte letter;

        lcd.setCursor(0, 1);
        for (byte i = 0; i < rfid.uid.size; i++)
        {
          lcd.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          lcd.print(rfid.uid.uidByte[i], HEX);
          content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
          content.concat(String(rfid.uid.uidByte[i], HEX));
        }

        // Enables SD card chip select pin
        digitalWrite(CS_SD, LOW);
        Serial.println();
        Serial.print("UID: ");
        Serial.println(content);
        Serial.println();
        delay(500);




        myFile1 = SD.open("file1.txt", FILE_READ);
        int i = -1;
        char v[100];
        char x;
        String vv = "";
        String xx = "";
        while (myFile1.available()) {
          x = myFile1.read();
//          xx = myFile1.read();
//          Serial.println(xx);
          i++;
          if (x == '\n') {
            i = -1;
            vv = String(v);
            vv = vv.substring(0, 12);
            Serial.println(vv);
            c = 2;
            if (vv == content) {
              c = 1;
              //              Serial.println(c);
              break;
            }
          }
          else
          {
            v[i] = x;
          }
        }


        if (c == 2) {
          c = 0;
          //          Serial.println(c);
          Serial.println("Unauthorized");
          lcd.clear();
          lcd.print("Unauthorized");
        }

        else if (c == 1) {
          //          Serial.println(c);
          c = 0;
          //          Serial.println(c);
          lcd.clear();
          lcd.print("Authorized");
          Serial.println("Authorized");
          //file1 closed
          myFile1.close();/////////////////////////////////////////
          // Disables SD card chip select pin
          digitalWrite(CS_SD, HIGH);
          digitalWrite(CS_SD, LOW);
          myFile3 = SD.open("file3.txt", FILE_READ);
          int j = 0;
          char var[123];
          char y;
          String vvv = "";
          int g = 0;
          String carry = "";
          Serial.println("Working before myFile3.available()");
          while (myFile3.available()) {
            
            y = myFile3.read();
            if (y == '\n') {
              temp = SD.open("temp.txt", FILE_WRITE);
              g = 0;
              vvv = String(var);
              vvv = vvv.substring(0, j);
              j = 0;
              
              //              Serial.println(content);
              //              Serial.println(content.length());


              if (vvv.substring(0,12) == content) {
                Serial.println("Working after 240");
                flag = 1;
                carry = vvv;
                myFile2 = SD.open("file2.txt", FILE_WRITE);
                if (myFile3.available()) {
                  Serial.println("Working after 245");
                  float pd, psd, tb;
                  int e = 0, index, count = 0;
                  String di = "";
                  bool found = false;
                  String var_st = String(var);
                  while(true){
                    if (count == 4) {
                      di = var_st.substring(index + 1, e - 1);
                      break;
                    }
                    if(var[e]==','){
                      count++;
                      if(count==3 && found == false){
                        index = e;
                        found = true;
                        Serial.println("Working after 255");
                      }
                    }
                    e++;  
                  }
                  count = 0;
                  Serial.println("Working after 265");
//                  while (true) {
//                    if (count == 3) {
//                      d[f] = var[e];
//                      f++;
//                    } else {
//                      if (var[e] == ',') {
//                        count++;
//                        if (count == 4) {
//                          break;
//                        }
//                      }
//                    }
//                    e++;
//                  }
                  
                  Serial.println("Working after 279");
                  Serial.println(di);
                  psd = di.toFloat();
                  Serial.println(psd);
                  pd = dist - psd;
                  tb = pd * 2.20;
                  Serial.println("Working after 289");
                  carry = carry + "," + lat + "," + lon + "," + dist + "," + pd + "," + tb + "!";
                  Serial.println(carry);
                  myFile2.println(carry);
                  myFile2.close();
                }
              }
              else {
                Serial.println(vvv);
                temp.println(vvv);
                
              }
              temp.close();
            }
            else {
              if (g == 0) {
                var[j] = y;
                j++;
              }
              if (y == '~') {
                g = 1;
              }
            }
          }
          temp.close();
          myFile3.close();
          SD.remove("file3.txt");
          
          temp = SD.open("temp.txt", FILE_READ);
          j = 0;
          vvv = "";
          g = 0;
          
          while (temp.available()) {
            
            y = temp.read();
            if (y == '\n') {
              myFile3 = SD.open("file3.txt", FILE_WRITE);
              g = 0;
              vvv = String(var);
              vvv = vvv.substring(0, j);
              j = 0;
              Serial.println(vvv);
              myFile3.println(vvv);
              myFile3.close();
            }
            else {
              if (g == 0) {
                var[j] = y;
                j++;
              }
              if (y == '~') {
                g = 1;
              }
            }
          }
          myFile3.close();
          SD.remove("temp.txt");
          
          if (flag == 1) {
            flag = 0;
            Serial.println("Found");

          }
          else {
            myFile3.close();
            // Disables SD card chip select pin
            digitalWrite(CS_SD, HIGH);
            digitalWrite(CS_SD, LOW);
            myFile3 = SD.open("file3.txt", FILE_WRITE);
            // If the file opened ok, write to it
            if (myFile3) {
              Serial.println(content);
              myFile3.print(content);
              myFile3.print(",");
              myFile3.print(lat, 6);
              myFile3.print(",");
              myFile3.print(lon, 6);
              myFile3.print(",");
              myFile3.print(dist);
              myFile3.print(",");
              myFile3.println("~");
              myFile3.close();
              Serial.println("Successful");
              //          lcd.clear();
              //          lcd.print("Successful");
            }
          }





          //        // Open file
          //        myFile=SD.open("DATA.txt", FILE_WRITE);
          //
          //        // If the file opened ok, write to it
          //        if (myFile) {
          //          Serial.println("Saved on database");
          //          myFile.print(content);
          //          myFile.print(", ");
          //          myFile.close();
          //          lcd.clear();
          //          lcd.print("Successful");
          //        }
          //        else {
          //          Serial.println("error opening data.txt");
          //          lcd.clear();
          //          lcd.print("error opening data.txt");
          //        }

          // Disables SD card chip select pin
          digitalWrite(CS_SD, HIGH);
        }
      }
    }
    else {
      digitalWrite(10, LOW);
      digitalWrite(9, LOW);
      digitalWrite(8, HIGH);
    }
  }
}
