#include "spi_sd.h"
#include "rtc.h"
#include "ezo_ph.h"

extern DateTime date;
extern pH pH_Device;


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

bool sd_remove_dir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
        return true;
    } else {
        Serial.println("rmdir failed");
        return false;
    }
}

bool sd_read_file(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return false;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
    return true;
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
    Serial.printf("Message to append: %s\n", message);

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

bool sd_rename_file(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
        return true;
    } else {
        Serial.println("Rename failed");
        return false;
    }
}

bool sd_delete_file(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
        return true;
    } else {
        Serial.println("Delete failed");
        return false;
    }
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

uint8_t me_sd() {
    static state_sd_t state_sd = SD_INIT;
    static const char* f_title = "datalog.txt";
    static char f_path[64]; 
    static char f_msg[128]; 
    static int n_datalog = 1;

    switch(state_sd) {
        case SD_INIT:
            if(sd_init()) {
                Serial.println("sd_init() OK!");
                state_sd = SD_CREATE_DIR;
            } else {
                // como vamos a manejar los errores?
                return ME_ERROR;
            }
            break;
        case SD_CREATE_DIR:
            sprintf(f_path, "/ciclo_%02d", n_datalog);
            if(sd_create_dir(SD, f_path)) {
                state_sd = SD_CREATE_FILE;
            } else {
                // como vamos a manejar los errores?
                return ME_ERROR;
            }
            break;
        case SD_CREATE_FILE:
            sprintf(f_path, "/ciclo_%02d/%s", n_datalog, f_title);
            if(sd_get_size(SD, f_path) == 0){
                if(sd_write_file(SD, f_path, "Fecha,Hora,pH,DO,Temperatura,LED1,LED2,LED3,LED4,LED5\n")) {
                    state_sd = SD_APPEND_FILE;
                } else {
                    // como vamos a manejar los errores?
                    return ME_ERROR;
                }
            } else {
                state_sd = SD_APPEND_FILE;
            }
            break;
        case SD_APPEND_FILE:
            sprintf(f_msg, "%02d/%02d/%d,%02d:%02d:%02d,%.2lf,50.00,20.00,10.00,10.00,10.00,10.00,10.00\n", 
                            date.day(), date.month(), date.year(), date.hour(), date.minute(), date.second(), pH_Device.get_last_received_reading());
            if(sd_append_file(SD, f_path, f_msg)) {
                return ME_FINISHED; 
            } else {
                return ME_ERROR; 
            }                       
            break;
    }
    return ME_PROCESSING;
}
