cc_library(
    name="abstract_light",
    hdrs=["abstract_light.hh"],
    deps=["//light_control:dmx"],
    visibility = ["//visibility:public"]
)

cc_library(
    name="litake_basic_light",
    srcs=["litake_basic_light.cc"],
    hdrs=["litake_basic_light.hh"],
    deps=[":abstract_light"],
    visibility = ["//visibility:public"]
)
