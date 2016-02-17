#include <robotdriver/toolboxdriver.h>
#include <robotdriver/motioncontroller.h>
#include <stdio.h>
#include <time.h>
#include <wiringPi.h>

/* In this example, we'll demonstrate how to use toolbox driver */

void delayMilli(int milliseconds) {
    struct timespec wait_time = {
        .tv_sec = 0,
        .tv_nsec = 1000000
    };
    for(int i=0; i<milliseconds; i++)
    	nanosleep(&wait_time, NULL);
}
static void collisionsCallback() {
    printf("coll3:%d, coll4:%d, coll5: %d\n", getCollisionDetector(3), getCollisionDetector(4), getCollisionDetector(5));
}
static void sensorsCallback() {
    printf("sensor1:%d, sensor2:%d, sensor3: %d, sensor4:%d, sensor5: %d\n", getSensor(1), getSensor(2), getSensor(3), getSensor(4), getSensor(5));
}

int main() {
    initToolboxDriver();
    initMotionController();

    setLED(1, 1);
    setLED(2, 0);
    setLED(3, 1);

    enableSensorCallback(1);
    disableSensorCallback(2);
    disableSensorCallback(3);
    disableSensorCallback(4);
    disableSensorCallback(5);
    setSensorsCallback(sensorsCallback);

    disableCollisionCallback(1);
    disableCollisionCallback(2);
    enableCollisionCallback(3);
    enableCollisionCallback(4);
    enableCollisionCallback(5);
    setCollisionsCallback(collisionsCallback);

	setAxActiveWheel(124);
	setAxSpeed(200);

    while(1) {
	/*delayMilli(1000);
	printf("%d\n", getAxPosition());
	setAxPosition(0);
	delayMilli(1000);
	printf("%d\n", getAxPosition());
	setAxPosition(100);*/
    }
    return 0;
}