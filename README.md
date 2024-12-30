# Tugas Besar Mata Kuliah Struktur Data

## Anggota Kelompok
1. 21104065 - Novri Anto
2. 21104066 - Doni ...
3. 21104067 - Nurul ...
4. 21104068 - Jefri ...

## Deskripsi Singkat
Kelompok ini membuat simple music player dimana user dapat memilih lagu mana yang ingin diputar, ada fitur untuk playlist juga, dan aplikasi ini juga mampu memutar lagu-lagu yang sudah pernah diputar sebelumnya.

### Fitur Aplikasi
- [x] Play/Pause lagu
- [x] Next lagu
- [x] Previous lagu
- [x] Menampilkan lagu yang sudah pernah diputar sebelumnya
- [x] Menampilkan lagu yang bisa diputar

## Struktur Data yang Digunakan
1. Doubly Linked List - Untuk menyimpan data lagu di playlist
2. Queue - Untuk menyimpan data lagu yang sudah pernah diputar sebelumnya

## Prasyarat
Pastikan Anda memiliki perangkat lunak berikut terinstal di laptop Windows Anda:

1. **Compiler C++**
    - **Opsi 1: Visual Studio 2019 atau lebih baru**
        - **Download:** [Unduh Visual Studio](https://visualstudio.microsoft.com/downloads/)
        - **Instalasi:**
            1. Jalankan installer Visual Studio.
            2. Pilih workload "**Desktop development with C++**".
            3. Klik "Install" dan tunggu hingga proses selesai.

    - **Opsi 2: MinGW-w64 (GCC)**
        - **Download:** [Unduh MinGW-w64](https://mingw-w64.org/doku.php/download)
        - **Instalasi:**
            1. Ekstrak file yang telah diunduh ke direktori pilihan Anda, misalnya `C:\MinGW`.
            2. Tambahkan `C:\MinGW\bin` ke variabel lingkungan PATH:
                - Buka **Control Panel** > **System and Security** > **System**.
                - Klik "**Advanced system settings**".
                - Klik "**Environment Variables**".
                - Di bagian "**System variables**", cari dan pilih variabel `Path`, lalu klik "**Edit**".
                - Klik "**New**" dan tambahkan `C:\MinGW\bin`.
                - Klik "OK" untuk menyimpan perubahan.

2. **pkg-config**
    - **Opsi 1: Menggunakan MSYS2**
        - **Download dan Instal MSYS2:** [Unduh MSYS2](https://www.msys2.org/)
        - **Instalasi:**
            1. Jalankan installer MSYS2 dan ikuti instruksinya.
            2. Buka terminal MSYS2 (MSYS2 MSYS).
            3. Perbarui paket-paket yang ada:
                ```bash
                pacman -Syu
                ```
            4. Instal pkg-config:
                ```bash
                pacman -S pkg-config
                ```

    - **Opsi 2: Instalasi Manual**
        - **Download:** [Unduh pkg-config-lite](https://sourceforge.net/projects/pkgconfiglite/)
        - **Instalasi:**
            1. Ekstrak file yang telah diunduh ke direktori pilihan Anda, misalnya `C:\pkg-config`.
            2. Tambahkan direktori `bin` dari pkg-config ke variabel lingkungan PATH seperti langkah pada opsi MinGW-w64.

3. **Library SFML**
    - **Download:** [Unduh SFML](https://www.sfml-dev.org/download.php)
    - **Pilih Versi yang Sesuai:**
        - Sesuaikan dengan compiler yang Anda gunakan (misalnya GCC untuk MinGW atau Visual C++ untuk Visual Studio).
    - **Ekstrak File:**
        - Ekstrak file yang diunduh ke direktori pilihan Anda, misalnya `C:\SFML`.

## Instruksi Pengaturan

1. **Clone Repository**
    - Buka Command Prompt atau terminal Git.
    - Jalankan perintah berikut untuk meng-clone repository:
        ```bash
        git clone https://github.com/your-username/simple-music-player.git
        cd simple-music-player
        ```

2. **Konfigurasikan Variabel Lingkungan**
    - Tambahkan path ke direktori `bin` SFML ke variabel lingkungan PATH:
        - Buka **Control Panel** > **System and Security** > **System**.
        - Klik "**Advanced system settings**".
        - Klik "**Environment Variables**".
        - Di bagian "**System variables**", cari dan pilih variabel `Path`, lalu klik "**Edit**".
        - Klik "**New**" dan tambahkan `C:\SFML\bin`.
        - Klik "OK" untuk menyimpan perubahan.

3. **Membangun Proyek**

    **Menggunakan Visual Studio:**
    1. Buka Visual Studio.
    2. Pilih "**Create a new project**".
    3. Pilih "**Console App (C++)**" dan klik "Next".
    4. Beri nama proyek dan pilih lokasi sesuai dengan repository yang di-clone.
    5. Setelah proyek dibuat, tambahkan file `main.cpp` ke dalam proyek jika belum ada.
    6. Konfigurasi properti proyek:
        - **C/C++ > General > Additional Include Directories:**
            - Tambahkan `C:\SFML\include`.
        - **Linker > General > Additional Library Directories:**
            - Tambahkan `C:\SFML\lib`.
        - **Linker > Input > Additional Dependencies:**
            - Tambahkan `sfml-audio.lib`, `sfml-system.lib`, dan library SFML lainnya yang diperlukan.
    7. Copy semua DLL dari `C:\SFML\bin` ke direktori tempat executable proyek berada (biasanya `Debug` atau `Release`).
    8. Build solusi dengan menekan `Ctrl + Shift + B` atau melalui menu "Build".

    **Menggunakan g++ (MinGW-w64):**
    1. Buka terminal MSYS2 MinGW.
    2. Navigasikan ke direktori proyek:
        ```bash
        cd /c/path/to/simple-music-player
        ```
    3. Compile proyek dengan perintah berikut:
        ```bash
        g++ -std=c++17 main.cpp -o music_player.exe $(pkg-config --cflags --libs sfml-audio sfml-system)
        ```

## Pengaturan SFML di VSCode

Berikut adalah langkah-langkah untuk mengatur library SFML di Visual Studio Code pada Windows secara spesifik untuk proyek ini:

1. **Konfigurasikan `c_cpp_properties.json`**
    - Buka folder proyek Anda di VSCode.
    - Pergi ke `View` > `Command Palette` (`Ctrl + Shift + P`).
    - Ketik "**C/C++: Edit Configurations (JSON)**" dan pilih.
    - Pastikan konfigurasi ini disimpan di dalam folder `.vscode` di dalam proyek.
    - Tambahkan path include SFML:
        ```json
        {
            "configurations": [
                {
                    "name": "Win32",
                    "includePath": [
                        "${workspaceFolder}/**",
                        "C:/SFML/include"
                    ],
                    "libraryPath": [
                        "C:/SFML/lib"
                    ],
                    "defines": [],
                    "compilerPath": "C:/MinGW/bin/g++.exe",
                    "cStandard": "c11",
                    "cppStandard": "c++17",
                    "intelliSenseMode": "windows-gcc-x64"
                }
            ],
            "version": 4
        }
        ```

2. **Konfigurasikan `tasks.json` untuk Build**
    - Pergi ke `Terminal` > `Configure Tasks` > `Create tasks.json file from template` > `Others`.
    - Pastikan file `tasks.json` disimpan di dalam folder `.vscode` di proyek.
    - Tambahkan konfigurasi build:
        ```json
        {
            "version": "2.0.0",
            "tasks": [
                {
                    "label": "Build",
                    "type": "shell",
                    "command": "g++",
                    "args": [
                        "-std=c++17",
                        "main.cpp",
                        "-o",
                        "music_player.exe",
                        "-IC:/SFML/include",
                        "-LC:/SFML/lib",
                        "-lsfml-audio",
                        "-lsfml-system"
                    ],
                    "group": {
                        "kind": "build",
                        "isDefault": true
                    },
                    "problemMatcher": ["$gcc"]
                }
            ]
        }
        ```

3. **Menjalankan dan Debugging**
    - Pastikan semua DLL dari `C:/SFML/bin` tersedia di direktori executable atau tambahkan ke PATH.
    - Jalankan build task dengan `Ctrl + Shift + B`.
    - Untuk debugging, konfigurasikan `launch.json` sesuai kebutuhan yang juga disimpan di folder `.vscode`.

## Menjalankan Aplikasi
- **Menggunakan Visual Studio:**
    1. Setelah build sukses, tekan `F5` atau klik "Start" untuk menjalankan aplikasi.
- **Menggunakan Command Prompt atau Terminal:**
    1. Pastikan Anda berada di direktori proyek.
    2. Jalankan aplikasi dengan perintah:
        ```bash
        ./music_player.exe
        ```
- **On Mac**
    1. Pastikan Anda berada di direktori proyek.
    2. Jalankan aplikasi dengan perintah:
        ```bash
        g++ -std=c++17  main.cpp -o main $(pkg-config --cflags --libs sfml-audio sfml-system) && ./main
        ```

## Menambahkan Lagu
- Letakkan file `.mp3` Anda di direktori `musics`.
- Aplikasi secara otomatis akan mendeteksi dan menampilkan lagu-lagu tersebut dalam playlist saat dijalankan.

## Pemecahan Masalah

- **SFML Tidak Ditemukan:**
    - Pastikan SFML telah diinstal dengan benar dan bahwa `pkg-config` dapat menemukannya.
    - Jalankan perintah berikut di terminal untuk memeriksa:
        ```bash
        pkg-config --cflags --libs sfml-audio
        ```

- **DLL Hilang:**
    - Jika Anda mendapatkan error mengenai DLL SFML yang hilang saat menjalankan aplikasi, pastikan:
        - Path ke direktori `bin` SFML sudah ditambahkan ke variabel lingkungan PATH.
        - Atau, salin semua file DLL dari `C:\SFML\bin` ke direktori tempat executable berada.

- **Error Kompilasi:**
    - Verifikasi bahwa Anda menggunakan versi SFML yang sesuai dengan compiler yang digunakan.
    - Pastikan semua path yang dibutuhkan sudah dikonfigurasi dengan benar di proyek.
