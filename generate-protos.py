import os
import subprocess

# Set the directory containing .proto files
PROTO_DIR = "lib/config"

def generate_protos():
    print("Generating .pb.h and .pb.c files from .proto files...")

    # Find the Nanopb plugin path
    nanopb_plugin = subprocess.run(
        ["python3", "-c", "import nanopb; print(nanopb.__path__[0])"], 
        capture_output=True, text=True
    ).stdout.strip()

    # Find all .proto files
    proto_files = [os.path.join(root, f) for root, _, files in os.walk(PROTO_DIR) for f in files if f.endswith(".proto")]

    if proto_files:
        # Run protoc command
        protoc_cmd = [
            "protoc",
            f"-I{PROTO_DIR}", # Make includes relative to the root folder
            f"-I{nanopb_plugin + '/generator/proto'}", # Allow nanopb specific includes
            f"--plugin=protoc-gen-nanopb={nanopb_plugin + '/generator/protoc-gen-nanopb'}",
            f"--nanopb_out={PROTO_DIR}"
        ] + proto_files

        subprocess.run(protoc_cmd, check=True)
        print("Proto generation complete.")
    else:
        print("No .proto files found.")

generate_protos()
