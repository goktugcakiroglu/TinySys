#!/bin/bash

# 1. Aşama: C Derleyici Motorumuzu (Backend) Makine Diline Çevir (Derle)
echo ">>> [1/3] C Motoru derleniyor..."
gcc derleyici.c -o derleyici

# Eğer C kodunda hata varsa sistemi durdur
if [ $? -ne 0 ]; then
    echo "Hata: C kodu derlenemedi! Lutfen kodunu kontrol et."
    exit 1
fi

# 2. Aşama: Saf Kaynak Kodumuzu Temizle
echo ">>> [2/3] Kod temizleyici (Lexical on-hazirlik) calisiyor..."
./temizleyici.sh test_kodu.txt

# 3. Aşama: Temizlenmiş Kodu C Motoruna Gönder
echo ">>> [3/3] C Derleyici Motoru baslatiliyor ve 'cikti.asm' uretiliyor..."
./derleyici temiz_test_kodu.txt > cikti.asm

echo ">>> SISTEM DONGUSU BASARIYLA TAMAMLANDI!"
