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
  this->storage_ = new NVSRollingCodeStorage(NVS_ROLLING_CODE_STORAGE, this->storage_key_);

  // Setup the Somfy Remote
  this->remote_ = new SomfyRemote(EMITTER_GPIO, this->remote_code_, this->storage_);

  // Attach to timebased cover controls
  automationTriggerUp_ = new Automation<>(this->get_open_trigger());
  actionTriggerUp = new SomfyCoverAction<>([=] { return this->open(); });
  automationTriggerUp_->add_action(actionTriggerUp);

  automationTriggerDown_ = new Automation<>(this->get_close_trigger());
  actionTriggerDown_ = new SomfyCoverAction<>([=] { return this->close(); });
  automationTriggerDown_->add_action(actionTriggerDown_);

  automationTriggerStop_ = new Automation<>(this->get_stop_trigger());
  actionTriggerStop_ = new SomfyCoverAction<>([=] { return this->stop(); });
  automationTriggerStop_->add_action(actionTriggerStop_);

  // Attach the prog button
  this->cover_prog_button_->add_on_press_callback([=] { return this->program(); });

  // Set extra settings
  this->has_built_in_endstop_ = true;
  this->assumed_state_ = true;

  TimeBasedCover::setup();
}

void SomfyCover::loop() { TimeBasedCover::loop(); }

void SomfyCover::dump_config() { ESP_LOGCONFIG(TAG, "Somfy cover"); }

cover::CoverTraits SomfyCover::get_traits() {
  auto traits = TimeBasedCover::get_traits();
  traits.set_supports_tilt(false);

  return traits;
}

void SomfyCover::control(const cover::CoverCall &call) { TimeBasedCover::control(call); }

void SomfyCover::open() {
  std::string command = "OPEN " + this->get_object_id();
  ESP_LOGI("somfy", command.c_str());
  this->sendCC1101Command(Command::Up);
}

void SomfyCover::close() {
  std::string command = "CLOSE " + this->get_object_id();
  ESP_LOGI("somfy", command.c_str());
  this->sendCC1101Command(Command::Down);
}

void SomfyCover::stop() {
  std::string command = "STOP " + this->get_object_id();
  ESP_LOGI("somfy", command.c_str());
  this->sendCC1101Command(Command::My);
}

void SomfyCover::program() {
  std::string command = "PROG " + this->get_object_id();
  ESP_LOGI("somfy", command.c_str());
  this->sendCC1101Command(Command::Prog);
}

void SomfyCover::sendCC1101Command(Command command) {
  if (ELECHOUSE_cc1101.getCC1101()) {  // Check the CC1101 Spi connection.
    ESP_LOGI("cc1101", "Connection OK");
  } else {
    ESP_LOGE("cc1101", "Connection Error");
  }
  ELECHOUSE_cc1101.SetTx();
  this->remote_->sendCommand(command);
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
