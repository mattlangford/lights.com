cc_library(
    name="abstract_light",
    srcs=[],
    hdrs=["abstract_light.hh"],
    deps=["//serial_control/"]
)

cc_library(
    name="litake_basic_light",
    srcs=["litake_basic_light.cc"],
    hdrs=["litake_basic_light.hh"],
    visibility = ["//visibility:public"],
    deps=[":abstract_light"]
)
