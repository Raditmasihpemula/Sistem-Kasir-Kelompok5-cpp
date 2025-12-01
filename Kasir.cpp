#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <limits> // Tambahan untuk membersihkan buffer input

using namespace std;

struct Produk {
    string id;
    string nama;
    double harga;
    int stok;
    int terjual;
};

struct ItemKeranjang {
    Produk* produk;
    int jumlah;
    double subtotal;
};

vector<Produk> inventaris = {
    {"P001", "Kopi Hitam", 15000, 50, 0},
    {"P002", "Latte", 22000, 40, 0},
    {"P003", "Roti Bakar", 12000, 30, 0},
    {"P004", "Nasi Goreng", 25000, 20, 0},
    {"P005", "Air Mineral", 5000, 100, 0}
};

// --- FUNGSI BANTUAN ---

string generateTransactionID() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];
    strftime(buffer, 20, "%Y%m%d%H%M%S", ltm);
    return string(buffer);
}

void printRupiah(double nominal) {
    cout << "Rp " << fixed << setprecision(0) << nominal;
}

Produk* cariProduk(string id) {
    for (auto &p : inventaris) {
        if (p.id == id) return &p;
    }
    return nullptr;
}

// --- FUNGSI BARU: MEMBERSIHKAN INPUT ERROR ---
void clearInputBuffer() {
    cin.clear(); // Hapus flag error pada cin
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Buang sisa input sampah
}

// --- FITUR UTAMA ---

void simpanLaporan(string idTrans, double total, string diskonCode) {
    ofstream file("laporan_penjualan.txt", ios::app);
    if (file.is_open()) {
        time_t now = time(0);
        char dt[30];
        ctime_s(dt, sizeof(dt), &now);
        string dateStr(dt); 
        dateStr.pop_back(); 

        file << dateStr << " | ID: " << idTrans 
             << " | Promo: " << (diskonCode.empty() ? "-" : diskonCode)
             << " | Total: " << (long)total << endl;
        file.close();
    }
}

void updateProdukTerlaris() {
    vector<Produk> sortedInv = inventaris; 
    sort(sortedInv.begin(), sortedInv.end(), [](const Produk &a, const Produk &b) {
        return a.terjual > b.terjual;
    });

    ofstream file("produk_terlaris.txt");
    if (file.is_open()) {
        file << "=== PRODUK TERLARIS ===\n";
        for (const auto &p : sortedInv) {
            if (p.terjual > 0) file << p.nama << " - Terjual: " << p.terjual << "\n";
        }
        file.close();
    }
}

void prosesTransaksi() {
    vector<ItemKeranjang> keranjang;
    string inputID;
    int qty;
    char lanjut;
    double totalBelanja = 0;

    cout << "\n=== TRANSAKSI BARU ===" << endl;
    
    do {
        cout << "Masukkan ID Produk (contoh: P001): ";
        cin >> inputID;

        Produk* p = cariProduk(inputID);

        if (p != nullptr) {
            cout << "Produk: " << p->nama << " | Harga: "; printRupiah(p->harga);
            cout << " | Stok: " << p->stok << endl;
            
            cout << "Jumlah beli: ";
            // --- PERBAIKAN: VALIDASI INPUT ANGKA ---
            while(!(cin >> qty)) {
                cout << ">> Error: Masukkan angka yang valid!\n";
                cout << "Jumlah beli: ";
                clearInputBuffer(); // Bersihkan input huruf yang nyangkut
            }

            if (qty > 0 && qty <= p->stok) {
                double sub = p->harga * qty;
                keranjang.push_back({p, qty, sub});
                totalBelanja += sub;
                p->stok -= qty; 
                p->terjual += qty;
                cout << ">> Berhasil ditambahkan.\n";
            } else {
                cout << ">> Stok kurang atau jumlah tidak valid!\n";
            }
        } else {
            cout << ">> Produk tidak ditemukan!\n";
        }

        cout << "Tambah barang lain? (y/n): ";
        cin >> lanjut;
        // Jaga-jaga jika user mengetik panjang (misal "yes"), ambil huruf pertama saja
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    } while (lanjut == 'y' || lanjut == 'Y');

    if (keranjang.empty()) {
        cout << "Transaksi dibatalkan (keranjang kosong).\n";
        return;
    }

    // --- Diskon ---
    string kodePromo;
    double potongan = 0;
    cout << "\nMasukkan Kode Promo (jika ada, ketik '-'): ";
    cin >> kodePromo; // string aman menerima apa saja

    if (kodePromo == "DISKON10") {
        potongan = totalBelanja * 0.10;
        cout << ">> Diskon 10% applied.\n";
    } else if (kodePromo == "HEMAT5000" && totalBelanja >= 50000) {
        potongan = 5000;
        cout << ">> Potongan Rp 5.000 applied.\n";
    }

    double totalAkhir = totalBelanja - potongan;
    string transID = generateTransactionID();

    // --- Cetak Struk ---
    cout << "\n===================================" << endl;
    cout << "           STRUK BELANJA           " << endl;
    cout << "ID: " << transID << endl;
    cout << "===================================" << endl;
    for (auto &item : keranjang) {
        cout << left << setw(15) << item.produk->nama 
             << "x" << item.jumlah 
             << right << setw(10) << (long)item.subtotal << endl;
    }
    cout << "-----------------------------------" << endl;
    cout << "Total       : "; printRupiah(totalBelanja); cout << endl;
    cout << "Diskon      : "; printRupiah(potongan); cout << endl;
    cout << "TOTAL BAYAR : "; printRupiah(totalAkhir); cout << endl;
    cout << "===================================" << endl;

    simpanLaporan(transID, totalAkhir, kodePromo);
    updateProdukTerlaris();
}

void tampilkanMenu() {
    cout << "\n--- MENU KASIR C++ ---" << endl;
    cout << "1. Lihat Daftar Produk" << endl;
    cout << "2. Transaksi Baru" << endl;
    cout << "3. Keluar" << endl;
    cout << "Pilihan: ";
}

int main() {
    int pilihan;
    do {
        tampilkanMenu();
        // --- PERBAIKAN: VALIDASI MENU ---
        if (!(cin >> pilihan)) {
            cout << ">> Error: Masukkan angka 1-3!\n";
            clearInputBuffer(); // Bersihkan jika user input huruf
            pilihan = 0; // Set ke 0 agar loop berlanjut
            continue;
        }

        switch (pilihan) {
        case 1:
            cout << "\n--- DAFTAR PRODUK ---" << endl;
            for (const auto &p : inventaris) {
                cout << "[" << p.id << "] " << left << setw(15) << p.nama 
                     << " Stok: " << setw(3) << p.stok 
                     << " Harga: "; printRupiah(p.harga); cout << endl;
            }
            break;
        case 2:
            prosesTransaksi();
            break;
        case 3:
            cout << "Sistem dimatikan..." << endl;
            break;
        default:
            cout << "Pilihan tidak valid." << endl;
        }
    } while (pilihan != 3);

    return 0;
}