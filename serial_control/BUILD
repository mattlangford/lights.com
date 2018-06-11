cc_library(
    name="abstract_serial_interface",
    hdrs=["abstract_serial_interface.hh"],
    visibility = ["//visibility:public"]
)

cc_library(
    name="ftd2xx_serial_interface",
    srcs=["ftd2xx_serial_interface.cc"],
    hdrs=["ftd2xx_serial_interface.hh"],
    deps=[":abstract_serial_interface",
          "@ftd2xx_libs//:ftd2xx"],
    visibility = ["//visibility:public"]
)

cc_library(
    name="virtual_serial_interface",
    hdrs=["virtual_serial_interface.hh"],
    deps=[":abstract_serial_interface"],
    visibility = ["//visibility:public"]
)
