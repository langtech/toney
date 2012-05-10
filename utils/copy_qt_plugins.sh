#! /bin/bash
#
# Utility program for preparing Mac OS X binary package for Toney app.
#
# Copy list of Qt plugins to the app directory. The required plugins are
# stored in the PLUGINS bash array, which needs to be revised if requirement
# changes. Also, QT_PLUGINS_DIR needs to be adjusted for the platform
# specific Qt plugin location.
#

QT_PLUGINS_DIR=/Users/haejoong/QtSDK/Desktop/Qt/4.8.1/gcc/plugins

PLUGINS=(
$QT_PLUGINS_DIR/imageformats/libqgif.dylib
$QT_PLUGINS_DIR/imageformats/libqico.dylib
$QT_PLUGINS_DIR/imageformats/libqjpeg.dylib
$QT_PLUGINS_DIR/imageformats/libqmng.dylib
$QT_PLUGINS_DIR/imageformats/libqsvg.dylib
$QT_PLUGINS_DIR/imageformats/libqtga.dylib
$QT_PLUGINS_DIR/imageformats/libqtiff.dylib
$QT_PLUGINS_DIR/sqldrivers/libqsqlite.dylib
)

usage() {
    echo "Usage: copy_qt_plugins.sh <app dir>"
    echo
}

APPDIR=$1
PLUGIN_BASE=$APPDIR/Contents/MacOS/plugins

if [ -z "$APPDIR" ]; then
    usage
    exit 1
fi

if [ ! -d "$APPDIR" ]; then
    echo "directory not found: $APPDIR"
    exit 1
fi


for f in "${PLUGINS[@]}"; do
    p=$PLUGIN_BASE/${f#*/plugins/}
    mkdir -p $(dirname "$p")
    cp -v "$f" "$p"
done
