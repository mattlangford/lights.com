genrule(
    name="move_from_release",
    srcs=["release/build/libftd2xx.a",
          "release/ftd2xx.h",
          "release/WinTypes.h"
    ],
    outs=["libftd2xx.a",
          "ftd2xx.h",
          "WinTypes.h"
    ],
    cmd = "cp $(location release/build/libftd2xx.a) $(location libftd2xx.a);\
           cp $(location release/ftd2xx.h) $(location ftd2xx.h);\
           cp $(location release/WinTypes.h) $(location WinTypes.h);"
)

cc_library(
    name = "ftd2xx",
    srcs = ["libftd2xx.a"],
    hdrs = ["ftd2xx.h", "WinTypes.h"],
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"],
)
