load("@io_bazel_rules_docker//cc:image.bzl", "cc_image")

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = ["//config:universe_generators",
            "//light_control:light_universe_controller",
            "//serial_control:ftd2xx_serial_interface",
            "//server_hooks:universe_resource",
            "@server//http_server:http_server",
            "@server//resources:file_resource"]
)

cc_image(
    name = "docker_main",
    srcs = ["main.cc"],
    deps = ["//config:universe_generators",
            "//light_control:light_universe_controller",
            "//serial_control:ftd2xx_serial_interface",
            "//server_hooks:universe_resource",
            "@server//http_server:http_server",
            "@server//resources:file_resource"]
)
