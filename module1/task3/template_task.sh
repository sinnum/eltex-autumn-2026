#!/bin/bash

SCRIPT_NAME=$(basename "$0")
if [ "$SCRIPT_NAME" == "template_task.sh" ]; then
    echo "я бригадир, сам не работаю"
    exit 0
fi

LOG_FILE="report_${SCRIPT_NAME}.log"
echo "$(date '+%Y-%m-%d %H:%M:%S') [$$] Скрипт запущен" >> "$LOG_FILE"

CUCKOO_PID=""
FIFO=""

for proc_dir in /proc/[0-9]*; do
    [ -r "$proc_dir/cmdline" ] || continue
    CMD=$(tr '\0' ' ' < "$proc_dir/cmdline" 2>/dev/null)
    if [[ "$CMD" == *"cuckoo.sh"* && "$CMD" == *"--pishitesuda="* ]]; then
        CUCKOO_PID=$(basename "$proc_dir")
        TMP="${CMD#*--pishitesuda=}"
        FIFO="${TMP%% *}"
        break
    fi
done

if [ -z "$CUCKOO_PID" ] || [ -z "$FIFO" ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S') [$$] Ошибка: cuckoo.sh не запущен или канал не найден!" >> "$LOG_FILE"
    exit 1
fi

echo "$SCRIPT_NAME[$$]: how much time do I have?" > "$FIFO"

N=""
while read -r line < "$FIFO"; do
    if [[ "$line" =~ ^$SCRIPT_NAME\[$$\]\ ([0-9]+)$ ]]; then
        N="${BASH_REMATCH[1]}"
        break
    fi
done

if [ -z "$N" ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S') [$$] Ошибка: пустой ответ" >> "$LOG_FILE"
    exit 1
fi

sleep "$N"

echo "$(date '+%Y-%m-%d %H:%M:%S') [$$] Скрипт завершился, работал $N секунд." >> "$LOG_FILE"
