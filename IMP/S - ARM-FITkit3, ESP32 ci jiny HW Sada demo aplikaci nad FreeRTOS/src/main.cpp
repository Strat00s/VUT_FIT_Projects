/**
 * @file main.cpp
 * @author Lukáš Baštýř (l.bastyr@seznam.cz)
 * 
 */

#include <Arduino.h>
#include <FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#define BUTTON (23)

QueueHandle_t queue;    //queue for sending data between tasks

//led blink task
void ledTask(void *params) {
    int cnt = 0;

    while(true) {
        //read from queue when something is waiting
        if (uxQueueMessagesWaiting(queue))
            xQueueReceive(queue, &cnt, 0);

        //blink every second while waiting for button to be depressed
        if (cnt < 0) {
            digitalWrite(LED_BUILTIN, HIGH);
            vTaskDelay(500);
            digitalWrite(LED_BUILTIN, LOW);
            vTaskDelay(500);
        }

        //blink every half second on start
        else if (cnt == 0) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            vTaskDelay(250);
        }

        //blink as many times as for how long the button was pressed
        else {
            for (int i = 0; i < cnt*2; i ++) {
                digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
                vTaskDelay(100);
            }
            vTaskDelay(1000);
        }
    }
}

//helper function to add data to queue
void addToQueue(int data) {
    xQueueSend(queue, &data, 0);
}

//button task
void buttonTask(void *params) {
    int cnt = 0;

    while(true) {
        //on button press
        if (!digitalRead(BUTTON)) {
            addToQueue(-1); //send -1 to start blinking every second while button is pressed

            //count up every second while the button is pressed
            while(!digitalRead(BUTTON)) {
                cnt++;
                vTaskDelay(1000);
            }

            addToQueue(cnt);    //send count to queue
            cnt = 0;            //reset queue
        }

        vTaskDelay(1);  //task is alive for watchdog
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);   //use builtin led as output
    pinMode(BUTTON, INPUT_PULLUP);  //use button on pin 23 as input

    queue = xQueueCreate(1, sizeof(int));   //create queue with size of 1 int

    //create tasks
    xTaskCreate(ledTask, "led_task", 100000, NULL, 1, NULL);
    xTaskCreate(buttonTask, "button_task", 100000, NULL, 1, NULL);
}

//nothing to do here
void loop() {
}