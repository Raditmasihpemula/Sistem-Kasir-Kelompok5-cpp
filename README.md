AlgoMart 🛒

Sistem Aplikasi Kasir Toko Berbasis C++ dan SQLite

Proyek ini dibuat untuk memenuhi Tugas Besar mata kuliah Algoritma Pemrograman. Aplikasi ini mensimulasikan sistem kasir sederhana (Point of Sales) menggunakan database SQLite untuk penyimpanan data transaksi dan produk.

📋 Fitur Utama

Aplikasi ini dibagi menjadi 6 modul utama sesuai pembagian tugas kelompok:

Input Transaksi 📝

Mencatat pembelian produk.

Validasi ID produk dari database.

Menambahkan item ke keranjang belanja sementara.

Hitung Total 🧮

Menghitung subtotal belanjaan yang ada di keranjang.

Diskon & Promo 🏷️

Menerapkan logika diskon (misal: Diskon 10% untuk belanja > Rp 50.000).

Cetak Struk & Simpan 🖨️

Generate Kode Transaksi Unik (contoh: TRX20241129-123).

Menyimpan data transaksi (Header & Detail) ke database SQLite (toko.db).

Menampilkan struk belanja ke layar.

Laporan Penjualan 📊

Rekap total transaksi harian.

Menghitung total omset harian menggunakan query SUM.

Produk Terlaris 🏆

Menampilkan 3 produk dengan penjualan terbanyak.

Menggunakan query GROUP BY dan ORDER BY.

🛠️ Prasyarat (Prerequisites)

Sebelum menjalankan kode, pastikan komputer sudah terinstall C++ Compiler (G++) dan SQLite3 Development Library.

Untuk Linux (Ubuntu/Kali/Debian):

Wajib menginstall library libsqlite3-dev agar bisa dicompile.

sudo apt-get update
sudo apt-get install g++ sqlite3 libsqlite3-dev


Untuk Windows:

Pastikan MinGW sudah terinstall dan dikonfigurasi dengan library SQLite, atau gunakan Visual Studio.

🚀 Cara Menjalankan Program

Clone Repository

git clone [https://github.com/Raditmasihpemula/Sistem-Kasir-Kelompok5-cpp#](https://github.com/Raditmasihpemula/Sistem-Kasir-Kelompok5-cpp#)
cd Sistem-Kasir-Kelompok5-cpp#


Compile Kode
PENTING: Gunakan flag -lsqlite3 untuk melink library database.

g++ main.cpp -o algomart -lsqlite3


Jalankan Aplikasi

./algomart


Catatan: Database toko.db akan otomatis dibuat saat program pertama kali dijalankan.

👥 Anggota Kelompok
Nama Mahasiswa(NIM)

1. Attar Naswan ()
2. Daffa Issanaputra ()
3. Deni Nawal Alfian ()
4. Nicholas Maximillian Adhi Laksono ()
5. Putra Nur Pratama ()
6. Radit Restu Juniarko (2507421015)

🤝 Panduan Kontribusi (Git Workflow)

Untuk menghindari konflik kode, ikuti langkah ini:

Jangan coding di branch main!

Buat branch baru sesuai fitur masing-masing:

git checkout -b fitur-saya


Setelah selesai, lakukan commit dan push:

git add .
git commit -m "Menambahkan logika diskon"
git push origin fitur-saya


Buat Pull Request di GitHub untuk digabungkan ke main.

Happy Coding! ☕