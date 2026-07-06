#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Uso: programa <ativo a ser monitorado> <preco venda> <preco compra>" << endl;
        return 1;
    }

    string ativo = argv[1];
    double preco_venda = stod(argv[2]);
    double preco_compra = stod(argv[3]);
}