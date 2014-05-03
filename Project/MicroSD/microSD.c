
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "fatfs/diskio.h"
#include "fatfs/ff.h"
#include "fatfs/ffconf.h"
#include "fatfs/integer.h"
#include "driverlib/rom.h"
#include "driverlib/ssi.h"


FATFS fs;
FIL data_log, status_log;
FRESULT res;
UINT br,bw;


void setup_microSD()
{   
    //Mount the FatFS System
    f_mount(0, &fs);
    
    start_datalog();
    

}


void close()
{
    // Close all files
    f_close(&data_log);

}

void unmount()
{
    f_mount(0, NULL);

}
void open_datalog(){

    //Open source file
    f_open(&data_log, "datalog.csv", FA_WRITE | FA_OPEN_ALWAYS);
    f_lseek(&data_log, data_log.fsize);
}

void start_datalog(){
     //Open source file
    f_open(&data_log, "datalog.csv", FA_WRITE | FA_OPEN_ALWAYS);
    f_lseek(&data_log, data_log.fsize);

    //Data Separation
    char end_line[2] = "\n";
    f_write(&data_log, end_line , sizeof(end_line) , &bw);

}



void write_datalog(char *velocity, char *coordX, char *coordY, char *time_log, char *ambient_temp, char *motor_temp, char *battery_percentage)
{   
    //Initial char space
    char row_log[128];
    sprintf(row_log, "%s, %s, %s, %s,%s, %s, %s \n", velocity, coordX, coordY, time_log, ambient_temp, motor_temp, battery_percentage);
   
    //Writes to file the data obtained
    f_write(&data_log, row_log, strlen(row_log), &bw);
}

