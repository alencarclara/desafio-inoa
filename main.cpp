#define NOMINMAX
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "include/nlohmann/json.hpp"
#include "stock_fetcher.hpp"
#include <thread>
#include <chrono>

using namespace std;

// Avisa o compilador sobre as opções de alerta
enum class Alert
{
    Buy,
    Sell
};

// "Forward declaration": avisa o main que esta função existe noutro ficheiro
int sendEmail(const std::string &senderEmail, const std::string &recipientEmail,
              const std::string &senderPassword, const std::string &ticker,
              Alert alert, const std::string &smtpUrl);

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

    cout << "Configuracoes carregadas! Iniciando monitoramento de " << ativo << "..." << endl;

    while (true)
    {
        optional<double> preco_atual = fetchStockPrice(ativo);

        if (preco_atual.has_value())
        {
            cout << "Cotacao atual de " << ativo << ": R$ " << preco_atual.value() << endl;

            if (preco_atual.value() <= preco_compra)
            {
                cout << "ALERTA: O preco caiu para R$" << preco_atual.value() << "! Hora de COMPRAR." << endl;
            }
            else if (preco_atual.value() >= preco_venda)
            {
                cout << "ALERTA: O preco subiu para R$" << preco_atual.value() << "! Hora de VENDER." << endl;
            }
        }

        // espera 1 minuto para nao estourar o limite da API -> sugestão da IA
        this_thread::sleep_for(chrono::minutes(1));
    }
}