#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "a_file.hh"

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  Tester t;
  t.test() = 32;
  vTaskDelete(NULL);
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}