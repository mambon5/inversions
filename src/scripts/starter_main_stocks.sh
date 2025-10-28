#!/bin/bash

BIN="/var/www/inversions/bin/anal_main"
SRC_DIR="/var/www/inversions/src/core"

# Llista explícita de fitxers .cpp necessaris per a anal_main
CPP_FILES=(
    "$SRC_DIR/time_utils.cpp"
    "$SRC_DIR/curl_utils.cpp"
    "$SRC_DIR/quote.cpp"
    "$SRC_DIR/spot.cpp"
    "$SRC_DIR/dates.cpp"
    "$SRC_DIR/textProcess.cpp"
    "$SRC_DIR/analitza_main_stocks.cpp"
)

# Comprova si el binari no existeix o algun fitxer .cpp és més nou
NEED_COMPILE=false
if [ ! -f "$BIN" ]; then
    NEED_COMPILE=true
else
    for f in "${CPP_FILES[@]}"; do
        if [ "$f" -nt "$BIN" ]; then
            NEED_COMPILE=true
            break
        fi
    done
fi

# Compila si cal
if [ "$NEED_COMPILE" = true ]; then
    echo "Compilant anal_main..."
    g++ "${CPP_FILES[@]}" -o "$BIN" -lcurl
    if [ $? -ne 0 ]; then
        echo "Error compilant anal_main!"
        exit 1
    fi
fi

# Executa el binari
echo password | sudo -S "$BIN" > /var/www/inversions/output/sortida_main.log 2>&1
