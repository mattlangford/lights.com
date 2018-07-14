cc_library(
    name = "channel_roles",
    hdrs = ["channel_roles.hh"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "channel",
    hdrs = ["channel.hh"],
    deps = [":channel_roles"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "light",
    hdrs = ["light.hh"],
    deps = [":channel"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "universe",
    hdrs = ["universe.hh"],
    deps = [":light"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "light_generators",
    srcs = ["light_generators.cc"],
    hdrs = ["light_generators.hh"],
    deps = [":channel_roles",
            ":channel",
            ":light"],
    visibility = ["//visibility:public"]
)
