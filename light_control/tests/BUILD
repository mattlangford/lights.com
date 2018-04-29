cc_test(
   name = "light_effect_tests",
   srcs = ["light_effect_tests.cc"],
   deps = ["//light_control:light_effects"],
)

cc_binary(
   name = "test_light",
   srcs = ["test_light.cc"],
   deps = ["//light_control:light_universe_controller",
           "//light_control:dmx",
           "//light_types:litake_basic_light"],
)
