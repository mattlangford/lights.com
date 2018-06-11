cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = ["//light_control:light_universe_controller",
            "//light_types:virtual_light",
            "//serial_control:virtual_serial_interface",
            "//server_hooks:universe_resource",
            "@server//http_server:http_server",
            "@server//resources:file_resource"]
)
