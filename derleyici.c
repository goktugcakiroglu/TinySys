#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// --- Sembol Tablosu Yapısı ---
typedef struct {
    char name[20];
    int defined; // 0: Tanımlı değil, 1: Tanımlı
} Symbol;

Symbol symbolTable[50]; // En fazla 50 değişken
int symbolCount = 0;

// Değişkeni bul veya ekle
void registerSymbol(char *name) {
    for(int i = 0; i < symbolCount; i++) {
        if(strcmp(symbolTable[i].name, name) == 0) return; // Zaten var
    }
    strcpy(symbolTable[symbolCount].name, name); 
    symbolTable[symbolCount].defined = 1;
    symbolCount++;
}

// Tanımlı mı kontrol et
int isDefined(char *name) {
    for(int i = 0; i < symbolCount; i++) {
        if(strcmp(symbolTable[i].name, name) == 0) return 1;
    }
    return 0;
}

// --- KÜRESEL DURUM DEĞİŞKENLERİ ---
int if_sayaci = 0; // İç içe olmayan IF bloklarını numaralandırmak için
int while_sayaci = 0;

//1. STRATEJİ (KOMUT) FONKSİYONLARI

void translate_print(char *satir) {
    char arg1[20];
    sscanf(satir, "PRINT %s", arg1);
    printf("; --- Ekrana Yazdirma: %s ---\n", satir);
    printf("\tLDAA %s\n", arg1);
    printf("\tJSR PRINT\n\n");
}

void translate_add(char *satir) {
    char hedef[20], arg1[20], arg2[20];
    sscanf(satir, "%s = %s + %s", hedef, arg1, arg2);

    if(!isDefined(arg1) && (arg1[0] < '0' || arg1[0] > '9')) {
        printf("; HATA: Tanimlanmamis degisken kullanildi -> %s\n", arg1);
        return;
    }
    
    printf("; --- Toplama: %s ---\n", satir);
    printf("\tLDAA %s\n", arg1);
    
    if (arg2[0] >= '0' && arg2[0] <= '9') {
        printf("\tADDA #%s\n", arg2); 
    } else {
        printf("\tADDA %s\n", arg2);  
    }
    
    printf("\tSTAA %s\n\n", hedef);
}

void translate_sub(char *satir) {
    char hedef[20], arg1[20], arg2[20];
    sscanf(satir, "%s = %s - %s", hedef, arg1, arg2);
    printf("; --- Cikarma: %s ---\n", satir);
    printf("\tLDAA %s\n", arg1);

    if (arg2[0] >= '0' && arg2[0] <= '9') {
        printf("\tSUBA #%s\n", arg2); 
    } else {
        printf("\tSUBA %s\n", arg2);  
    }

    printf("\tSTAA %s\n\n", hedef);
}

void translate_and(char *satir) {
    char hedef[20], arg1[20], arg2[20];
    sscanf(satir, "%s = %s & %s", hedef, arg1, arg2);
    printf("; --- Mantiksal VE: %s ---\n", satir);
    printf("\tLDAA %s\n", arg1);
    printf("\tANDA %s\n", arg2);
    printf("\tSTAA %s\n\n", hedef);
}

void translate_if(char *satir) {
    char arg1[20], arg2[20];
    sscanf(satir, "IF %s == %s", arg1, arg2);
    if_sayaci++;
    printf("; --- KOSUL (IF) BASLANGICI: %s ---\n", satir);
    printf("\tLDAA %s\n", arg1);
    
    // Eğer ikinci argüman bir sayıysa (örn: 10), başına '#' koy (Immediate Addr)
    if (arg2[0] >= '0' && arg2[0] <= '9') {
        printf("\tCMPA #%s\n", arg2);
    } else {
        printf("\tCMPA %s\n", arg2);
    }
    
    // Z=0 ise (Eşit değilse) bloğun sonuna zıpla. 
    // ETİKETİN YANINDAKİ BOŞLUĞA DİKKAT! (IDE'nin çökmemesi için)
    printf("\tBNE L_ENDIF_%d \n\n", if_sayaci); 
}

void translate_endif(char *satir) {
    printf("; --- KOSUL (IF) BITISI ---\n");
    // IDE kuralları gereği etiket en sola dayalı ve yanında BOŞLUK var!
    printf("L_ENDIF_%d \n\n", if_sayaci); 
}

void translate_assign(char *satir) {
    char var_name[20], value[20];
    sscanf(satir, "%s = %s", var_name, value);
    
    registerSymbol(var_name); // Sembol tablosuna kaydettik!
    
    printf("; --- Atama: %s ---\n", satir);
    
    // Değer sayı mı değişken mi?
    if (value[0] >= '0' && value[0] <= '9') {
        printf("\tLDAA #%s\n", value); // Sayıysa Immediate
    } else {
        printf("\tLDAA %s\n", value);  // Değişken ise Direct
    }
    
    printf("\tSTAA %s\n\n", var_name);
}

void translate_while(char *satir) {
    char arg1[20], arg2[20];
    // Sadece < (küçüktür) şartını destekleyen basit bir WHILE motoru
    sscanf(satir, "WHILE %s < %s", arg1, arg2);
    while_sayaci++;
    
    printf("; --- DONGU (WHILE) BASLANGICI: %s ---\n", satir);
    // SDK6800 Kuralı: Etiket en solda ve yanında BOŞLUK var!
    printf("L_WHILE_START_%d \n", while_sayaci); 
    printf("\tLDAA %s\n", arg1);
    
    if (arg2[0] >= '0' && arg2[0] <= '9') {
        printf("\tCMPA #%s\n", arg2);
    } else {
        printf("\tCMPA %s\n", arg2);
    }
    
    // X < 10 arıyoruz. Eğer BÜYÜK veya EŞİTSE (BGE) döngüden çıkmalı! (PDF Sayfa 2)
    printf("\tBGE L_WHILE_END_%d \n\n", while_sayaci); 
}

void translate_endwhile(char *satir) {
    printf("; --- DONGU (WHILE) BITISI ---\n");
    // Döngünün başına dön (BRA - Branch Always)
    printf("\tBRA L_WHILE_START_%d \n", while_sayaci); 
    // Çıkış etiketi
    printf("L_WHILE_END_%d \n\n", while_sayaci); 
}

void translate_pointer_init(char *satir) {
    char ptr_adi[20], adres[20];
    // POINTER P = &SAYAC formatını yakala
    sscanf(satir, "POINTER %s = &%s", ptr_adi, adres); 
    printf("; --- Pointer Atama: %s ---\n", satir);
    printf("\tLDX #%s     ; %s adresini Index Register'a yukle\n\n", adres, adres);
}

void translate_pointer_set(char *satir) {
    char ptr_adi[20], deger[20];
    // *P = 10 formatını yakala
    sscanf(satir, "*%s = %s", ptr_adi, deger); 
    printf("; --- Pointer ile Deger Degistirme: %s ---\n", satir);
    
    if (deger[0] >= '0' && deger[0] <= '9') {
        printf("\tLDAA #%s\n", deger);
    } else {
        printf("\tLDAA %s\n", deger);
    }
    // XR'ın gösterdiği adrese (0 ofset ile) A'daki değeri yaz!
    printf("\tSTAA 0,X\n\n"); 
}

void translate_read(char *satir) {
    char arg1[20];
    sscanf(satir, "READ %s", arg1);
    printf("; --- Klavyeden Veri Okuma: %s ---\n", satir);
    printf("\tJSR READ\n");      // Klavyeyi dinleyen alt programa git (A Akümülatörüne veri dolar)
    printf("\tSTAA %s\n\n", arg1); // Gelen veriyi değişkene kaydet
}

 //2. MİMARİ ARAYÜZ (INTERFACE) VE KAYIT DEFTERİ

typedef void (*TranslateStrategy)(char *satir);

typedef struct {
    char *pattern;
    TranslateStrategy execute;
} Rule;

// OCP KURALLARI: Yeni bir özellik mi lazım? Sadece buraya ekle!
Rule rules[] = {
    {"POINTER", translate_pointer_init},
    {"*", translate_pointer_set},
    {"READ", translate_read},
    {"PRINT", translate_print},
    {"ENDIF", translate_endif},
    {"IF", translate_if},
    {"ENDWHILE", translate_endwhile}, 
    {"WHILE", translate_while},       
    {"+", translate_add},
    {"-", translate_sub},
    {"&", translate_and},
    {"=", translate_assign} // '=' karakteri diğer operatörlerden sonra gelmeli
};

#define RULE_COUNT (sizeof(rules) / sizeof(rules[0]))

//3. DERLEYİCİ MOTORU

void assembly_cevir(char *satir) {
    satir[strcspn(satir, "\n")] = 0;
    satir[strcspn(satir, "\r")] = 0;

    for (int i = 0; i < RULE_COUNT; i++) {
        if (strstr(satir, rules[i].pattern) != NULL) {
            rules[i].execute(satir);
            return;
        }
    }
    printf("; HATA: Taninmayan TinyLang Komutu -> %s\n", satir);
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    FILE *dosya = fopen(argv[1], "r");
    if (dosya == NULL) return 1;

    printf("; --- MOTOROLA 6800 ASSEMBLY CIKTISI (FULL-STACK TINYSYS) ---\n\n");

    char satir[100];
    while (fgets(satir, sizeof(satir), dosya) != NULL) {
        assembly_cevir(satir);
    }

    fclose(dosya);
    return 0;
}
