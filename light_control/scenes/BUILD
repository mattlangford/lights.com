# This is like a metagroup
cc_library(
    name="scenes",
    deps=[":submarine",
          ":aurora",
          ":sunset",
          ":aurora_circle",
          ":red_circle"],
    visibility=["//visibility:public"]
)

cc_library(
    name="submarine",
    srcs=["submarine.cc"],
    hdrs=["submarine.hh"],
    deps=["//config:channel_roles",
          "//light_control:scene_helpers",
          "//utils:scene_utilities",
          "//utils:universe_utilities"],
    alwayslink=True  # needed for static registry
)

cc_library(
    name="aurora",
    srcs=["aurora.cc"],
    hdrs=["aurora.hh"],
    deps=["//config:channel_roles",
          "//light_control:scene_helpers",
          "//utils:scene_utilities",
          "//utils:universe_utilities"],
    alwayslink=True  # needed for static registry
)

cc_library(
    name="sunset",
    srcs=["sunset.cc"],
    hdrs=["sunset.hh"],
    deps=["//config:channel_roles",
          "//light_control:scene_helpers",
          "//utils:scene_utilities",
          "//utils:universe_utilities"],
    alwayslink=True  # needed for static registry
)

cc_library(
    name="red_circle",
    srcs=["red_circle.cc"],
    hdrs=["red_circle.hh"],
    deps=["//config:channel_roles",
          "//light_control:scene_helpers",
          "//utils:scene_utilities",
          "//utils:universe_utilities"],
    alwayslink=True  # needed for static registry
)

cc_library(
    name="aurora_circle",
    srcs=["aurora_circle.cc"],
    hdrs=["aurora_circle.hh"],
    deps=["//config:channel_roles",
          "//light_control:scene_helpers",
          "//utils:scene_utilities",
          "//utils:universe_utilities"],
    alwayslink=True  # needed for static registry
)
