#!/bin/bash
set -e

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "========================================="
echo "Building ClipGrab for Apple Silicon (arm64)"
echo "========================================="

# 1. Check Homebrew
if ! command -v brew &> /dev/null; then
    echo -e "${RED}Error: Homebrew is not installed. Please install Homebrew first.${NC}"
    exit 1
fi

# 2. Check and install qt
if [ ! -d "/opt/homebrew/opt/qt" ]; then
    echo "Installing qt (Qt 6) via Homebrew..."
    brew install qt
else
    echo "qt (Qt 6) is already installed."
fi

# 3. Check and install ffmpeg
if ! command -v ffmpeg &> /dev/null; then
    echo "Installing ffmpeg via Homebrew..."
    brew install ffmpeg
fi

# 4. Check and install deno
if ! command -v deno &> /dev/null; then
    echo "Installing deno via Homebrew..."
    brew install deno
fi

# 5. Set up paths
QMAKE="/opt/homebrew/opt/qt/bin/qmake"
MACDEPLOYQT="/opt/homebrew/opt/qt/bin/macdeployqt"

if [ ! -f "$QMAKE" ]; then
    echo -e "${RED}Error: qmake not found at $QMAKE${NC}"
    exit 1
fi

# Clean previous build
echo "Cleaning old build artifacts..."
make clean &>/dev/null || true
rm -rf ClipGrab.app Makefile

# 6. Run qmake and make
echo "Configuring project with qmake..."
"$QMAKE" clipgrab.pro

echo "Compiling ClipGrab..."
make -j$(sysctl -n hw.ncpu)

# 7. Package Qt libraries
echo "Deploying Qt frameworks..."
"$MACDEPLOYQT" ClipGrab.app

# Fix QtWebEngineProcess library loading issue
echo "Creating helper dynamic framework symlink for QtWebEngine..."
mkdir -p ClipGrab.app/Contents/Frameworks/QtWebEngineCore.framework/Versions/A/Helpers/QtWebEngineProcess.app/Contents
ln -sf ../../../../../../ ClipGrab.app/Contents/Frameworks/QtWebEngineCore.framework/Versions/A/Helpers/QtWebEngineProcess.app/Contents/Frameworks


# 8. Copy native arm64 dependencies
echo "Bundling native arm64 ffmpeg and deno..."
cp /opt/homebrew/bin/ffmpeg ClipGrab.app/Contents/MacOS/
cp /opt/homebrew/bin/deno ClipGrab.app/Contents/MacOS/

# 9. Verify architecture
echo -e "${YELLOW}Verifying compiled executable architectures:${NC}"
file ClipGrab.app/Contents/MacOS/ClipGrab
file ClipGrab.app/Contents/MacOS/ffmpeg
file ClipGrab.app/Contents/MacOS/deno

echo -e "${GREEN}Success! ClipGrab.app is built natively for Apple Silicon arm64.${NC}"
echo "You can move it to /Applications/ to replace the old version."
