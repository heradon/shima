#!/bin/sh
if ! ps -auw | grep -q '[/]home/odroid/startup'; then
    /home/odroid/startup /home/odroid/startup
fi
