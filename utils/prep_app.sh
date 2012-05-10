#! /bin/bash
#
# Create a distributable Mac OS X app with all its dependencies resolved.
#
# This program is written for Toney app and does not work with other apps 
# in general.
#

set -e -o pipefail

usage() {
    echo "Usage: prep_app.sh <source app>"
    echo
    echo "  <source app> is an app directory (e.g. MyApp.app) that was created"
    echo "  by Qt as a result of building a Qt project (.pro file)."
    echo
    echo "  <source app> is copied to currently and its dependencies are"
    echo "  added to the new directory."
    echo
}

case $0 in
    /*) thisdir=$(dirname $0) ;;
    *)  thisdir=$(pwd)/$(dirname $0) ;;
esac

BUILT_APP=$1

if [ -z "$BUILT_APP" ]; then
    usage
    exit 1
fi

APPDIR=`basename $BUILT_APP`

if [ -d "$APPDIR" ]; then
    echo "App directory already exists: $APPDIR"
    echo
    exit 1
fi

trap "echo; echo ABORT: error while creating the app.; rm -rf $APPDIR" ERR

echo "** Cloning the built app..."
cp -rv $BUILT_APP $APPDIR
echo

echo "** Copying play command..."
cp -v /opt/local/bin/play $APPDIR/Contents/MacOS/
echo

echo "** Copying plugins..."
$thisdir/copy_qt_plugins.sh $APPDIR
echo

echo "** Copying help docs..."
cp -v $thisdir/../doc/toney.q{hc,ch} $APPDIR/Contents/MacOS/
echo

echo "** Resolving dependencies..."
$thisdir/resolve_dependencies.py $APPDIR | bash
$thisdir/resolve_dependencies.py $APPDIR play | grep -v ^rm | bash
echo

