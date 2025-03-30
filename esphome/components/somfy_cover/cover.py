import esphome.codegen as cg
from esphome.components import button, cover
import esphome.config_validation as cv
from esphome.const import CONF_CLOSE_DURATION, CONF_ID, CONF_OPEN_DURATION

CODEOWNERS = ["@HarmEllis"]

AUTO_LOAD = ["time_based", "button"]

somfy_cover_ns = cg.esphome_ns.namespace("somfy_cover")
SomfyCover = somfy_cover_ns.class_("SomfyCover", cover.Cover, cg.Component)

CONF_PROG_BUTTON = "cover_prog_button"
CONF_REMOTE_CODE = "cover_remote_code"

CONFIG_SCHEMA = cover.COVER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(SomfyCover),
        cv.Required(CONF_PROG_BUTTON): button.button_schema(class_=button.Button),
        cv.Required(CONF_OPEN_DURATION): cv.positive_time_period_milliseconds,
        cv.Required(CONF_CLOSE_DURATION): cv.positive_time_period_milliseconds,
        cv.Required(CONF_REMOTE_CODE): cv.uint32_t,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cover.register_cover(var, config)

    btn = await button.new_button(config[CONF_PROG_BUTTON])
    cg.add(var.set_prog_button(btn))

    cg.add(var.set_open_duration(config[CONF_OPEN_DURATION]))

    cg.add(var.set_close_duration(config[CONF_CLOSE_DURATION]))

    cg.add(var.set_cover_remote_code(config[CONF_REMOTE_CODE]))
