#include <time.h>
#include <stdint.h>

#include "imudriver.h"
#include "bno055.h"
#include "i2c-cache.h"
#include "i2c-functions.h"

#define IMU_HEADING 0
#define IMU_ROLL 1
#define IMU_PITCH 2

static struct device_cache *cache;

static double headingOffset = 0.0;
static double pitchOffset = 0.0;
static double rollOffset = 0.0;

static void delay(int milliseconds) {
    struct timespec wait_time = {
        .tv_sec = 0,
        .tv_nsec = 1000000*milliseconds,
    };
    nanosleep(&wait_time, NULL);
}

void setMode(bno055_opmode_t mode) {
  I2Cwrite8(BNO055_ADDRESS, BNO055_OPR_MODE_ADDR, mode);
  delay(30);
}

int initIMU() {
    /* Make sure we have the right device */
    uint8_t id = I2Cread8(BNO055_ADDRESS, BNO055_CHIP_ID_ADDR);
    if(id != BNO055_ID) {
        return -1;  // no? ok bail
    }

    /* Switch to config mode (just in case since this is the default) */
    setMode(OPERATION_MODE_CONFIG);

    /* Reset */
    I2Cwrite8(BNO055_ADDRESS, BNO055_SYS_TRIGGER_ADDR, 0x20);
    while (((uint8_t) I2Cread8(BNO055_ADDRESS, BNO055_CHIP_ID_ADDR)) != BNO055_ID) {
        delay(10);
    }
    delay(50);

    /* Set to normal power mode */
    I2Cwrite8(BNO055_ADDRESS, BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
    delay(10);

    I2Cwrite8(BNO055_ADDRESS, BNO055_PAGE_ID_ADDR, 0);

    I2Cwrite8(BNO055_ADDRESS, BNO055_SYS_TRIGGER_ADDR, 0x0);
    delay(10);
    /* Set the  operating mode (see section 3.3) */
    setMode(OPERATION_MODE_IMUPLUS);
    delay(20);

    /* set up the cache system */
    cache = initCache(BNO055_ADDRESS, 0, 3, 0, 0);
    cache->r16_cmds[IMU_HEADING] = BNO055_EULER_H_LSB_ADDR;
    cache->r16_cmds[IMU_ROLL] = BNO055_EULER_R_LSB_ADDR;
    cache->r16_cmds[IMU_PITCH] = BNO055_EULER_P_LSB_ADDR;

    return 0;
}

double getHeading() {
    int val = c_read16(cache, IMU_HEADING);
    double result = val/16;
    result = result - headingOffset;
    if(result >= 360)
        result -= 360;
    if(result < 0)
        result += 360;
    return result;
}
void setHeading(double heading) {
    if(heading >= 0 && heading < 360) {
        headingOffset = 0;
        headingOffset = getHeading() - heading;
    }
}
double getPitch() {
    int val = c_read16(cache, IMU_PITCH);
    double result = val/16;
    result = result - pitchOffset;
    if(result >= 360)
        result -= 360;
    if(result < 0)
        result += 360;
    return result;
}
void setPitch(double pitch) {
    if(pitch >= 0 && pitch < 360) {
        pitchOffset = 0;
        pitchOffset = getPitch() - pitch;
    }
}
double getRoll() {
    int val = c_read16(cache, IMU_ROLL);
    double result = val/16;
    result = result - rollOffset;
    if(result >= 360)
        result -= 360;
    if(result < 0)
        result += 360;
    return result;
}
void setRoll(double roll) {
    if(roll >= 0 && roll < 360) {
        rollOffset = 0;
        rollOffset = getRoll() - roll;
    }
}
