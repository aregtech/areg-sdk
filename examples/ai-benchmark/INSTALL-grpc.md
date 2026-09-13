# Installing gRPC for the comparison arm

Do this once, outside any measured run. The agent never installs anything itself: an
install inside a run is billed as framework cost and makes the two arms incomparable.
`run-benchmark.sh --framework grpc` refuses to start while `protoc` or
`grpc_cpp_plugin` is missing.

## Distribution packages

On Debian and Ubuntu:

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config \
                    libgrpc++-dev protobuf-compiler-grpc libprotobuf-dev
```

On any other platform, install gRPC and protobuf with its package manager, choosing
the package that ships the CMake config files. Then check:

```bash
protoc --version
grpc_cpp_plugin --version
```

## Confirm that CMake finds it

A project that cannot configure turns the run into a build-system exercise and
measures nothing, so probe it first:

```bash
mkdir -p grpcprobe && cd grpcprobe
cat > CMakeLists.txt <<'PROBE'
cmake_minimum_required(VERSION 3.20)
project(probe CXX)
find_package(Protobuf CONFIG)
find_package(gRPC CONFIG)
message(NOTICE "Protobuf_FOUND=${Protobuf_FOUND} gRPC_FOUND=${gRPC_FOUND}")
PROBE
cmake -S . -B b
```

If `gRPC_FOUND` is empty, the packages ship pkg-config files but no CMake config
package. Either build from source as below, which always installs one, or record in
the run's notes that the arm was told to use `pkg-config`: it changes what the agent
has to do.

## From source, when the version has to be pinned

```bash
git clone --recurse-submodules -b v1.62.0 --depth 1 --shallow-submodules \
    https://github.com/grpc/grpc grpc-src
cmake -S grpc-src -B grpc-src/build -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX="$HOME/opt/grpc" \
      -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF \
      -DgRPC_SSL_PROVIDER=package -DgRPC_ZLIB_PROVIDER=package
cmake --build grpc-src/build -j8
cmake --install grpc-src/build
```

Then set `CMAKE_PREFIX_PATH` to the install prefix in the shell that starts the run.

## What must not be done

- **Never `FetchContent` gRPC inside the measured project.** It builds gRPC during the
  agent's own build, adding tens of minutes and thousands of lines of unrelated compiler
  output to the agent's context. The areg arm links a framework it builds once; the
  gRPC arm links an installed one.
- **Never let the agent run a package manager.** If a header is missing the run is
  void: fix the machine and start a fresh run.
