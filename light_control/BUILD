cc_library(
    name="dmx",
    srcs=["dmx.cc"],
    hdrs=["dmx.hh"],
    deps=["//serial_control:ftd2xx_serial_interface"],
    visibility = ["//visibility:public"]
)

cc_library(
    name="scheduling",
    srcs=["scheduling.cc"],
    hdrs=["scheduling.hh"],
    deps=["//light_control:dmx"],
    visibility = ["//visibility:public"]
)

cc_library(
    name="scene_helpers",
    srcs=["scene_helpers.cc"],
    hdrs=["scene_helpers.hh"],
    deps=["//config:universe",
          ":scheduling"],
    visibility = ["//visibility:public"]
)

cc_library(
    name="light_universe_controller",
    srcs=["light_universe_controller.cc"],
    hdrs=["light_universe_controller.hh"],
    deps=["//config:universe",
          "//light_control:dmx",
          "//serial_control:abstract_serial_interface",
          "//utils:universe_utilities",
          ":scheduling"],
    visibility = ["//visibility:public"]
)
