//Шифр Magma(ГОСТ 28147-89) Произвольный текст, 32 раунда, сеть фейстеля, нелинейное преобразование, побитовые циклические сдвиги

#include<iostream>
#include<fstream>
#include<bitset>
#include<vector>

using namespace std;


//таблица замен(нелинейное биективное преобразование)
static unsigned char Pi[8][16] =
{
    {12,4,6,2,10,5,11,9,14,8,13,7,0,3,15,1},
    {6,8,2,3,9,10,5,12,1,14,4,7,11,13,0,15},
    {11,3,5,8,2,15,10,13,14,1,7,4,12,9,6,0},
    {12,8,2,1,13,4,15,6,7,0,10,5,3,14,9,11},
    {7,15,5,10,8,1,6,13,0,9,3,14,11,4,2,12},
    {5,13,15,6,9,2,12,10,11,7,8,1,4,3,14,0},
    {8,14,2,5,6,9,1,12,15,4,11,0,13,10,3,7},
    {1,7,14,13,0,5,8,3,4,15,10,6,9,12,11,2}
};

typedef uint8_t vect[4]; //блок размером 32 бита

vect iter_key[32]; //итерационные ключи шифрования

//двоичное представление символа
string toBinary(uint8_t value) {
    return bitset<8>(value).to_string();
}

void print_binary_mess(uint8_t* a, int size)
{
    for (int i = 0; i < size; i++) {
        cout << toBinary(a[i]) << " ";
    }
    cout << endl;
}

void print_mess(uint8_t* a, int size)
{
    for (int i = 0; i < size; i++) {
        cout << a[i] << " ";
    }
    cout << endl;
}
//Функция для перестановок
void GOST_Magma_T(uint8_t* in_data, uint8_t* out_data)
{
    uint8_t first_part_byte, sec_part_byte;
    for (int i = 0; i < 4; i++)
    {
        first_part_byte = (in_data[i] & 0x0f);
        sec_part_byte = (in_data[i] & 0xf0) >> 4;
        first_part_byte = Pi[i * 2][first_part_byte];
        sec_part_byte = Pi[i * 2 + 1][sec_part_byte];
        out_data[i] = (sec_part_byte << 4) | first_part_byte;
    }
}
//Функция сложения по модулю 2
void GOST_Magma_XOR_2(uint8_t* a, uint8_t* b, uint8_t* c)
{
    for (int i = 0; i < 4; i++)
        c[i] = a[i] ^ b[i];
}
//Функция сложения по модуля 2 в степени 32
void GOST_Magma_XOR_2deg32(uint8_t* a, uint8_t* b, uint8_t* c)
{
    unsigned int internal = 0;
    for (int i = 0; i < 4; i++)
    {
        internal = a[i] + b[i] + (internal >> 8);
        c[i] = internal & 0xff;
    }
}
//Функция, включающая сложение итерационного ключа и левого подблока по модулю 2 в степени 32, нелинейное биективное преобразование и циклический сдвиг
void GOST_Magma_g(uint8_t* k, uint8_t* a, uint8_t* out_data)
{
    uint8_t internal[4];
    uint32_t out_data_32;
    GOST_Magma_XOR_2deg32(a, k, internal);
    GOST_Magma_T(internal, internal);
    out_data_32 = internal[3];
    out_data_32 = (out_data_32 << 8) + internal[2];
    out_data_32 = (out_data_32 << 8) + internal[1];
    out_data_32 = (out_data_32 << 8) + internal[0];
    out_data_32 = (out_data_32 << 11) | (out_data_32 >> 21);
    out_data[0] = out_data_32;
    out_data[1] = out_data_32 >> 8;
    out_data[2] = out_data_32 >> 16;
    out_data[3] = out_data_32 >> 24;
}
//Функция сложения результата g и правого подблока по модулю 2 и итогового формирования нового левого и правого подблока
void GOST_Magma_G(uint8_t* k, uint8_t* a, uint8_t* out_data, bool f)
{
    uint8_t a_0[4];
    uint8_t a_1[4];
    uint8_t G[4];
    int i;
    for (i = 0; i < 4; i++)
    {
        a_1[i] = a[4 + i];
        a_0[i] = a[i];
    }
    GOST_Magma_g(k, a_0, G);
    GOST_Magma_XOR_2(a_1, G, G);
    if (f == true)
    {
        for (i = 0; i < 4; i++)
        {
            a_1[i] = a_0[i];
            a_0[i] = G[i];
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
            a_1[i] = G[i];
    }
    for (i = 0; i < 4; i++)
    {
        out_data[i] = a_0[i];
        out_data[4 + i] = a_1[i];
    }

}
//Функция для разбиения ключа на восемь подключей
vector<vector<uint8_t>> split_key(uint8_t* key)
{
    vector<vector<uint8_t>> part_of_key(8, vector<uint8_t>(4, 0));
    int N = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            part_of_key[i][j] = key[N + j];
        }
        N += 4;
    }
    return part_of_key;
}
//Функция для определение итерационных(раундовых) ключей
void GOST_Magma_Expand_Key(uint8_t* key)
{
    vector<vector<uint8_t>> sp_key = split_key(key);
    uint8_t sp_key_arr[8][4];
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++) sp_key_arr[i][j] = sp_key[i][j];
    }
    for (int i = 0; i < 24; i++)
    {
        memcpy(iter_key[i], sp_key_arr[7 - i % 8], 4);
    }
    for (int i = 24; i < 32; i++)
    {
        memcpy(iter_key[i], sp_key_arr[i % 8], 4);
    }
    printf("Раундовые ключи:\n");
    int i;
    for (i = 0; i < 32; i++)
    {
        for (int j = 0; j < 4; j++) cout << iter_key[i][j] << " ";
        cout << endl;
    }
}
//Функция для шифрования блока
void GOST_Magma_Encrypt(uint8_t* blk, uint8_t* out_blk)
{
    GOST_Magma_G(iter_key[0], blk, out_blk, true);
    for (int i = 1; i < 31; i++)
        GOST_Magma_G(iter_key[i], out_blk, out_blk, true);
    GOST_Magma_G(iter_key[31], out_blk, out_blk, false);
}
//Функция для расшифровки блока
void GOST_Magma_Decrypt(uint8_t* blk, uint8_t* out_blk)
{
    GOST_Magma_G(iter_key[31], blk, out_blk, true);
    for (int i = 30; i > 0; i--)
        GOST_Magma_G(iter_key[i], out_blk, out_blk, true);
    GOST_Magma_G(iter_key[0], out_blk, out_blk, false);
}

void GOST_Magma_XOR_2_blk(uint8_t* a, uint8_t* b, uint8_t* c)
{
    for (int i = 0; i < 8; i++)
        c[i] = a[i] ^ b[i];
}
//В главной функции реализовано шифрование и дешифрование сообщения
int main()
{
    setlocale(LC_ALL, "Russian");
    ifstream in1("key.txt");
    ifstream in2("test.txt");
    int size;
    in2 >> size;
    uint8_t* mess = new uint8_t[size];
    for (int i = 0; i < size; i++)
    {
        uint8_t a;
        in2 >> a;
        mess[i] = a;
        cout << mess[i] << " ";
    }
    cout << endl;
    uint8_t key[32];
    for (int i = 0; i < 32; i++)
    {
        uint8_t a;
        in1 >> a;
        key[i] = a;
        cout << key[i] << " ";
    }
    cout << endl;
    GOST_Magma_Expand_Key(key);
    int length = 0;
    if (size % 8 != 0) length = size + (8 - size % 8);
    else length = size;
    cout << "Длина дополненного сообщения:" << endl;
    cout << length << endl;
    cout << "Бинарный вид сообщения:" << endl;
    print_binary_mess(mess, size);
    uint8_t* new_mess = new uint8_t[length];
    uint8_t* hash_mess = new uint8_t[length];
    uint8_t* error_mess = new uint8_t[length];
    for (int i = 0; i < size; i++)
    {
        new_mess[i] = mess[i];
        hash_mess[i] = mess[i];
        error_mess[i] = mess[i];
    }
    error_mess[0] = 64;
    //Заполняем нулевыми байтами последний блок
    if (length > size)
    {
        new_mess[size] = 0;
        hash_mess[size] = 128; // для хэш функции дополнение последнего блока начинается с 10000000
        error_mess[size] = 128;
        for (int i = size + 1; i < length; i++)
        {
            hash_mess[i] = 0;
            new_mess[i] = 0;
            error_mess[i] = 0;
        }
    }
    cout << "Бинарный вид дополненного сообщения:" << endl;
    print_binary_mess(new_mess, length);
    cout << "Бинарный вид изменненого сообщения на 1 бит:" << endl;
    print_binary_mess(error_mess, length);
    int block_num = length / 8;
    uint8_t* enc_mess = new uint8_t[length];
    uint8_t* dec_mess = new uint8_t[length];
    uint8_t hash_value[8];
    uint8_t err_hash_value[8];
    for (int i = 0; i < 8; i++)
    {
        hash_value[i] = rand() % 205 + 50;
    }
    for (int i = 0; i < block_num; i++)
    {
        uint8_t blk[8];
        uint8_t blk_hash[8];
        uint8_t err_blk_hash[8];
        for (int j = 0; j < 8; j++)
        {
            blk[j] = new_mess[j + 8 * i];
            blk_hash[j] = hash_mess[j + 8 * i];
        }
        if (i == 0)
        {
            GOST_Magma_Encrypt(blk_hash, hash_value);
            GOST_Magma_Encrypt(blk_hash, err_hash_value);
        }
        else
        {
            GOST_Magma_XOR_2_blk(hash_value, blk_hash, blk_hash);
            GOST_Magma_Encrypt(blk_hash, hash_value);
            GOST_Magma_XOR_2_blk(err_hash_value, err_blk_hash, err_blk_hash);
            GOST_Magma_Encrypt(err_blk_hash, err_hash_value);
        }
        for (int i = 0; i < 8; i++) printf("%02x", hash_value[i]);
        printf("\n");
        uint8_t out_blk[8];
        GOST_Magma_Encrypt(blk, out_blk);
        for (int k = 0; k < 8; k++)
        {
            enc_mess[k + 8 * i] = out_blk[k];
        }
        GOST_Magma_Decrypt(out_blk, out_blk);
        for (int k = 0; k < 8; k++)
        {
            dec_mess[k + 8 * i] = out_blk[k];
        }
    }
    cout << "Зашифрованное сообщение:" << endl;
    printf("%s\n", enc_mess);
    print_binary_mess(enc_mess, length);
    for (int i = 0; i < length; i++) printf("%02x", enc_mess[i]);
    printf("\n");
    cout << "Расшифрованное сообщение:" << endl;
    printf("%.*s\n", size, dec_mess);
    print_binary_mess(dec_mess, size);
    for (int i = 0; i < size; i++) printf("%02x", dec_mess[i]);
    printf("\n");
    cout << "Полученное хэш-значение для данного сообщения:" << endl;
    for (int i = 0; i < 8; i++) printf("%02x", hash_value[i]);
    printf("\n");
    cout << "Полученное хэш-значение для измененного на 1 бит сообщения:" << endl;
    for (int i = 0; i < 8; i++) printf("%02x", err_hash_value[i]);
    printf("\n");
    delete[] mess;
    delete[] new_mess;
    delete[] enc_mess;
    delete[] dec_mess;
    delete[] hash_mess;
    delete[] error_mess;
    return 0;
}