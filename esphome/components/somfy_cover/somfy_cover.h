#pragma once

#include "esphome/core/component.h"
#include "esphome/components/time_based/time_based_cover.h"
#include "esphome/components/button/button.h"

// Libraries for CC1101 module and SomfyRemote
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <NVSRollingCodeStorage.h>
#include <SomfyRemote.h>

#define EMITTER_GPIO 2

// Set custom spi pins (byte sck, byte miso, byte mosi, byte ss)
#define SCK_PIN 14
#define MISO_PIN 39
#define MOSI_PIN 12
#define SS_PIN 15

#define CC1101_FREQUENCY 433.42

#define COVER_OPEN 1.0f
#define COVER_CLOSED 0.0f

#define NVS_ROLLING_CODE_STORAGE "somfy_cover"

namespace esphome {
namespace somfy_cover {

// Helper class to attach cover functions to the time based cover triggers
template<typename... Ts> class SomfyCoverAction : public Action<Ts...> {
 public:
  // The function to be called when the action plays.
  std::function<void(Ts...)> callback;

  explicit SomfyCoverAction(std::function<void(Ts...)> callback) : callback(callback) {}

  void play(Ts... x) override {
    if (callback)
      callback(x...);
  }
};

class SomfyCover : public time_based::TimeBasedCover {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  // Set time based cover values
  void set_open_duration(uint32_t open_duration) { this->open_duration_ = open_duration; }
  void set_close_duration(uint32_t close_duration) { this->close_duration_ = close_duration; }

  // Set somfy cover button and value
  void set_prog_button(button::Button *cover_prog_button) { this->cover_prog_button_ = cover_prog_button; }
  void set_cover_remote_code(u_int32_t cover_remote_code) { this->cover_remote_code_ = cover_remote_code; }

  cover::CoverTraits get_traits() override;

 protected:
  void control(const cover::CoverCall &call) override;

  // Set via the ESPHome yaml
  button::Button *cover_prog_button_{nullptr};
  uint32_t cover_remote_code_{0};

  // Set via the constructor
  SomfyRemote *remote;
  NVSRollingCodeStorage *storage;

  void open();
  void close();
  void stop();
  void program();

  // Create automations to attach the cover control functions
  Automation<> *automationTriggerUp_;
  SomfyCoverAction<> *actionTriggerUp;
  Automation<> *automationTriggerDown_;
  SomfyCoverAction<> *actionTriggerDown_;
  Automation<> *automationTriggerStop_;
  SomfyCoverAction<> *actionTriggerStop_;

  void sendCC1101Command(Command command);

  static void setupCC1101();
};

extern bool cc1101I_initialized;

}  // namespace somfy_cover
}  // namespace esphome
