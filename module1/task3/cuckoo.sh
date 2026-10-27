#!/bin/bash

for arg in "$@"; do
  case "$arg" in
    --pishitesuda=*)
      FIFO="${arg#*=}"
      ;;
  esac
done

if [ -z "$FIFO" ]; then
  exit 1
fi

LOG="/var/log/cuckoo.log"

log_msg() {
  echo "$(date '+%Y-%m-%d %H:%M:%S') $1" >> "$LOG"
}

cleanup() {
  log_msg "Shutdown!"
  rm -f "$FIFO"
  exit 0
}

trap cleanup SIGTERM SIGINT

[ -p "$FIFO" ] || mkfifo "$FIFO"
log_msg "Startup!"

while true; do
  if read -r line < "$FIFO"; then
    if [[ "$line" =~ ^([a-zA-Z0-9_.-]+)\[([0-9]+)\]:\ how\ much\ time\ do\ I\ have\?$ ]]; then
      NAME="${BASH_REMATCH[1]}"
      PID="${BASH_REMATCH[2]}"
      N=$((RANDOM % 9 + 2))
      log_msg "$NAME[$PID] $N"
      echo "$NAME[$PID] $N" > "$FIFO"
    fi
  fi
done
