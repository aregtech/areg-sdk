#!/bin/sh
# =============================================================================
#  setup-environment.sh
#
#  Installs the tools that build Areg on Linux and macOS: a C++ compiler,
#  CMake, Java, Git, and optionally Python, ncurses and cross compilers.
#  Plain POSIX sh. Shows a menu, prints the exact commands, asks before it runs
#  them, then runs tools/check-env.sh.
#
#  Usage:  tools/setup-environment.sh [--all] [--with LIST] [--yes] [--dry-run]
#  Exit:   0 = installed and checked, 1 = bad argument, refused or failed,
#          2 = no supported package manager.
# =============================================================================

set -eu

HERE=$(cd "$(dirname "$0")" && pwd)

# Menu items: id and label. The defaults are the required tools.
ITEMS="gcc clang cmake java git python ncurses x86 arm32 arm64"
SELECTED="gcc cmake java git"
DRY_RUN=0
ASSUME_YES=0

fail() {
    echo "error: $1" >&2
    exit "${2:-1}"
}

usage() {
    cat <<'EOF'
usage: setup-environment.sh [--all] [--with LIST] [--yes] [--dry-run]

Install the tools that build Areg.

  --all          select every item of the menu
  --with LIST    select these items, comma separated:
                 gcc, clang, cmake, java, git, python, ncurses, x86, arm32, arm64
  --yes          run the commands without asking
  --dry-run      print the commands, run nothing
  -h, --help     show this help

Without --all or --with, a menu asks on a terminal. Without a terminal, the
required tools are selected: gcc, cmake, java, git.
EOF
}

label() {
    case "$1" in
        gcc)     echo "C++ compiler: GCC" ;;
        clang)   echo "C++ compiler: Clang" ;;
        cmake)   echo "CMake 3.20+" ;;
        java)    echo "Java 17+ (runs the code generator)" ;;
        git)     echo "Git" ;;
        python)  echo "Python 3.10+ (agentic coding only)" ;;
        ncurses) echo "ncurses (only for AREG_EXTENDED=ON)" ;;
        x86)     echo "Cross: Linux x86, 32-bit" ;;
        arm32)   echo "Cross: Linux ARMv7, 32-bit" ;;
        arm64)   echo "Cross: Linux ARM64" ;;
    esac
}

is_selected() {
    case " $SELECTED " in *" $1 "*) return 0 ;; esac
    return 1
}

toggle() {
    if is_selected "$1"; then
        SELECTED=$(echo " $SELECTED " | sed "s/ $1 / /" | sed 's/^ *//;s/ *$//')
    else
        SELECTED="$SELECTED $1"
    fi
}

while [ $# -gt 0 ]; do
    case "$1" in
        --all)     SELECTED="$ITEMS"; NO_MENU=1 ;;
        --with)    [ $# -ge 2 ] || fail "--with needs a list"
                   SELECTED=$(echo "$2" | tr ',' ' '); NO_MENU=1; shift ;;
        --yes)     ASSUME_YES=1 ;;
        --dry-run) DRY_RUN=1 ;;
        -h|--help) usage; exit 0 ;;
        *)         usage >&2; fail "unknown argument: $1" ;;
    esac
    shift
done

for id in $SELECTED; do
    case " $ITEMS " in *" $id "*) ;; *) fail "unknown item: $id" ;; esac
done

# --- Package manager ---------------------------------------------------------
OS=$(uname -s)
PM=""
if [ "$OS" = "Darwin" ]; then
    command -v brew >/dev/null 2>&1 && PM="brew"
    [ -n "$PM" ] || fail "Homebrew is missing. Install it from https://brew.sh/ and run this again." 2
elif command -v apt-get >/dev/null 2>&1; then PM="apt"
elif command -v dnf     >/dev/null 2>&1; then PM="dnf"
elif command -v pacman  >/dev/null 2>&1; then PM="pacman"
else
    fail "no supported package manager (apt, dnf, pacman, brew). Install the tools by hand: docs/wiki/01e-build-environment.md" 2
fi

SUDO=""
if [ "$PM" != "brew" ] && [ "$(id -u)" -ne 0 ]; then
    SUDO="sudo "
fi

# The newest Java package the distribution offers, 17 or newer.
apt_java() {
    for v in 25 21 17; do
        if apt-cache show "openjdk-$v-jre-headless" >/dev/null 2>&1; then
            echo "openjdk-$v-jre-headless"; return
        fi
    done
    echo "default-jre-headless"
}

dnf_java() {
    for v in 25 21 17; do
        if dnf -q info "java-$v-openjdk-headless" >/dev/null 2>&1; then
            echo "java-$v-openjdk-headless"; return
        fi
    done
    echo "java-latest-openjdk-headless"
}

# The versioned multilib package of the default g++. The gcc-multilib and g++-multilib
# meta packages conflict with the ARM cross compilers; the versioned one does not.
apt_multilib() {
    pkg=$(apt-cache depends g++-multilib 2>/dev/null | sed -n 's/.*Depends: \(g++-[0-9]*-multilib\).*/\1/p' | head -n 1)
    echo "${pkg:-g++-multilib}"
}

# Package names of one item for the detected package manager; empty if none.
packages() {
    case "$PM:$1" in
        apt:gcc)       echo "build-essential" ;;
        apt:clang)     echo "clang" ;;
        apt:cmake)     echo "cmake" ;;
        apt:java)      apt_java ;;
        apt:git)       echo "git" ;;
        apt:python)    echo "python3" ;;
        apt:ncurses)   echo "libncurses-dev" ;;
        apt:x86)       apt_multilib ;;
        apt:arm32)     echo "g++-arm-linux-gnueabihf" ;;
        apt:arm64)     echo "g++-aarch64-linux-gnu" ;;
        dnf:gcc)       echo "gcc-c++ make" ;;
        dnf:clang)     echo "clang" ;;
        dnf:cmake)     echo "cmake" ;;
        dnf:java)      dnf_java ;;
        dnf:git)       echo "git" ;;
        dnf:python)    echo "python3" ;;
        dnf:ncurses)   echo "ncurses-devel" ;;
        pacman:gcc)    echo "base-devel" ;;
        pacman:clang)  echo "clang" ;;
        pacman:cmake)  echo "cmake" ;;
        pacman:java)   echo "jre-openjdk-headless" ;;
        pacman:git)    echo "git" ;;
        pacman:python) echo "python" ;;
        pacman:ncurses) echo "ncurses" ;;
        brew:cmake)    echo "cmake" ;;
        brew:java)     echo "openjdk" ;;
        brew:git)      echo "git" ;;
        brew:python)   echo "python" ;;
    esac
}

# --- Menu --------------------------------------------------------------------
if [ -z "${NO_MENU:-}" ] && [ -t 0 ]; then
    while :; do
        echo ""
        echo "Areg build environment setup ($OS, $PM)"
        echo ""
        n=0
        for id in $ITEMS; do
            n=$((n + 1))
            if is_selected "$id"; then mark="x"; else mark=" "; fi
            printf "  %2d) [%s] %s\n" "$n" "$mark" "$(label "$id")"
        done
        echo ""
        printf "Numbers to toggle (e.g. 2 6), Enter to continue, q to quit: "
        read -r answer || exit 1
        [ "$answer" = "q" ] && exit 1
        [ -z "$answer" ] && break
        for num in $answer; do
            n=0
            for id in $ITEMS; do
                n=$((n + 1))
                [ "$num" = "$n" ] && toggle "$id"
            done
        done
    done
fi

# --- Commands ----------------------------------------------------------------
PKGS=""
NOTES=""
XCODE=0
for id in $SELECTED; do
    if [ "$PM" = "brew" ] && { [ "$id" = "gcc" ] || [ "$id" = "clang" ]; }; then
        XCODE=1
        continue
    fi
    p=$(packages "$id")
    if [ -n "$p" ]; then
        PKGS="$PKGS $p"
    elif [ "$PM" = "brew" ] && [ "$id" = "ncurses" ]; then
        :
    else
        NOTES="$NOTES
  - $(label "$id"): not offered for $PM here; see docs/wiki/01e-build-environment.md"
    fi
done

CMDS=""
add_cmd() {
    CMDS="$CMDS
$1"
}

[ "$XCODE" -eq 1 ] && ! xcode-select -p >/dev/null 2>&1 && add_cmd "xcode-select --install"
# 32-bit ncurses for an x86 build with AREG_EXTENDED=ON comes from the i386 architecture.
if [ "$PM" = "apt" ] && is_selected x86 && is_selected ncurses; then
    dpkg --print-foreign-architectures 2>/dev/null | grep -q '^i386$' \
        || add_cmd "${SUDO}dpkg --add-architecture i386"
    PKGS="$PKGS libncurses-dev:i386"
fi
if [ -n "$PKGS" ]; then
    case "$PM" in
        apt)    add_cmd "${SUDO}apt-get update"
                add_cmd "${SUDO}apt-get install -y$PKGS" ;;
        dnf)    add_cmd "${SUDO}dnf install -y$PKGS" ;;
        pacman) add_cmd "${SUDO}pacman -S --needed --noconfirm$PKGS" ;;
        brew)   add_cmd "brew install$PKGS" ;;
    esac
fi
# A 32-bit build includes <asm/...>; the versioned multilib package does not link it.
if [ "$PM" = "apt" ] && is_selected x86 && [ ! -e /usr/include/asm ] && [ -d /usr/include/x86_64-linux-gnu/asm ]; then
    add_cmd "${SUDO}ln -s /usr/include/x86_64-linux-gnu/asm /usr/include/asm"
fi
if [ "$PM" = "brew" ] && is_selected java; then
    add_cmd "sudo ln -sfn \"\$(brew --prefix)/opt/openjdk/libexec/openjdk.jdk\" /Library/Java/JavaVirtualMachines/openjdk.jdk"
fi

echo ""
if [ -z "$CMDS" ]; then
    echo "Nothing to install for the selected items."
else
    echo "These commands install the selected tools:"
    echo "$CMDS" | sed '/^$/d; s/^/  /'
fi
[ -n "$NOTES" ] && echo "$NOTES"
echo ""

if [ "$DRY_RUN" -eq 1 ]; then
    echo "Dry run: nothing was run."
    exit 0
fi

if [ -n "$CMDS" ]; then
    if [ "$ASSUME_YES" -ne 1 ]; then
        [ -t 0 ] || fail "no terminal to confirm on; pass --yes to run the commands"
        printf "Run them? [y/N] "
        read -r answer || exit 1
        case "$answer" in y|Y|yes|YES) ;; *) echo "Nothing was run."; exit 1 ;; esac
    fi
    echo "$CMDS" | sed '/^$/d' | while IFS= read -r cmd; do
        echo "> $cmd"
        sh -c "$cmd" || fail "the command failed: $cmd"
    done
fi

sh "$HERE/check-env.sh"
