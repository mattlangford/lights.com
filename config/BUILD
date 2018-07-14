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
