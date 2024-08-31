#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define I2C_SDA 12
#define I2C_SCL 14

#define SPI_MISO    34
#define SPI_MOSI    33
#define SPI_SS      26
#define SPI_CLK     25

#define N_DATALOG 1

RTC_DS3231 _rtc;
DateTime date;

enum state_general_t {
    RTC_STATE,
    SD_STATE,
    FREEZE_STATE
};

enum state_sd_t {
    SD_INIT,
    SD_CREATE_DIR,
    SD_CREATE_FILE,
    SD_APPEND_FILE
};

enum state_rtc_t {
    RTC_INIT,
    RTC_READ
};

void me_general();
bool me_sd();
bool me_rtc();

char f_string[64];
static const char* f_title = "datalog.txt"; 

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

bool sd_create_dir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)) {
        Serial.println("Dir created");
        return true;
    } else {
        Serial.println("mkdir failed");
        return false;
    }
}

void removeDir(fs::FS &fs, const char * path){
  Serial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

bool sd_write_file(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return false;
  }
  if(file.print(message)){
    Serial.println("File written");
    file.close();
    return true;
  } else {
    Serial.println("Write failed");
    file.close();
    return false;
  }
  
}

bool sd_append_file(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return false;
    }
    if(file.print(message)){
        Serial.println("Message appended");
        file.close();
        return true;
    } else {
        Serial.println("Append failed");
        file.close();
        return false;
    }
}

unsigned long sd_get_size(fs::FS &fs, const char * path){
    unsigned long ret;
    File file = fs.open(path);
    if(!file)
        return 0;
    else
        ret = file.size();
        
    file.close();
    return ret;
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for(i=0; i<2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

bool sd_init() {
    SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_SS);
    SPI.setDataMode(SPI_MODE0);

    if(!SD.begin(SPI_SS)){
        Serial.println("Card Mount Failed");
        return false;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return false;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    return true;
}

void init_i2c() {
    Wire.begin(I2C_SDA, I2C_SCL);
}

bool rtc_init() {
    if (!_rtc.begin()){
        Serial.println(F("RTC init failed"));
        return false;
    }
    _rtc.clearAlarm(1);
    _rtc.clearAlarm(2);
    Serial.println(F("RTC init OK"));
    return true;   
}

void rtc_set_time(DateTime d){
    if(_rtc.begin()){
        _rtc.adjust(d);
    }
}

void setup(){
    Serial.begin(9600);
    init_i2c();

}

void loop(){
    me_general();
}

void me_general(){
    static state_general_t state_general = RTC_STATE;

    switch(state_general) {
        case RTC_STATE:
            if(!me_rtc()) {
                state_general = FREEZE_STATE;
            }
            break;

        case SD_STATE:
            if(!me_sd()) {
                state_general = FREEZE_STATE;
            }
            break;     

        case FREEZE_STATE:
            while(1);
            break;
    }    
}

bool me_rtc() {
    static state_rtc_t state_rtc = RTC_INIT;

    switch(state_rtc) {
        case RTC_INIT:
            if(rtc_init()) {
                Serial.println("rtc_init() OK!");
                state_rtc = RTC_READ;
            } else {
                // como vamos a manejar los errores?
                return false;
            }
            break;  
        case RTC_READ:
            date = _rtc.now();
            Serial.printf("%02d/%02d/%d - %02d:%02d:%02d\n", date.day(), date.month(), date.year(), date.hour(), date.minute(), date.second());
            state_rtc = RTC_READ;
            return false;
            break;
    }
    return true;
}

bool me_sd() {
    static state_sd_t state_sd = SD_INIT;

    switch(state_sd) {
        case SD_INIT:
            if(sd_init()) {
                Serial.println("sd_init() OK!");
                state_sd = SD_CREATE_DIR;
            } else {
                // como vamos a manejar los errores?
                return false;
            }
            break;
        case SD_CREATE_DIR:
            if(sd_create_dir(SD, "/ciclo_01")) {
                state_sd = SD_CREATE_FILE;
            } else {
                // como vamos a manejar los errores?
                return false;
            }
            break;
        case SD_CREATE_FILE:
            sprintf(f_string, "/ciclo_%02d/%s", N_DATALOG, f_title);
            if(sd_get_size(SD, f_string) == 0){
                if(sd_write_file(SD, f_string, "Fecha,Hora,pH,DO,Temperatura,LED1,LED2,LED3,LED4,LED5\n")) {
                    state_sd = SD_APPEND_FILE;
                    return true;
                } else {
                    // como vamos a manejar los errores?
                    return false;
                }
            } else {
                state_sd = SD_APPEND_FILE;
                return true;
            }
            break;
        case SD_APPEND_FILE:
            sd_append_file(SD, f_string, "31/08/2024,13:00:00,7.00,50.00,20.00,10.00,10.00,10.00,10.00,10.00\n");
            return false;            
            break;
    }
    return true;
}