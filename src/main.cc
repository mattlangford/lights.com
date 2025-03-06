#include <Arduino.h>

#include "runner.hh"
#include "universe.hh"
#include "lights.hh"

std::shared_ptr<light::Universe> universe = nullptr;
runner::Runner* active_runner = nullptr;

void setup() {
  config::UniverseNode config;
  config.lights.push_back({.name = "misseye1", .channels=lights::misseye(1)});
  config.lights.push_back({.name = "misseye2", .channels=lights::misseye(100)});

  universe = std::make_shared<light::Universe>(config);

  active_runner = new runner::Runner();
}

void loop() { }