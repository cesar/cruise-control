
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
    Serial.println("Mounting");
    //Mount the FatFS System
    f_mount(0, &fs);

   

 
}

void close()
{       Serial.println("Closing");

    // Close all files
    // f_close(&status_log);
    f_close(&data_log);
    f_mount(0, NULL);
    Serial.println("Closed");


}
void open_datalog(){

    //Open source file
  int j =  f_open(&data_log, "datalog.csv", FA_WRITE | FA_OPEN_ALWAYS);
   
   Serial.println(j);
   j =  f_lseek(&data_log, data_log.fsize);
Serial.println(j);
    //Data Separation
    char end_line[2] = "\n";
   j = f_write(&data_log, end_line , sizeof(end_line) , &bw);
   Serial.println(j);

    }

void open_statuslog()
{   
    //Open Status_log File
    f_open(&status_log, "status_log.txt", FA_WRITE | FA_OPEN_ALWAYS);
}

void write_datalog(char *velocity, char *coordX, char *coordY, char *time_log, char *ambient_temp, char *motor_temp)
{   
    //Initial char space
    char row_log[128];
    sprintf(row_log, "%s, %s, %s, %s,%s, %s \n", velocity, coordX, coordY, time_log, ambient_temp, motor_temp);
   
    //Writes to file the data obtained
   int j= f_write(&data_log, row_log, strlen(row_log), &bw);
   Serial.println(j);
}

void setup()
{
    Serial.begin(9600);

    
}
void loop(){
    delay(1000);
            setup_microSD();

        open_datalog();
        close();
    }
