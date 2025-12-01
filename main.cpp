#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h> // Pastikan library sqlite3 sudah terinstall di laptop
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

// --- STRUKTUR DATA ---
struct Produk {
    int id;
    string nama;
    double harga;
};

struct ItemBelanja {
    Produk produk;
    int jumlah;
    double subtotal;
};

// --- GLOBAL VARIABLES ---
sqlite3* DB;
vector<ItemBelanja> keranjang;
string kodeTransaksiSaatIni;

// --- DATABASE SETUP ---
void inisialisasiDatabase() {
    string sql = 
        "CREATE TABLE IF NOT EXISTS PRODUK("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "NAMA TEXT NOT NULL, "
        "HARGA REAL NOT NULL);"
        
        "CREATE TABLE IF NOT EXISTS TRANSAKSI("
        "KODE_UNIK TEXT PRIMARY KEY, "
        "TANGGAL TEXT, "
        "TOTAL_BAYAR REAL);"

        "CREATE TABLE IF NOT EXISTS DETAIL_TRANSAKSI("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "KODE_TRANSAKSI TEXT, "
        "NAMA_PRODUK TEXT, "
        "JUMLAH INTEGER, "
        "HARGA_SATUAN REAL);";

    char* messageError;
    int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "Error Create Table: " << messageError << endl;
        sqlite3_free(messageError);
    } else {
        // Data Dummy untuk tes awal
        string dummy = "INSERT OR IGNORE INTO PRODUK (ID, NAMA, HARGA) VALUES (1, 'Kopi Susu', 15000), (2, 'Roti Bakar', 12000), (3, 'Indomie Goreng', 10000), (4, 'Rokok', 25000);";
        sqlite3_exec(DB, dummy.c_str(), NULL, 0, NULL);
    }
}

// --- FITUR 1: INPUT TRANSAKSI (Anggota 1) ---
void inputTransaksi() {
    int idProduk, jumlah;
    cout << "\n--- INPUT TRANSAKSI ---\n";
    cout << "Masukkan ID Produk (1-3): "; // Sesuai data dummy
    cin >> idProduk;

    string sql = "SELECT NAMA, HARGA FROM PRODUK WHERE ID = " + to_string(idProduk) + ";";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string nama = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            double harga = sqlite3_column_double(stmt, 1);
            
            cout << "Produk: " << nama << " @ Rp" << (size_t)harga << endl;
            cout << "Jumlah: ";
            cin >> jumlah;

            ItemBelanja item;
            item.produk.id = idProduk;
            item.produk.nama = nama;
            item.produk.harga = harga;
            item.jumlah = jumlah;
            item.subtotal = harga * jumlah;

            keranjang.push_back(item);
            cout << "Sukses masuk keranjang!\n";
        } else {
            cout << "Produk tidak ditemukan!\n";
        }
    }
    sqlite3_finalize(stmt);
}

// --- FITUR 2: HITUNG TOTAL (Anggota 2) ---
double hitungTotal() {
    double total = 0;
    for (const auto& item : keranjang) {
        total += item.subtotal;
    }
    return total;
}

// --- FITUR 3: DISKON (Anggota 3) ---
double hitungDiskon(double totalBelanja) {
    double diskon = 0;
    if (totalBelanja > 50000) { // Contoh: Diskon jika beli > 50rb
        diskon = totalBelanja * 0.10;
        cout << "Diskon 10% diterapkan!\n";
    }
    return diskon;
}

// --- FITUR 4: CETAK & SIMPAN (Anggota 4) ---
void cetakStrukDanSimpan() {
    if (keranjang.empty()) {
        cout << "Keranjang kosong.\n"; return;
    }

    double total = hitungTotal();
    double diskon = hitungDiskon(total);
    double grandTotal = total - diskon;

    // Generate Kode Unik
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, 80, "TRX%Y%m%d", ltm);
    string prefix(buffer);
    kodeTransaksiSaatIni = prefix + "-" + to_string(rand() % 1000);

    strftime(buffer, 80, "%Y-%m-%d", ltm);
    string tanggal(buffer);

    // Simpan ke DB
    string sqlHeader = "INSERT INTO TRANSAKSI (KODE_UNIK, TANGGAL, TOTAL_BAYAR) VALUES ('" 
                       + kodeTransaksiSaatIni + "', '" + tanggal + "', " + to_string(grandTotal) + ");";
    sqlite3_exec(DB, sqlHeader.c_str(), NULL, 0, NULL);

    for (const auto& item : keranjang) {
        string sqlDetail = "INSERT INTO DETAIL_TRANSAKSI (KODE_TRANSAKSI, NAMA_PRODUK, JUMLAH, HARGA_SATUAN) VALUES ('"
                           + kodeTransaksiSaatIni + "', '" + item.produk.nama + "', " 
                           + to_string(item.jumlah) + ", " + to_string(item.produk.harga) + ");";
        sqlite3_exec(DB, sqlDetail.c_str(), NULL, 0, NULL);
    }

    // Output Struk
    cout << "\n=== STRUK " << kodeTransaksiSaatIni << " ===\n";
    for (const auto& item : keranjang) {
        cout << item.produk.nama << " x " << item.jumlah << " = " << (size_t)item.subtotal << endl;
    }
    cout << "------------------------\n";
    cout << "Total   : " << (size_t)total << endl;
    cout << "Diskon  : " << (size_t)diskon << endl;
    cout << "Bayar   : " << (size_t)grandTotal << endl;
    
    keranjang.clear();
}

// --- FITUR 5: LAPORAN HARIAN (Anggota 5) ---
void laporanHarian() {
    cout << "\n--- LAPORAN HARI INI ---\n";
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d", ltm);
    string today(buffer);

    string sql = "SELECT COUNT(*), SUM(TOTAL_BAYAR) FROM TRANSAKSI WHERE TANGGAL = '" + today + "';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Tanggal: " << today << endl;
            cout << "Total Transaksi: " << sqlite3_column_int(stmt, 0) << endl;
            double omset = sqlite3_column_double(stmt, 1);
            cout << "Omset: Rp " << (size_t)omset << endl;
        }
    }
    sqlite3_finalize(stmt);
}

// --- FITUR 6: PRODUK TERLARIS (Anggota 6) ---
void produkTerlaris() {
    cout << "\n--- TOP PRODUK ---\n";
    string sql = "SELECT NAMA_PRODUK, SUM(JUMLAH) as Qty FROM DETAIL_TRANSAKSI "
                 "GROUP BY NAMA_PRODUK ORDER BY Qty DESC LIMIT 3;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_text(stmt, 0) << " (Terjual: " << sqlite3_column_int(stmt, 1) << ")\n";
        }
    }
    sqlite3_finalize(stmt);
}

int main() {
    if (sqlite3_open("toko.db", &DB)) {
        cerr << "Gagal koneksi DB!\n"; return -1;
    }
    inisialisasiDatabase();

    int pil;
    do {
        cout << "\n1. Input | 2. Cek Total | 3. Cek Diskon | 4. Bayar | 5. Laporan | 6. Terlaris | 0. Keluar\nPilih: ";
        cin >> pil;
        switch(pil) {
            case 1: inputTransaksi(); break;
            case 2: cout << "Total: " << (size_t)hitungTotal() << endl; break;
            case 3: hitungDiskon(hitungTotal()); break;
            case 4: cetakStrukDanSimpan(); break;
            case 5: laporanHarian(); break;
            case 6: produkTerlaris(); break;
        }
    } while (pil != 0);

    sqlite3_close(DB);
    return 0;
}