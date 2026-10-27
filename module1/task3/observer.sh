#!/bin/bash

CONFIG_FILE="/root/observer.conf"
LOG_FILE="/root/observer.log"

if [ ! -f "$CONFIG_FILE" ]; then
    exit 1
fi

while IFS= read -r SCRIPT_PATH; do
    [ -z "$SCRIPT_PATH" ] && continue

    SCRIPT_NAME=$(basename "$SCRIPT_PATH")
    IS_RUNNING=0

    for proc_dir in /proc/[0-9]*; do
        [ -r "$proc_dir/cmdline" ] || continue
        CMD=$(tr '\0' ' ' < "$proc_dir/cmdline" 2>/dev/null)
        
        if [[ "$CMD" == *"$SCRIPT_NAME"* ]]; then
            IS_RUNNING=1
            break
        fi
    done

    if [ "$IS_RUNNING" -eq 0 ]; then
        echo "$(date '+%Y-%m-%d %H:%M:%S') Перезапуск $SCRIPT_PATH" >> "$LOG_FILE"
	sleep 2
        nohup "$SCRIPT_PATH" > /dev/null 2>&1 &
    fi

done < "$CONFIG_FILE"
