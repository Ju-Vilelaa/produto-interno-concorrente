#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define LOG
//recebe os 2 vetores e suas dimensões
void gera_vetores(float *vet1, float *vet2, int N){
        srand(time(NULL));
        for(int i = 0; i < N; i++){
            vet1[i] = (float)rand() / (float) RAND_MAX;
            vet2[i] = (float)rand() / (float) RAND_MAX;
        }
    }

float produto_interno_seq( float *vet1, float *vet2, int N){
    float produto_interno = 0.0;
    for(int i = 0; i <N; i++){
        produto_interno += vet1[i] *vet2[i];
    }
    return produto_interno;
}


int main(int argc, char *argv[]){
    long int N;
    FILE *arquivo_binario;
    float produto_interno;

    //valida a entrada
    if(argc < 2){
        printf("Digite <%s> <dimensao do vetor> <nome arquivo> \n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    char *nome_arquivo = argv[2];

    //aloca vetores
    float *vet1 = (float*) malloc(sizeof(float) * N);
    float *vet2 = (float*) malloc(sizeof(float) * N);

    //testa alocacao dos vetores
    if(vet1 == NULL || vet2 == NULL){
        printf("Erro de alocacao de memoria\n");
        return 2;
    }

    //gera vetores de entrada
    gera_vetores(vet1, vet2, N);
    produto_interno = produto_interno_seq(vet1, vet2, N);

    //abre arquivo de escrita binaria
    arquivo_binario = fopen(nome_arquivo, "wb");
    if(arquivo_binario == NULL){
        printf("ERRO ao abrir o arquivo");
        return 3;
    }

    #ifdef LOG
   fprintf(stdout, "%ld\n", N);
   for(long int i=0; i<N; i++) {
      fprintf(stdout, "%f ",vet1[i]);
   }
   for(long int i=0; i<N; i++) {
      fprintf(stdout, "%f ",vet2[i]);
   }
   fprintf(stdout, "\n");
   fprintf(stdout, "%lf\n", produto_interno);
   #endif

    //Escreve o valor de N no arquivo
    //fwrite(ptr para o array de elementos a serem escritos, tamanho de cada elemento, numero de elementos, ponteiro para o FILE onde serao gravados)

    if(fwrite(&N, sizeof(long int), 1, arquivo_binario) != 1){
        printf("Erro de escrita no arquivo\n");
        return 4;
    }

    //Escreve o primeiro vetor
    if(fwrite(vet1, sizeof(float), N, arquivo_binario) !=N){
        printf("Erro de escrita do primeiro vetor no arquivo\n");
        return 5;
    }

    //Escreve o segundo vetor
    if(fwrite(vet2, sizeof(float), N, arquivo_binario) !=N){
        printf("Erro de escrita do segundo vetor no arquivo\n");
        return 6;
    }

    //Escreve resultado produto interno no arquivo
     if(fwrite(&produto_interno, sizeof(float), 1, arquivo_binario) !=1){
        printf("Erro de escrita do resultado no arquivo\n");
        return 7;
    }

    fclose(arquivo_binario);
    free(vet1);
    free(vet2);

    printf("Dados salvos com sucesso no arquivo %s\n", nome_arquivo);


    return 0;
}