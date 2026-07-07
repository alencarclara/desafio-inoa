#include <iostream>
#include <string>
#include <fstream>
#include <vector>

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

    ifstream arqEntrada("config.txt");

    string linha;
    vector<string> chaves(5);
    vector<string> valores(5);

    string email;
    string smtp;
    string usuario;
    string senha;
    int porta;
    int i = 0;

    if (!arqEntrada.is_open()) // verificação sugerida por IA
    {
        cout << "Erro ao abrir config.txt" << endl;
        return 1;
    }

    while (getline(arqEntrada, linha))
    {
        if (!linha.empty())
        {
            cout << linha << endl;
            size_t pos = linha.find('=');
            chaves[i] = linha.substr(0, pos);
            valores[i] = linha.substr(pos + 1);

            if (chaves[i] == "email_destino")
            {
                email = valores[i];
                i++;
            }

            else if (chaves[i] == "smtp_server")
            {
                smtp = valores[i];
                i++;
            }

            else if (chaves[i] == "smtp_port")
            {
                porta = stoi(valores[i]);
                i++;
            }

            else if (chaves[i] == "usuario")
            {
                usuario = valores[i];
                i++;
            }

            else if (chaves[i] == "senha")
            {
                senha = valores[i];
                i++;
            }
        }
    }
}