import esphome.codegen as cg
from esphome.components import button, cover
import esphome.config_validation as cv
from esphome.const import (
    CONF_CLOSE_DURATION,
    CONF_ID,
    CONF_OPEN_DURATION,
    PLATFORM_ESP32,
    PLATFORM_ESP8266,
    PLATFORM_RP2040,
)

CODEOWNERS = ["@HarmEllis"]

AUTO_LOAD = ["button", "time_based"]

DEPENDENCIES = ["esp32"]

somfy_cover_ns = cg.esphome_ns.namespace("somfy_cover")
SomfyCover = somfy_cover_ns.class_("SomfyCover", cover.Cover, cg.Component)

CONF_PROG_BUTTON = "prog_button"
CONF_REMOTE_CODE = "remote_code"
CONF_SOMFY_STORAGE_KEY = "storage_key"

CONFIG_SCHEMA = cv.All(
    cover.COVER_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(SomfyCover),
            cv.Required(CONF_PROG_BUTTON): cv.use_id(button.Button),
            cv.Required(CONF_OPEN_DURATION): cv.positive_time_period_milliseconds,
            cv.Required(CONF_CLOSE_DURATION): cv.positive_time_period_milliseconds,
            cv.Required(CONF_REMOTE_CODE): cv.uint32_t,
            cv.Required(CONF_SOMFY_STORAGE_KEY): cv.All(cv.string, cv.Length(max=15)),
        }
    ).extend(cv.COMPONENT_SCHEMA),
    cv.only_on([PLATFORM_ESP32, PLATFORM_ESP8266, PLATFORM_RP2040]),
    cv.only_with_arduino,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add_library("SPI", None)
    cg.add_library("EEPROM", None)
    cg.add_library("SmartRC-CC1101-Driver-Lib", "2.5.7")
    cg.add_library("Somfy_Remote_Lib", "0.4.1")
    await cg.register_component(var, config)
    await cover.register_cover(var, config)

    btn = await cg.get_variable(config[CONF_PROG_BUTTON])
    cg.add(var.set_prog_button(btn))

    cg.add(var.set_open_duration(config[CONF_OPEN_DURATION]))

    cg.add(var.set_close_duration(config[CONF_CLOSE_DURATION]))

    cg.add(var.set_remote_code(config[CONF_REMOTE_CODE]))

    cg.add(var.set_storage_key(config[CONF_SOMFY_STORAGE_KEY]))
