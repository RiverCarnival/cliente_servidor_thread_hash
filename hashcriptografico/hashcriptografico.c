/*
INSTALAR COMO ROOT
apt-get install libssl-dev coreutils

LEMBRAR DE ADIOCNAR A COMPILACAO DO GCC
-lcrypto

SE QUISER COMPARAR NA LINHA DE COMANDO
echo -n "texto" | sha512sum
*/

#include <stdio.h>
#include <openssl/sha.h> // NECESSARIO PARA USAR A FUNCAO SHA512(texto, tamanhotexto, NULL)
#include <string.h>

int 
main(int argc, char **argv) 
  {
  if(argc != 2) 
    {
    printf("Usage: %s <string>\n", argv[0]);
    return (1);
    }
  printf("INPUT: %s\n", argv[1]);
//  unsigned char *hash = SHA512(argv[1], strlen(argv[1]), NULL);
  unsigned char *hash = SHA512("12", 2, NULL);
  printf("SHA256: ");
  for (int i = 0; i < 64; i++) 
    {
    printf("%02x", hash[i]);
    }
  printf("\n");
  return 0;
  }
