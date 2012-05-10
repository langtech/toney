#! /usr/bin/env python

"""
Print out commands that take care of dependencies of an Mac OS X app.
  - Copies frameworks that need to be distributed with the app.
  - Fixes external paths encoded in the binaries. Turn them into relative
    paths that point to binaries within the app directory.

This script was specially designed and customized for Toney and may not work
with Mac OS X apps in general.

Usage:

  Create the plugins directory within the app directory and copy over any
  necessary Qt plugins to the created plugins directory. Then run this
  program and pipe the output to bash.

  $ mkdir MyApp.app/Contents/MacOS/plugins
  $ cp -r <path_to_qt>/plugins/<needed_plugin> MyApp.app/Contents/MacOS/plugins
  $ cp -r <path_to_qt>/plugins/<other_plugin> MyApp.app/Contents/MacOS/plugins
  ...
  $ prep_app.py MyApp.app | bash

"""

import sys
import os
from subprocess import Popen, PIPE

APPDIR = sys.argv[1]  # the app directory
FRMDIR = os.path.join(APPDIR, 'Contents/Frameworks')
RESDIR = os.path.join(APPDIR, 'Contents/Resources')
PLUGINDIR = os.path.join(APPDIR, 'Contents/MacOS/plugins')
PROG = os.path.join(APPDIR, 'Contents/MacOS/Toney')


def qt_framework_dir(libpath):
    """
    Guesses the base framework directory containing the given library file.
    """
    return libpath.split('.framework/')[0] + '.framework/'

def is_dependency(libpath):
    """
    Guesses whether the library should be distributed as a dependency.
    """
    if '/QtSDK/' in libpath:
        return True
    if libpath.startswith('/opt/local/lib/'):  # libs from macports
        return True
    return False

def find_dependencies(libpath, dep):
    """
    For given binary, find its dependencies recursively.
    """
    pipe = Popen(['otool','-L',libpath], stdout=PIPE)
    L = pipe.stdout.readlines()
    pipe.communicate()
    for l in L:
        apath = l.strip().split()[0]
        if apath.endswith(':'):
            continue
        if apath not in dep and is_dependency(apath):
            name = os.path.basename(libpath).split('.')[0]
            name2 = os.path.basename(apath).split('.')[0]
            if name == name2:
                # hmm.. it depends on itself
                continue
            dep[apath] = "@executable_path/../Frameworks/" + os.path.basename(apath)
            find_dependencies(apath, dep)

dep = {}  # dependencies list. Key=original path. Value=relative path
find_dependencies(PROG, dep)

if os.path.isdir(PLUGINDIR):
    for dirpath, dirnames, filenames in os.walk(PLUGINDIR):
        for filename in filenames:
            path = os.path.join(dirpath, filename)
            find_dependencies(path, dep)
            
print "rm -rf %s/*" % FRMDIR
print "rm -rf %s/*" % RESDIR
print "mkdir -p %s" % FRMDIR
print "mkdir -p %s" % RESDIR

for x, newpath in dep.items():
    print "install_name_tool -change %s %s %s" % (x, newpath, PROG)

for x in dep:
    libfile = os.path.basename(x)
    libpath = os.path.join(APPDIR, 'Contents/Frameworks', libfile)
    print "cp %s %s" % (x, libpath)

    for y, newpath in dep.items():
        if x != y:
            print "install_name_tool -change %s %s %s" % (y, newpath, libpath)

    print "install_name_tool -id %s %s" % (dep[x], libpath)

for x in dep:
    if '/QtSDK/' in x:
        base = qt_framework_dir(x)
        resdir = os.path.join(base, 'Versions/Current/Resources')
        if os.path.exists(resdir):
            print "find %s -depth 1 -exec cp -r \\{} %s \\;" % (resdir, RESDIR)

if os.path.isdir(PLUGINDIR):
    for dirpath, dirnames, filenames in os.walk(PLUGINDIR):
        for filename in filenames:
            path = os.path.join(dirpath, filename)
            pipe = Popen(['otool','-D',path], stdout=PIPE)
            L = pipe.stdout.readlines()
            orgid = L[1].strip()
            for x, newpath in dep.items():
                print "install_name_tool -change %s %s %s" % (x, newpath, path)

            p = dirpath.split('/plugins/')[1]
            newpath = "@executable_path/plugins/" + p
            print "install_name_tool -id %s %s" % (newpath, path)
            
