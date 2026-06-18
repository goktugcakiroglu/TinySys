import tkinter as tk
from tkinter import messagebox
import subprocess
import os

def sistemi_calistir():
    kullanici_kodu = text_girdi.get("1.0", tk.END).strip()

    if not kullanici_kodu:
        messagebox.showerror("Uyarı", "Lütfen önce sol tarafa kodunuzu yazın!")
        return
    
    # Satır sonu uyumsuzluklarını önlemek için newline='\n' eklendi
    with open("test_kodu.txt", "w", encoding="utf-8", newline="\n") as f:
        f.write(kullanici_kodu)

    try:
        # 1. DOKUNUŞ: encoding="utf-8" ve errors="replace" parametreleri eklendi!
        sonuc = subprocess.run(
            ["bash", "./tam_sistem.sh"], 
            capture_output=True, 
            text=True,
            encoding="utf-8",
            errors="replace"
        )
        
        if sonuc.returncode != 0:
            hata_mesaji = sonuc.stderr if sonuc.stderr else sonuc.stdout
            raise Exception(hata_mesaji)

        # 2. DOKUNUŞ: Dosya okurken de errors="replace" eklendi!
        with open("cikti.asm", "r", encoding="utf-8", errors="replace") as f:
            assembly_kodu = f.read()

        text_cikti.delete("1.0", tk.END)
        text_cikti.insert(tk.END, assembly_kodu)
    
    except Exception as e:
        messagebox.showerror("Sistem Hatası (Terminal Çıktısı)", f"Arka planda bir şeyler ters gitti:\n\n{e}")

# --- ARAYÜZ TASARIMI (Responsive Grid) ---
app = tk.Tk()
app.title("TinySys: İşletim Ekosistemi")
app.geometry("900x600")
app.configure(bg="#2b2b2b")

# Başlık (Pack ile üstte durmaya devam edebilir)
lbl_baslik = tk.Label(app, text="TinySys Full-Stack Derleyici", font=("Arial", 16, "bold"), bg="#2b2b2b", fg="white")
lbl_baslik.pack(pady=10)

# Ana Çerçeve (Kutuların içine gireceği esnek alan)
frame = tk.Frame(app, bg="#2b2b2b")
frame.pack(padx=10, pady=5, fill=tk.BOTH, expand=True)

# SİHİRLİ DOKUNUŞ: Grid sütunlarına ve satırına ağırlık (weight) veriyoruz. 
# Böylece ekran büyüdükçe %50 %50 genişleyecekler.
frame.columnconfigure(0, weight=1)
frame.columnconfigure(1, weight=1)
frame.rowconfigure(0, weight=1)

# --- Sol Panel (Grid 0. Sütun) ---
frame_sol = tk.Frame(frame, bg="#2b2b2b")
frame_sol.grid(row=0, column=0, sticky="nsew", padx=(0, 5))

lbl_sol = tk.Label(frame_sol, text="Yüksek Seviye Kod (TinyLang)", bg="#2b2b2b", fg="#4CAF50", font=("Arial", 11, "bold"))
lbl_sol.pack()

text_girdi = tk.Text(frame_sol, font=("Consolas", 12), bg="#1e1e1e", fg="white", insertbackground="white")
text_girdi.pack(fill=tk.BOTH, expand=True)

varsayilan_kod = ""
if os.path.exists("test_kodu.txt"):
    with open("test_kodu.txt", "r", encoding="utf-8") as f:
        varsayilan_kod = f.read()

text_girdi.insert(tk.END, varsayilan_kod)

# --- Sağ Panel (Grid 1. Sütun) ---
frame_sag = tk.Frame(frame, bg="#2b2b2b")
frame_sag.grid(row=0, column=1, sticky="nsew", padx=(5, 0))

lbl_sag = tk.Label(frame_sag, text="Motorola 6800 Assembly Çıktısı", bg="#2b2b2b", fg="#00BCD4", font=("Arial", 11, "bold"))
lbl_sag.pack()

text_cikti = tk.Text(frame_sag, font=("Consolas", 12), bg="#1e1e1e", fg="#e6e6e6", insertbackground="white")
text_cikti.pack(fill=tk.BOTH, expand=True)

# Derle Butonu (Yine Pack ile altta ortalanır)
btn_derle = tk.Button(app, text="🚀 Kodu Derle ve Assembly'e Çevir", font=("Arial", 12, "bold"), bg="#E91E63", fg="white", command=sistemi_calistir)
btn_derle.pack(pady=15, ipadx=10, ipady=5)

app.mainloop()