cc_library(
    name = "universe_resource",
    srcs = ["universe_resource.cc"],
    hdrs = ["universe_resource.hh"],
    deps = ["//config:universe",
            "//light_control:light_universe_controller",
            "//light_control/scenes",
            "//light_control:scene_helpers",
            "//utils:universe_utilities",
            "@server//json:json",
            "@server//resources:json_resource",
            "@server//:logging"],
    visibility = ["//visibility:public"]
)
