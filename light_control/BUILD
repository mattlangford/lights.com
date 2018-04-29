cc_library(
    name="dmx",
    srcs=["dmx.cc"],
    hdrs=["dmx.hh"],
    deps=["//serial_control:ftd2xx_serial_interface"],
    visibility = ["//visibility:public"]
)

cc_library(
    name="light_effects",
    hdrs=["light_effects.hh"],
    visibility = ["//visibility:public"]
)

cc_library(
    name="light_universe_controller",
    srcs=["light_universe_controller.cc"],
    hdrs=["light_universe_controller.hh"],
    deps=["//light_types:abstract_light"],
    visibility = ["//visibility:public"]
)
