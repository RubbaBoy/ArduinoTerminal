#include <SPI.h>
#include <SD.h>
#include <EthernetV2_0.h>
#include <TouchScreen.h>
#include <TFT.h>
#include <PS2Keyboard.h>

int point_x = 0;
int point_y = 0;
int cache = 0;
String command_cache[100];
int command_length = 0;

const int DataPin = 20;
const int IRQpin =  21;

const int resetPin = 14;

PS2Keyboard keyboard;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

File file;

#define W5200_CS  10
#define SDCARD_CS 44

void setup() {
  keyboard.begin(DataPin, IRQpin);
  Tft.init();
  Serial.begin(9600);

  pinMode(SDCARD_CS, OUTPUT);
  
  pinMode(resetPin, INPUT);
  digitalWrite(resetPin, LOW);
  
  Tft.drawString("UdderOS", getX(), getY(), 2, BLUE);
  setY(getY() + 20);
  setX(0);
  Tft.drawString("Initializing SD card...", 0, getY(), 1, RED);
  setY(getY() + 10);
  if (!SD.begin(SDCARD_CS)) {
    Tft.drawString("Initialization failed!", 0, getY(), 1, RED);
    setY(getY() + 10);
  }
  Tft.drawString("Initialization done.", 0, getY(), 1, RED);
  setY(getY() + 10);
  Tft.drawString("Type in a command", getX(), getY(), 1, BLUE);
  setY(getY() + 10);
  setX(0);
  Tft.drawString(">", getX(), getY(), 1, GREEN);
  setX(20);

}
String TMP = "";
void loop(void) {

  if (keyboard.available()) {

    if (point_x <= 230) { //End of line threshhold
      // read the next key
      char c = keyboard.read();

      if (c == PS2_ENTER) {        
        for (int i = 0; i <= command_length; i++) {
          TMP += command_cache[i];
          Serial.println("T: " + command_cache[i]);
          command_cache[i] = "";          
        }
        command_length = 0;        
        setY(getY() + 10);
        setX(0);
        Serial.println();
        runCommand(TMP);
        Serial.println("ENTETED: " + TMP);
        TMP = "";
      } else if (c == PS2_DELETE) {
        Serial.print("[Del]");
        backspace();
        command_cache[command_length] = "";
        command_length--;
      } else {
        // otherwise, just print all normal characters
        Serial.print(c);
        Tft.drawChar(c, point_x, point_y, 1, GREEN);
        command_length++;
        command_cache[command_length] = c;

      }
      point_x += 10;
    } else {
      point_x = 0;
      point_y = point_y += 10;
    }
  }

  delay(10);
}

void backspace() {
  point_x = point_x - 10;
  if (point_x < 0) {
    point_x = 240;
    point_y -= 10;
    Tft.fillRectangle(point_x, point_y, 10, 10, BLACK);
    point_x = point_x - 10; //Stops the: point_x += 10;
  } else {
    Tft.fillRectangle(point_x, point_y, 10, 10, BLACK);
    point_x = point_x - 10; //Stops the: point_x += 10;
  }
}

void TFTPrintFile(File file, String filename, String extension) {
  file = SD.open(filename + "." + extension);
  if (file) {
    // read all data from the file:
    while (file.available()) {
      char readByte = file.read();
      if (readByte == 13) {
        point_y += 10;
        point_x = 10;
        backspace();
      } else {
        Tft.drawChar(readByte, point_x, point_y, 1, GREEN);
        Serial.print(readByte);
        point_x += 10;
      }
    }
    point_x = 0;
    point_y += 10;

    file.close();
  }
}

void runCommand(String args) {
  String tmp;
  String tmp2;
  tmp2 = args;
  if (args.indexOf(" ") > -1) {
    tmp = args.substring(args.indexOf(" "));
    Serial.println("1: " + tmp);
    tmp2.replace(tmp, "");
    Serial.println("2: " + tmp2);
    tmp2.replace(" ", "");
    tmp = tmp2;
  } else {
    tmp = args;
  }
  if (tmp == "help") {
    help();
  } else if (tmp == "rfile") {

    if(args.indexOf(" ") == 5) {
      String temp = args.substring(6);
      Tft.drawString("Opening file.", getX(), getY(), 1, RED);
      setY(getY() + 10);
      setX(0);
      fileFunct("read", temp);
      temp = "";
    } else {
      Tft.drawString("Try: rfile file.txt", getX(), getY(), 1, RED);
      setY(getY() + 10);
      setX(0);
    }
    
  } else if (tmp == "lfile") {

    if(args.indexOf(" ") == 5) {
      String temp = args.substring(6);
      Tft.drawString("Listing directories", getX(), getY(), 1, RED);
      setY(getY() + 10);
      setX(0);
      fileFunct("list", temp);
      temp = "";
    } else {
      Tft.drawString("Try: lfile -directory", getX(), getY(), 1, RED);
      setY(getY() + 10);
      setX(0);
    }
    
  } else if(tmp == "reset") {
    reset();
  } else if (tmp == "cls") {
    Tft.fillRectangle(0, 0, 240, 320, BLACK);
    setX(0);
    setY(0);
  }  else {
    Tft.drawString("Unknown!", getX(), getY(), 1, RED);
    setY(getY() + 10);
    setX(0);
  }
  setX(0);
  Tft.drawString(">", getX(), getY(), 1, GREEN);
  
}

void reset() {
  pinMode(resetPin, OUTPUT);
}

void newLine() {
  point_y += 10;
  point_x = 0;
}

int getX() {
  return point_x;
}

void setX(int x) {
  point_x = x;
}

int getY() {
  return point_y;
}

void setY(int y) {
  point_y = y;
}

void help() {
  Tft.drawString("Help:", getX(), getY(), 1, RED);
  newLine();
  Tft.drawString("-help | Shows commands", getX(), getY(), 1, RED);
  newLine();
  Tft.drawString("-rfile | reads file", getX(), getY(), 1, RED);
  newLine();
  Tft.drawString("-lfile | lists files in directory", getX(), getY(), 1, RED);
  newLine();
  Tft.drawString("-reset | resets the device", getX(), getY(), 1, RED);
  newLine();
  Tft.drawString("-cls | clears the screen", getX(), getY(), 1, RED);
  newLine();
}

void fileFunct(String args1, String args2) {

  if (args1 == "read") {
    file = SD.open(args2);
    if (file) {
      // read all data from the file:
      while (file.available()) {
        char readByte = file.read();
        if (readByte == 13) {
          setY(getY() + 10);
          setX(10);
          backspace();
        } else {
          Tft.drawChar(readByte, getX(), getY(), 1, GREEN);
          Serial.print(readByte);
          setX(getX() + 10);
        }
      }
      setX(0);
      setY(getY() + 10);

      file.close();
    }
  } else
  if (args1 == "list") {
    printDirectory(SD.open(args2), 0);
  }
}



void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    Serial.print(entry.name());
    Tft.drawString(entry.name(), getX(), getY(), 1, GREEN);
    setX(getX() + strlen(entry.name()) * 10);
    if (entry.isDirectory()) {
      Serial.println("/");
      Tft.drawString("/", 180, getY(), 1, GREEN);
      newLine();
    } else {
      // files have sizes, directories do not
      Serial.println("    ");
      Tft.drawString("    ", getX(), getY(), 1, GREEN);
      setX(getX() + 40);
      Serial.print(entry.size());
      Tft.drawNumber(entry.size(), 180, getY(), 1, GREEN);
      newLine();
    }
    entry.close();
  }
}
