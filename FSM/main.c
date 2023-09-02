/*
    For DEBUG info compile with -D=DEBUG
    For LCD compile with -D=LCD
    For TERMINAL compile with -D=TERMINAL
*/

/*
    This should work as it has been tested on received data.
    The only thing left to do here is to add the correct HAL transmition/receive functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h> // only used to create a delay on Linux - remove later

#define ARRAY_LENGTH 64

typedef struct {
    uint16_t pm1MC, pm25MC, pm10MC,
             pm1Air, pm25Air, pm10Air,
             particles03, particles05, particles1,
             particles25, particles50, particles10,
             tvoc, eCO2, reserved1,
             tComp, rhComp, tRaw,
             rhRaw;
    uint8_t aqi, reserved2, version, errorCode;
} apcData;

typedef enum {
    SETUP,
    GET_DATA,
    PROCESS_DATA,
    OUTPUT_DATA,
} STATE;

apcData apc;

bool setupDevice() { // add HAL

    #ifdef LCD
    
    printf("SETUP LCD ...\n");
    
    #endif

    #ifdef TERMINAL
    
    printf("SETUP TERMINAL ...\n");
    
    #endif

    return true;

}

bool sendCmd(uint8_t *cmd) { // add HAL

    printf("SENDING COMMAND ...\n");

    return true;

}

bool getValues(uint8_t *rcv) {

    uint16_t sum = 0;
    for (int i = 0; i<62; i++) { // calculate checksum
        sum += rcv[i];
    }
    #ifdef DEBUG

    printf("SUM: %d\n", sum);

    #endif
    if (sum != (((uint16_t) rcv[62] << 8) | rcv[63])) { // check, if checksum is correct
        #ifdef DEBUG

        printf("Checksum error!\n");
        
        #endif
        return false;
    }
    #ifdef DEBUG

    printf("CHECKSUM OK!\n");

    #endif
    uint16_t arr[19];
    int i = 4, index = 0; // i starts at 4 to skip the first four bytes
    uint16_t converted16;
    uint32_t converted32;
    while (i < ARRAY_LENGTH) {
        if (i < 42) {
            converted16 = ((uint16_t) rcv[i] << 8) | rcv[i + 1];
            #ifdef DEBUG

            printf("(%02d) %02d %3d = %d, %ld\n", i, rcv[i], rcv[i + 1], converted16, sizeof(converted16));
	        
            #endif
            arr[index++] = converted16;
            i += 2;
        } else if (i >= 42 && i < 58) {
            converted32 = 0;
	        for (int j = 0; j<4; j++) {
                #ifdef DEBUG

	            printf("(%d) %d ", i, rcv[i + j]);
	            
                #endif
                converted32 = (((uint32_t) converted32 << 8) | rcv[i + j]);
            }
            #ifdef DEBUG
	        
            printf(" = %d.3\n", converted32);
            
            #endif
            i += 4;
        } else {
	        break;
        }
    }

    memcpy(&apc, arr, 38);
    apc.aqi = rcv[58];
    apc.version = rcv[60];
    apc.errorCode = rcv[61];

    return true;

}

bool printValues(apcData *data) { // add HAL

    printf("PM1.0 Mass concentration: %u\n", data->pm1MC);
    printf("PM2.5 Mass concentration: %u\n", data->pm25MC);
    printf("PM10 Mass concentration: %u\n", data->pm10MC);
    printf("PM1.0 In air: %u\n", data->pm1Air);
    printf("PM2.5 In air: %u\n", data->pm25Air);
    printf("PM10 In air: %u\n", data->pm10Air);
    printf("# particles > 0.3nm: %u\n", data->particles03);
    printf("# particles > 0.5nm: %u\n", data->particles03 - data->particles05);
    printf("# particles > 1.0nm: %u\n", data->particles03 - data->particles1);
    printf("# particles > 2.5nm: %u\n", data->particles03 - data->particles25);
    printf("# particles > 5.0nm: %u\n", data->particles03 - data->particles50);
    printf("# particles > 10nm: %u\n", data->particles03 - data->particles10);
    printf("TVOC: %u\n", data->tvoc);
    printf("eCO2: %u\n", data->eCO2);
    printf("T-comp: %.2f\n", (double) data->tComp / 10);
    printf("RH-comp: %.2f\n", (double) data->rhComp / 10);
    printf("T-raw: %.2f\n", (double) data->tRaw / 10);
    printf("RH-raw: %.2f\n", (double) data->rhRaw / 10);
    printf("AQI: %u\n", data->aqi);
    printf("Firmware version: %u\n", data->version);
    printf("Error code: %u\n", data->errorCode);
    printf("\n");

    return true;

}

int main(int argc, char **args) {

    uint8_t getDataCmd[] = {0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71}; // command to be sent to receive the data
    uint8_t data[] = {66, 77, 0, 60, 0, 11,	0, 16, 0, 18, 0, 13, 0, 19, 0, 21, 10, 14, 2, 197, 0, 100, 0, 20, 0, 20, 0, 20, 0, 21, 1,
                      144, 0, 2, 0, 255, 3, 49, 1, 31, 2, 110, 0, 7, 210, 20, 0, 0,	0, 1, 0, 15, 33, 135, 0, 0, 202, 52, 1, 0, 34, 0,
                      7, 221}; // example received data
    STATE state = SETUP;
    
    while (1) {
        switch (state) {
            case SETUP:
                if (setupDevice()) {
                    state = GET_DATA;
                }
                break;
            case GET_DATA:
                if (sendCmd(getDataCmd)) {
                    state = PROCESS_DATA;
                }
                break;
            case PROCESS_DATA:
                if (getValues(data)) {
                    state = OUTPUT_DATA;
                } else {
                    state = GET_DATA;
                }
                break;
            case OUTPUT_DATA:
                if (printValues(&apc)) {
                    state = GET_DATA;
                    sleep(6); // only on Linux
                }
                break;
            default:
                return -1;
        }
    }

    return 0;

}