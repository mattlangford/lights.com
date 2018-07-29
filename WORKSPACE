new_http_archive(
    name="ftd2xx_libs",
    url="http://www.ftdichip.com/Drivers/D2XX/Linux/libftd2xx-arm-v6-hf-1.4.8.tgz",
    build_file="third_party/ftd2xx/ftd2xx.BUILD"
)

git_repository(
    name = "variant",
    remote = "https://github.com/mattlangford/variant.git",
    commit = "a88daadb9c45b7304bcb4f4d422e669e83ce387e"
)

# TEMP TEST REPO
# local_repository(
#     name = "server",
#     path = "/home/matt/Documents/server/"
# )

git_repository(
    name = "server",
    remote = "https://github.com/mattlangford/server.git",
    commit = "b5a8f95cbe21c7789874519e5e98156a4c54837b"
)

#################################
# Needed for docker integration #
#################################

# load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
# 
# http_archive(
#     name = "io_bazel_rules_docker",
#     sha256 = "6dede2c65ce86289969b907f343a1382d33c14fbce5e30dd17bb59bb55bb6593",
#     strip_prefix = "rules_docker-0.4.0",
#     urls = ["https://github.com/bazelbuild/rules_docker/archive/v0.4.0.tar.gz"],
# )
# 
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
