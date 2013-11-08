#!/bin/sh
# prerm script
#
# Removes all files installed by: ./setup.py install
echo "pre remove script"
service @PROJECT_NAME@ stop
update-rc.d @PROJECT_NAME@ remove
