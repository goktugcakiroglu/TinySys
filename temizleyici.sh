#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Hata: Lutfen bir dosya adi girin!"
    echo "Kullanim: ./temizleyici.sh dosya_adi"
    exit 1
fi

DOSYA=$1
TEMIZ_DOSYA="temiz_$DOSYA"

echo "[$DOSYA] dosyasi isleniyor..."

grep -v "^//" $DOSYA | grep -v "^$" > $TEMIZ_DOSYA

echo "İslem tamamlandi! Temizlenmis kod [$TEMIZ_DOSYA] icine kaydedildi."
