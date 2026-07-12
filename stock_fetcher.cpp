#define WIN32_LEAN_AND_MEAN
#include "stock_fetcher.hpp"
#include "include/nlohmann/json.hpp"
#include <curl/curl.h>
#include <iostream>

using namespace std;
using json = nlohmann::json;

// Função auxiliar obrigatória do cURL para salvar a resposta da internet na memória
size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output)
{
    output->append((char *)contents, size * nmemb);
    return size * nmemb;
}

optional<double> fetchStockPrice(const string &ticker)
{
    const string api_key = "06H0PJ5H60NBSD0R";
    const string url = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=" + ticker + "&apikey=" + api_key;

    CURL *curl = curl_easy_init();
    string readBuffer;
    optional<double> quotePrice;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            try
            {
                json j = json::parse(readBuffer);

                // Verifica se a API retornou um erro ou limite de uso
                if (j.contains("Information") || j.contains("Note"))
                {
                    cerr << "Aviso da API: Limite de requisições atingido ou erro." << endl;
                    return nullopt;
                }

                // Extrai o preço da resposta GLOBAL_QUOTE
                string price_str = j["Global Quote"]["05. price"];
                quotePrice = stod(price_str);
            }
            catch (const exception &e)
            {
                cerr << "Erro ao processar os dados da bolsa: " << e.what() << endl;
            }
        }
        else
        {
            cerr << "Erro de conexao cURL: " << curl_easy_strerror(res) << endl;
        }
    }
    return quotePrice;
}