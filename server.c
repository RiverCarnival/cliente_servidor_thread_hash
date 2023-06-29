#include <stdio.h> // printf sprintf
#include <stdlib.h> // exit 
#include <string.h> // strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h> // inet_pton AF_INET
#include <unistd.h> // read 
#include <pthread.h> // lib para usar thread
#include <openssl/sha.h> // lib do hash criptgrafico
#define PORT 8080 // porta ao qual irá ouvir
//gcc  server.c -o server -lpthread -lssl -lcrypto


typedef struct // estrutura do arquivo, onde fica o hash e o nome do arquivo
{ 
    char filename[100]; 
    char hash[SHA256_DIGEST_LENGTH * 2 + 1];
} DataStructure;

void *connection_handler(void *socket_desc) //estrutura
{  
    int sock = *(int *)socket_desc;
    DataStructure data;
    
    // Preencher os dados da estrutura com o arquivo e seu hash
    strcpy(data.filename, "arquivo.txt");

    FILE *file = fopen(data.filename, "rb");
    if (file == NULL) 
    {
        printf("falha ao abrir o arquivo\n");
        close(sock);
        pthread_exit(NULL);
    }

    // calcular o hash do arquivo
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256); // inicia o calculo do Hash
    unsigned char buffer[1024];
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file))) 
    {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(hash, &sha256);

    fclose(file);

    // converter o hash para uma string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) 
    { // percorre o hash gerado anteriormente 
        sprintf(data.hash + (i * 2), "%02x", hash[i]); //converte em string
    }
    //data.hash[SHA256_DIGEST_LENGTH * 2] = '\0';

    // envia os dados da estrutura para o cliente
    if (send(sock, &data, sizeof(DataStructure), 0) < 0) 
    {
        printf("falha ao enviar dados para o cliente\n");
    }

    close(sock); // fecha o socket
    pthread_exit(NULL); // fecha as thred
}

int main() {
    int server_fd, new_socket, opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // cria o socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("falha ao criar socket");
        exit(EXIT_FAILURE);
    }

    // defini opções do socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // atribuir o endereço ao socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("falha ao atribuir endereco ao socket");
        exit(EXIT_FAILURE);
    }

    // ouvir por conexoes
    if (listen(server_fd, 3) < 0) 
    {
        perror("falha ao ouvir por conexoes");
        exit(EXIT_FAILURE);
    }

    printf("servidor aguardando conexoes...\n");

    while (1) 
    {
        // aceitar conexão
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) 
        {
            perror("falha ao aceitar conexao");
            exit(EXIT_FAILURE);
        }

        // uma nova thread
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, connection_handler, (void *)&new_socket) < 0) 
        {
            perror("falha ao criar thread");
            exit(EXIT_FAILURE);
        }

        printf("conexao aceita thread atribuida: %ld\n", thread_id);
    }

    return 0;
}
