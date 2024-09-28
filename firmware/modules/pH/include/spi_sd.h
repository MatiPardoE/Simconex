#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "utilities.h"

#define SPI_MISO    34
#define SPI_MOSI    33
#define SPI_SS      26
#define SPI_CLK     25

enum state_sd_t {
    SD_INIT,
    SD_CREATE_DIR,
    SD_CREATE_FILE,
    SD_APPEND_FILE
};

bool sd_create_dir(fs::FS &fs, const char * path);
bool sd_remove_dir(fs::FS &fs, const char * path);
bool sd_read_file(fs::FS &fs, const char * path);
bool sd_write_file(fs::FS &fs, const char * path, const char * message);
bool sd_append_file(fs::FS &fs, const char * path, const char * message);
unsigned long sd_get_size(fs::FS &fs, const char * path);
bool sd_rename_file(fs::FS &fs, const char * path1, const char * path2);
bool sd_delete_file(fs::FS &fs, const char * path);
bool sd_init();
uint8_t me_sd();