#include <stdio.h> // printf sprintf
#include <stdlib.h> // exit 
#include <string.h> // strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h> // inet_pton AF_INET
#include <unistd.h> // read 
#include <openssl/sha.h> // lib do hash criptgrafico
//gcc cliente.c -o cliente -lssl -lcrypto


#define PORT 8080 // porta que iŕa ouvir

typedef struct // estrutura do arquivo, onde fica o hash e o nome do arquivo
{
    char filename[100];
    char hash[SHA256_DIGEST_LENGTH * 2 + 1];
} DataStructure;

int main() 
{
    int sock; // valor do socket
    struct sockaddr_in server_addr;
    char server_ip[] = "127.0.0.1"; // endereço do servidor
    DataStructure data; // armazena o que é recebido do servidor

    // cria o socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        printf("falha ao criar socket\n");
        return 0; // finaliza o programa caso de erro
    }

    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT); // converte para o formato de rede

    // converte o endereco IP para o formato correto
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) 
    {
        printf("endereco errado\n");
        return 0; // caso o endereço esteja errado encerra o programa
    }

    // conectar ao servidor
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        printf("falha na conexao\n");
        return 0; //caso nao consiga conectar ao servidor encerra a conecxao
    }

    // eecebe os dados do servidor
    if (recv(sock, &data, sizeof(DataStructure), 0) < 0) 
    {
        printf("falha ao receber dados do servidor\n");
        return 0;
    }

    // verifica o hash recebido
    FILE *file = fopen(data.filename, "rb"); // rb é para leitura binaria
    if (file == NULL) 
    {
        printf("falha ao abrir o arquivo\n");
        close(sock);
        return 0;
    }

    // Calcular o hash do arquivo recebido
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char buffer[1024];
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file))) // le as partes recebidas
    {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(hash, &sha256); // finaliza o calculo do hash e armazena no vetor

    fclose(file); // fecha o arquivo

    // converte o hash para uma string legivel
    char receivedHash[SHA256_DIGEST_LENGTH * 2 + 1]; //armazena o hash recebido
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) 
    {
        sprintf(receivedHash + (i * 2), "%02x", hash[i]); // converte para exadecimal 
    }
    //receivedHash[SHA256_DIGEST_LENGTH * 2] = '\0';

    // Compara o hash recebido com o hash calculado
    if (strcmp(data.hash, receivedHash) == 0) 
    {
        printf("O hash recebido e correspondente ao do servidor\n");
    } else {
        printf("O hash recebido não é correspondente ao do servidor\n");
    }

    close(sock);

    return 0;
}
