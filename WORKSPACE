new_http_archive(
    name="ftd2xx_libs",
    url="http://www.ftdichip.com/Drivers/D2XX/Linux/libftd2xx-arm-v6-hf-1.4.8.tgz",
    build_file="third_party/ftd2xx/ftd2xx.BUILD"
)
new_http_archive(
    name="ftd2xx_libs",
    url="http://www.ftdichip.com/Drivers/D2XX/Linux/libftd2xx-x86_64-1.4.6.tgz",
    build_file="third_party/ftd2xx/ftd2xx.BUILD"
)

git_repository(
    name = "variant",
    remote = "https://github.com/mattlangford/variant.git",
    commit = "d877154a2954230004d5b91c640372818c6efb5a"
)

# local_repository(
#     name = "server",
#     path = "/home/pi/server/"
# )

git_repository(
    name = "server",
    remote = "https://github.com/mattlangford/server.git",
    commit = "764697eb61c7f7284623643abde3af523da8118e"
)

#################################
# Needed for docker integration #
#################################
# TODO: This doesn't work on raspberry pi

# load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
# 
# http_archive(
#     name = "io_bazel_rules_docker",
#     sha256 = "6dede2c65ce86289969b907f343a1382d33c14fbce5e30dd17bb59bb55bb6593",
#     strip_prefix = "rules_docker-0.4.0",
#     urls = ["https://github.com/bazelbuild/rules_docker/archive/v0.4.0.tar.gz"],
# )
#
# load(
#     "@io_bazel_rules_docker//container:container.bzl",
#     "container_pull",
#     container_repositories = "repositories",
# )
# 
# # This is NOT needed when going through the language lang_image
# # "repositories" function(s).
# container_repositories()
# 
# container_pull(
#   name = "java_base",
#   registry = "gcr.io",
#   repository = "distroless/java",
#   # 'tag' is also supported, but digest is encouraged for reproducibility.
#   digest = "sha256:deadbeef",
# )
# 
# 
# load(
#     "@io_bazel_rules_docker//cc:image.bzl",
#     _cc_image_repos = "repositories",
# )
# 
# _cc_image_repos()
