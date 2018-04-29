cc_library(
    name="abstract_serial_interface",
    srcs=[],
    hdrs=["abstract_serial_interface.hh"],
    deps=[]
)

cc_library(
    name="ftd2xx_serial_interface",
    srcs=["ftd2xx_serial_interface.cc"],
    hdrs=["ftd2xx_serial_interface.hh"],
    deps=[":abstract_serial_interface",
          "@ftd2xx_libs//:ftd2xx"],
)
