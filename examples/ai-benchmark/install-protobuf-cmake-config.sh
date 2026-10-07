#!/usr/bin/env bash
# ===========================================================================
# Installs the CMake config package that Debian and Ubuntu leave out of
# libprotobuf-dev, so that find_package(Protobuf CONFIG) works beside the
# gRPCConfig.cmake those distributions do ship.
#
# The package forwards to CMake's own FindProtobuf module and the system
# protobuf, so no second protobuf is installed and nothing is compiled.
#
#   examples/ai-benchmark/install-protobuf-cmake-config.sh              # /usr/local, uses sudo
#   examples/ai-benchmark/install-protobuf-cmake-config.sh --user       # ~/.local, no sudo
#   examples/ai-benchmark/install-protobuf-cmake-config.sh --prefix DIR
#   examples/ai-benchmark/install-protobuf-cmake-config.sh --uninstall [--user | --prefix DIR]
#
# Exit code 0 when the check at the end finds Protobuf and gRPC in CONFIG mode.
# ===========================================================================
set -euo pipefail

PREFIX="/usr/local"
UNINSTALL=""
while [ $# -gt 0 ]; do
    case "$1" in
        --user)      PREFIX="${HOME}/.local"; shift ;;
        --prefix)    PREFIX="$2"; shift 2 ;;
        --uninstall) UNINSTALL=1; shift ;;
        -h|--help)   sed -n '2,16p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "unknown option: $1" >&2; exit 2 ;;
    esac
done

DEST="${PREFIX}/lib/cmake/protobuf"
SUDO=""
if ! mkdir -p "${DEST}" 2>/dev/null || [ ! -w "${DEST}" ]; then
    SUDO="sudo"
fi

if [ -n "${UNINSTALL}" ]; then
    ${SUDO} rm -f "${DEST}/protobuf-config.cmake" "${DEST}/protobuf-config-version.cmake"
    ${SUDO} rmdir "${DEST}" 2>/dev/null || true
    echo "removed ${DEST}"
    exit 0
fi

command -v protoc >/dev/null || { echo "protoc not found: install protobuf-compiler" >&2; exit 2; }
command -v cmake  >/dev/null || { echo "cmake not found" >&2; exit 2; }
VERSION="$(protoc --version | awk '{print $2}')"
[ -n "${VERSION}" ] || { echo "cannot read the protoc version" >&2; exit 2; }

TMP="$(mktemp -d)"
trap 'rm -rf "${TMP}"' EXIT

cat > "${TMP}/protobuf-config.cmake" <<'EOF'
# CMake config package for the distribution's protobuf, which ships none.
# It forwards to CMake's FindProtobuf module, which defines protobuf::libprotobuf,
# protobuf::libprotoc, protobuf::protoc and protobuf_generate().
include(CMakeFindDependencyMacro)
set(_protobuf_saved_module_path "${CMAKE_MODULE_PATH}")
set(CMAKE_MODULE_PATH "")
find_package(Protobuf MODULE QUIET)
set(CMAKE_MODULE_PATH "${_protobuf_saved_module_path}")
unset(_protobuf_saved_module_path)
if(NOT Protobuf_FOUND)
    set(Protobuf_FOUND FALSE)
    set(Protobuf_NOT_FOUND_MESSAGE "the protobuf headers, library or protoc were not found")
    return()
endif()
set(protobuf_FOUND TRUE)
if(NOT DEFINED Protobuf_PROTOC_EXECUTABLE AND TARGET protobuf::protoc)
    get_target_property(Protobuf_PROTOC_EXECUTABLE protobuf::protoc IMPORTED_LOCATION)
endif()
EOF

cat > "${TMP}/protobuf-config-version.cmake" <<EOF
set(PACKAGE_VERSION "${VERSION}")
if(PACKAGE_FIND_VERSION VERSION_GREATER PACKAGE_VERSION)
    set(PACKAGE_VERSION_COMPATIBLE FALSE)
else()
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
    if(PACKAGE_FIND_VERSION STREQUAL PACKAGE_VERSION)
        set(PACKAGE_VERSION_EXACT TRUE)
    endif()
endif()
EOF

${SUDO} mkdir -p "${DEST}"
${SUDO} install -m 0644 "${TMP}/protobuf-config.cmake" "${TMP}/protobuf-config-version.cmake" "${DEST}/"
echo "installed ${DEST}/protobuf-config.cmake (protobuf ${VERSION})"

# The check of INSTALL-grpc.md, with the prefix named so that a --prefix outside the
# default search path is checked too.
mkdir -p "${TMP}/check"
cat > "${TMP}/check/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.20)
project(probe CXX)
find_package(Protobuf CONFIG)
find_package(gRPC CONFIG)
message(NOTICE "Protobuf_FOUND=${Protobuf_FOUND} gRPC_FOUND=${gRPC_FOUND}")
EOF
OUT="$(cmake -S "${TMP}/check" -B "${TMP}/check/build" -DCMAKE_PREFIX_PATH="${PREFIX}" 2>&1 || true)"
echo "${OUT}" | grep -E "Protobuf_FOUND=" || { echo "${OUT}" | tail -20; exit 1; }
echo "${OUT}" | grep -q "Protobuf_FOUND=1 gRPC_FOUND=1" || exit 1
case ":${PATH}:" in
    *":${PREFIX}/bin:"*|*":${PREFIX}/sbin:"*) ;;
    *) [ "${PREFIX}" = "/usr/local" ] || echo "note: ${PREFIX}/bin is not on PATH, so a project needs -DCMAKE_PREFIX_PATH=${PREFIX} to find it" ;;
esac
