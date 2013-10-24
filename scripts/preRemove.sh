#!/bin/sh
# prerm script
#
# Removes all files installed by: ./setup.py install
echo "pre remove script"
update-rc.d @PROJECT_NAME@ remove
