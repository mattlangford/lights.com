#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "runner.hh"

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  vTaskDelete(NULL);
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}