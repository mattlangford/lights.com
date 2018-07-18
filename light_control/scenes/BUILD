# This is like a metagroup
cc_library(
    name="scenes",
    deps=[":submarine"],
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
