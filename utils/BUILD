cc_library(
    name = "universe_utilities",
    srcs = ["universe_utilities.cc"],
    hdrs = ["universe_utilities.hh"],
    deps = ["//config:universe",
            "//light_control:dmx"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "scene_utilities",
    srcs = ["scene_utilities.cc"],
    hdrs = ["scene_utilities.hh"],
    deps = ["//config:universe",
            "//light_control:dmx",
            "//light_control:scheduling",
            ":universe_utilities"],
    visibility = ["//visibility:public"],
)
