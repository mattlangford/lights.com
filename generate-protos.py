import os
import subprocess

# Set the directory containing .proto files
PROTO_DIR = "lib/config"

def generate_protos():
    print("Generating .pb.h and .pb.c files from .proto files...")

    subprocess.run([f"rm -f {PROTO_DIR}/*pb.*"], shell=True, check=True)

    # Find all .proto files
    proto_files = [os.path.join(root, f) for root, _, files in os.walk(PROTO_DIR) for f in files if f.endswith(".proto")]

    # Define the protoc command
    command = [
        "protoc",
        "--plugin=protoc-gen-eams=lib/EmbeddedProto/protoc-gen-eams",
        f"-I{PROTO_DIR}",
        f"--eams_out={PROTO_DIR}",
    ] + proto_files

    subprocess.run(command, check=True)

generate_protos()
