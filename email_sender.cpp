// Baseado na documentação do libcurl

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <string>
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

enum class Alert
{
    Buy,
    Sell
};

struct upload_status
{
    string *payload;
    size_t bytes_read;
};

static size_t read_cb(char *ptr, size_t size, size_t nmemb, void *userp)
{
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    size_t room = size * nmemb;

    if ((size == 0) || (nmemb == 0) || (room < 1))
    {
        return 0;
    }

    if (upload_ctx->bytes_read >= upload_ctx->payload->size())
    {
        return 0;
    }

    size_t len = upload_ctx->payload->size() - upload_ctx->bytes_read;

    if (room < len)
    {
        len = room;
    }

    memcpy(ptr, upload_ctx->payload->c_str() + upload_ctx->bytes_read, len);
    upload_ctx->bytes_read += len;

    return len;
}

int sendEmail(const string &senderEmail, const string &recipientEmail, const string &senderPassword, const string &ticker, const string &smtpUrl, Alert alert, double precoAtual)
{
    stringstream streamPreco;
    streamPreco << fixed << setprecision(2) << precoAtual;
    string precoFormatado = streamPreco.str();

    string dataBuy =
        "To: " + recipientEmail + "\r\n"
        "From: " + senderEmail + "\r\n"
        "Subject: Alerta de COMPRA de " + ticker + "\r\n"
        "\r\n"
        "A cotacao de " + ticker + " da B3 caiu para R$ " + precoFormatado + " e atingiu seu preco alvo de compra!\r\n";

    string dataSell =
        "To: " + recipientEmail + "\r\n"
        "From: " + senderEmail + "\r\n"
        "Subject: Alerta de VENDA de " + ticker + "\r\n"
        "\r\n"
        "A cotacao de " + ticker + " da B3 subiu para R$ " + precoFormatado + " e atingiu seu preco alvo de venda!\r\n";

    CURL *curl;

    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK)
        return (int)result;

    curl = curl_easy_init();

    if (!curl)
    {
        cerr << "Erro ao inicializar CURL para o email.\n";
        return 1;
    }

    if (curl)
    {
        struct curl_slist *recipients = NULL;
        struct upload_status upload_ctx = {0};

        curl_easy_setopt(curl, CURLOPT_USERNAME, senderEmail.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, senderPassword.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());

        recipients = curl_slist_append(recipients, recipientEmail.c_str());

        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_cb);

        upload_ctx.bytes_read = 0;

        if (alert == Alert::Buy)
        {
            upload_ctx.payload = &dataBuy;
        }
        else
        {
            upload_ctx.payload = &dataSell;
        }

        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);

        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        result = curl_easy_perform(curl);

        if (result != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(result));

        curl_slist_free_all(recipients);

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return (int)result;
}