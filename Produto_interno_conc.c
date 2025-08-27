#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "timer.h"
#include <math.h>


float *vet1, *vet2;

typedef struct {
    //numero de threads
    short int nthreads;
    //id das threads
    short int id;
    //tamanho do vetor
    long int N;
} t_args;

//funcao de produto interno
void *Produto_interno_conc(void *arg){
    int ini, fim, fatia;
    t_args *dados = (t_args *) arg;
    float soma = 0.0, *retorno;

    fatia = dados->N / dados->nthreads;
    ini = dados->id * fatia;
    fim = ini + fatia;

    for(int i = ini; i< fim; i++){
        soma += vet1[i] * vet2[i];
    }

    retorno = (float *) malloc(sizeof(float));
    if(retorno != NULL) *retorno = soma;
    else printf("ERRO: malloc()\n");

    pthread_exit((void *) retorno);
}

//funcao principal
int main(int argc, char *argv[]){
    long int N; //tamanho vetor
    short int nthreads; //numero de threads

    FILE *arquivo_entrada; //arquivo de entrada
    size_t ret; //retorno da funcao de leitura no arquivo de entrada
    float produto_interno; //resultado do produto interno registrado no arquivo

    float produto_interno_conc; //resultado do produto interno concorrente
    float *produto_interno_retorno_threads; //auxiliar para retorno das threads

    pthread_t *tid_sistema; //vetor de identificadores das threads no sistema

    double start, finish; // variáveis para medir o tempo de execução

    //validacao argumentos de entrada
    if(argc <3){
        printf("Use: %s <arquivo_entrada> <numero_threads>\n", argv[0]);exit(-1);
    }
    
    //abre o arquivo com os vetores
    arquivo_entrada = fopen(argv[1], "rb");
    if(arquivo_entrada==NULL) { printf("--ERRO: fopen()\n"); exit(-1); }

    //le o tamanho dos vetores
    ret = fread(&N, sizeof(long int), 1, arquivo_entrada);
    if(!ret) {
     fprintf(stderr, "Erro de leitura das dimensoes do vetor\n");
     return 3;
    }

    //alocando espaco na memoria para vetores de entrada
    vet1 = (float*) malloc (sizeof(float) * N);
    vet2 = (float*) malloc (sizeof(float) * N);

   ret = fread(vet1, sizeof(float), N, arquivo_entrada);
   ret = fread(vet2, sizeof(float), N, arquivo_entrada);

    if(ret < N) {
        fprintf(stderr, "Erro de leitura dos vetores\n");
        return 4;
    }

    nthreads = atoi(argv[2]);
    if(nthreads > N) nthreads = N;

    tid_sistema = (pthread_t *) malloc (sizeof(pthread_t) * nthreads);
    if(tid_sistema == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria\n");
        return 5;
    }

    GET_TIME(start);
    for(long int i = 0; i < nthreads; i++){
        t_args *dados = (t_args*) malloc(sizeof(t_args));  
        if (dados == NULL) {
            fprintf(stderr, "Erro de alocacao de memoria\n");
            return 6;
        }

        dados->nthreads = nthreads;
        dados->id = i;
        dados->N = N;

        if(pthread_create(&tid_sistema[i], NULL, Produto_interno_conc, (void*)dados)){
            fprintf(stderr, "--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    //espera todas a threads terminarem e calcula o produto interno 
    produto_interno_retorno_threads = 0;
    for(int i=0; i<nthreads; i++) {
     if (pthread_join(tid_sistema[i], (void *) &produto_interno_retorno_threads)) {
        printf("--ERRO: pthread_join()\n"); exit(-1);
     }
     produto_interno_conc+= *produto_interno_retorno_threads;

     free(produto_interno_retorno_threads);
  }
    GET_TIME(finish);

    double elapsed = finish - start;

    printf("Tempo de execucao concorrente: %f segundos\n", elapsed);

  //imprime resultados
  printf("produto_interno_conc             = %.26f\n", produto_interno_conc);

  //le o somatorio registrado no arquivo
  ret = fread(&produto_interno, sizeof(float), 1, arquivo_entrada); 

  float variacao_relativa = (produto_interno - produto_interno_conc) / produto_interno;

  printf("\nproduto_interno                    = %.26lf\n", produto_interno);

  printf("variacao_relativa                  = %.6f\n", variacao_relativa);


    free(vet1);
    free(vet2);
    free(tid_sistema);
    fclose(arquivo_entrada);

    return 0;
}




