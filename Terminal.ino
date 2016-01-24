#include <SPI.h>
#include <SD.h>
#include <EthernetV2_0.h>
#include <TouchScreen.h>
#include <TFT.h>
#include <PS2Keyboard.h>
//#include "commands.cpp"

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

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

File file;

String root_string = "";
File root;

char* temporary[100];

//char* before = "C:" + root_string + ">";

#define W5200_CS  10
#define SDCARD_CS 44

void setup() {
  keyboard.begin(DataPin, IRQpin);
  Tft.init();
  Serial.begin(9600);

  pinMode(SDCARD_CS, OUTPUT);
  
  pinMode(resetPin, INPUT);
  digitalWrite(resetPin, LOW);
  
  root = SD.open(root_string);

  

//  Serial.println("done!");
  


  Tft.drawString("UdderOS", getX(), getY(), 2, BLUE);
  setY(getY() + 20);
  setX(0);
  Tft.drawString("Initializing SD card...", 0, getY(), 1, RED);
  setY(getY() + 10);
  if (!SD.begin(SDCARD_CS)) {
    Tft.drawString("Initialization failed!", 0, getY(), 1, RED);
    setY(getY() + 10);
    return;
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

    //if (point_x >= 240 && point_y >= 320) {
    //
    //} else
    if (point_x <= 230) { //End of line threshhold
      // read the next key
      char c = keyboard.read();
      //    Tft.drawString(c, 0, 0, 10, RED);

      if (c == PS2_ENTER) {        
        //      String args[command_length];
        //      int temp = 0;
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
        


        //      newLine();
        //      Tft.drawString("", point, 0, 1, RED);
        //    } else if (c == PS2_TAB) {
        //      Serial.print("[Tab]");
        //      Tft.drawString("[Tab]", point_x, point_y, 1, GREEN);
        //    } else if (c == PS2_ESC) {
        //      Serial.print("[ESC]");
        //      Tft.drawString("[ESC]", point_x, point_y, 1, GREEN);
        //    } else if (c == PS2_PAGEDOWN) {
        //      Serial.print("[PgDn]");
        //      Tft.drawString("[PgDn]", point_x, point_y, 1, GREENED);
        //    } else if (c == PS2_PAGEUP) {
        //      Serial.print("[PgUp]");
        //      Tft.drawString("[PgUp]", point_x, point_y, 1, GREEND);
        //    } else if (c == PS2_LEFTARROW) {
        //      Serial.print("[Left]");
        //      Tft.drawString("[Left]", point_x, point_y, 1, GREEND);
        //    } else if (c == PS2_RIGHTARROW) {
        //      Serial.print("[Right]");
        //      Tft.drawString("[Right]", point_x, point_y, 1, GREEN);
        //    } else if (c == PS2_UPARROW) {
        //      Serial.print("[Up]");
        //      Tft.drawString("[Up]", point_x, point_y, 1, GREENED);
        //    } else if (c == PS2_DOWNARROW) {
        //      Serial.print("[Down]");
        //      Tft.drawString("[Down}", point_x, point_y, 1, GREEN);
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
    tmp2.replace(" ", ""); // first was " -"
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
//      const char *mystring = temp;
//      Tft.drawString(mystring, getX(), getY(), 1, RED);
      setY(getY() + 10);
      setX(0);
      fileFunct("read", root_string);
      temp = "";
    } else {
      Tft.drawString("Try: rfile file.txt", getX(), getY(), 1, RED);
      setY(getY() + 10);
      setX(0);
    }
    
  } else if (tmp == "lfile") {

//    if(args.indexOf(" ") == 5) {
      String temp = args.substring(6);
      Tft.drawString("Listing directories", getX(), getY(), 1, RED);
//      const char *mystring = temp;
//      Tft.drawString(mystring, getX(), getY(), 1, RED);
      setY(getY() + 10);
      setX(0);
      fileFunct("list", root_string);
      temp = "";
//    } else {
//      Tft.drawString("Try: lfile -directory", getX(), getY(), 1, RED);
//      setY(getY() + 10);
//      setX(0);
//    }
    
  } else if(tmp == "reset") {
    reset();
  } else if (tmp == "cls") {
    Tft.fillRectangle(0, 0, 240, 320, BLACK);
    setX(0);
    setY(0);
  } else if (tmp == "cd") {
    String temp2;
  if(args.indexOf(" ") == 2) {
      String temp = args.substring(3);
      if (temp != "..") {
  //      newLine();
  //      if (temp == "..") {
  //        temp2 = root_string.substring(root_string.lastIndexOf("/"));
  //        root_string.replace(temp2, "");
  ////        root_string
  //        root = SD.open(root_string);
  //      }
  //      root_string = root_string + temp;
        temp.replace("/", "");
        root = SD.open(root_string + temp);
        Serial.println("TEMP: " + temp);
        root_string = root_string + "/" +  temp;
      } else {
        String t = root_string;
        int temp666 = root_string.lastIndexOf("/");
        if (root_string.lastIndexOf("/") < 0) {
          temp666 = root_string.length();
        }
        String temp66 = root_string.substring(temp666);
        t.replace(temp66, "");
        root_string = t;
      }
      temp = "";
    } else {
      Tft.drawString("Try: cd folder", getX(), getY(), 1, RED);
      setY(getY() + 10);
      setX(0);
    }
    
  } else {
    Tft.drawString("Unknown!", getX(), getY(), 1, RED);
    setY(getY() + 10);
    setX(0);
  }
//  before = "C:" + root_string + ">";
  char temporary[root_string.length() + 1];
  int t = root_string.length() + 1;
  root_string.toCharArray(temporary, root_string.length() + 1);
  setX(0);
  Tft.drawString(temporary, getX(), getY(), 1, GREEN);
  setX(root_string.length() * 10);
//  setX(10);
  Serial.println(root_string.length() * 10 - 10);
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
  Tft.drawString("-cd | changes selected directory", getX(), getY(), 1, RED);
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
//    root = SD.open(args2);
    printDirectory(SD.open(args2), 0);
  }
//  digitalWrite(SDCARD_CS, HIGH);
}

//void tftWrite(String string, int x, int y, int Size, unsigned int color) {
//  Tft.drawString(string, x, y, Size, color);
//}

//void tftWrite(char Char, int x, int y, int Size, unsigned int color) {
//  Tft.drawChar(Char, x, y, Size, color);
//}




void printDirectory(File dir, int numTabs) {

//  if (!SD.begin(SDCARD_CS)) {
//    Tft.drawString("Initialization failed!", getX(), getY(), 1, RED);
//    setY(getY() + 10);
//    return;
//  }
  
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
//    for (uint8_t i = 0; i < numTabs; i++) {
//      Serial.print("  ");
//      Tft.drawString("  ", getX(), getY(), 1, GREEN);
//      setX(getX() + 20);
//    }
    Serial.print(entry.name());
    Tft.drawString(entry.name(), getX(), getY(), 1, GREEN);
    setX(getX() + strlen(entry.name()) * 10);
    if (entry.isDirectory()) {

      
      Serial.println("/");
//      setY(getY() + 10);
      Tft.drawString("/", 180, getY(), 1, GREEN);
//      setY(getY() - 10);
      newLine();
//      printDirectory(entry, numTabs + 1);
    } else {

      
      // files have sizes, directories do not
//      setY(getY() + 10);
      Serial.println("    ");
//      setY(getY() + 10);
      Tft.drawString("    ", getX(), getY(), 1, GREEN);
//      setY(getY() - 10);
      setX(getX() + 40);
//      Serial.println("T: ");
      Serial.print(entry.size());
//      setY(getY() + 10);
      Tft.drawNumber(entry.size(), 180, getY(), 1, GREEN);
//      setY(getY() - 10);
//      setX(strlen(String(entry.size())) * 10);
      newLine();
    }
    entry.close();
  }
}
