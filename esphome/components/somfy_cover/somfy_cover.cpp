#include "esphome/core/log.h"
#include "somfy_cover.h"

namespace esphome {
namespace somfy_cover {

static const char *TAG = "somfy_cover.cover";

// Default value
bool cc1101I_initialized = false;

void SomfyCover::setup() {
  // Setup the cc1101 module
  setupCC1101();

  // Setup cover rolling code storage
  this->storage = new NVSRollingCodeStorage(NVS_ROLLING_CODE_STORAGE, this->get_object_id().c_str());

  // Setup the Somfy Remote
  this->remote = new SomfyRemote(EMITTER_GPIO, this->cover_remote_code_, this->storage);

  // Attach to timebased cover controls
  automationTriggerUp_ = new Automation<>(this->get_open_trigger());
  actionTriggerUp = new SomfyCoverAction<>([=]() { this->open(); });
  automationTriggerUp_->add_action(actionTriggerUp);
  this->open_trigger_->set_automation_parent(automationTriggerUp_);

  automationTriggerDown_ = new Automation<>(this->get_close_trigger());
  actionTriggerDown_ = new SomfyCoverAction<>([=]() -> void { this->close(); });
  automationTriggerDown_->add_action(actionTriggerDown_);
  this->close_trigger_->set_automation_parent(automationTriggerDown_);

  automationTriggerStop_ = new Automation<>(this->get_stop_trigger());
  actionTriggerStop_ = new SomfyCoverAction<>([=]() -> void { this->stop(); });
  automationTriggerStop_->add_action(actionTriggerStop_);
  this->stop_trigger_->set_automation_parent(automationTriggerStop_);

  // Attach the prog button
  this->cover_prog_button_->add_on_press_callback([=]() { this->program(); });

  // Set extra settings
  this->has_built_in_endstop_ = true;

  TimeBasedCover::setup();
}

void SomfyCover::loop() { TimeBasedCover::loop(); }

void SomfyCover::dump_config() { ESP_LOGCONFIG(TAG, "Somfy cover"); }

cover::CoverTraits SomfyCover::get_traits() {
  auto traits = cover::CoverTraits();
  traits.set_is_assumed_state(true);
  traits.set_supports_position(true);
  traits.set_supports_tilt(false);
  traits.set_supports_stop(true);

  return traits;
}

void SomfyCover::control(const cover::CoverCall &call) { TimeBasedCover::control(call); }

void SomfyCover::open() {
  ESP_LOGI("somfy", "OPEN", this->get_object_id());
  this->sendCC1101Command(Command::Up);
}

void SomfyCover::close() {
  ESP_LOGI("somfy", "CLOSE", this->get_object_id());
  this->sendCC1101Command(Command::Down);
}

void SomfyCover::stop() {
  ESP_LOGI("somfy", "STOP", this->get_object_id());
  this->sendCC1101Command(Command::My);
}

void SomfyCover::program() {
  ESP_LOGI("somfy", "PROG", this->get_object_id());
  this->sendCC1101Command(Command::Prog);
}

void SomfyCover::sendCC1101Command(Command command) {
  if (ELECHOUSE_cc1101.getCC1101()) {  // Check the CC1101 Spi connection.
    ESP_LOGI("cc1101", "Connection OK");
  } else {
    ESP_LOGE("cc1101", "Connection Error");
  }
  ELECHOUSE_cc1101.SetTx();
  remote->sendCommand(command);
  ELECHOUSE_cc1101.setSidle();
}

void SomfyCover::setupCC1101() {
  // The cc1101 library does not need to be setup more than once
  if (cc1101I_initialized) {
    return;
  }

  // Set EMITTER_GPIO as OUTPUT, otherwise no commands will be sent
  pinMode(EMITTER_GPIO, OUTPUT);
  digitalWrite(EMITTER_GPIO, LOW);

  // Setup SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN
  pinMode(SCK_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(SS_PIN, OUTPUT);

  // Set custom spi pins (byte sck, byte miso, byte mosi, byte ss)
  ELECHOUSE_cc1101.setSpiPin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

  ELECHOUSE_cc1101.Init();

  ELECHOUSE_cc1101.setMHZ(CC1101_FREQUENCY);

  if (ELECHOUSE_cc1101.getCC1101()) {  // Check the CC1101 Spi connection.
    ESP_LOGI("cc1101", "Connection OK");
    cc1101I_initialized = true;
  } else {
    ESP_LOGE("cc1101", "Connection Error");
  }
}

}  // namespace somfy_cover
}  // namespace esphome
