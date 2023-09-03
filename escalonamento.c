/*
Nome: Rebeca Madi Oliveira
RA: 22153168
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    char PID[5]; // PID
    int timeIn; // tempo de chegada
    int exec; // tempo de execução
    int prio; // prioridade
    int type; // tipo
    int wait; // tempo de espera do processo
    int run; // tempo executando 
    int tr; //tempo de chegada
}Processo;

typedef struct l{
    Processo* p;
    int t;
    struct l* next;
}Lista;

typedef struct{
    double tt, tw;
}Tempo;

typedef struct{
    int cpu, io, ambos;
}Quantidade;

Processo* novoProcesso(char PID[], int timeIn, int exec, int prio, int type, int wait, int run){
    /*Aloca oo processo*/
    Processo* p = malloc(sizeof(Processo));
    strcpy(p->PID, PID);
    p->exec = exec;
    p->prio = prio;
    p->timeIn = timeIn;
    p->type = type;
    p->wait = wait;
    p->run = run;
    p->tr = timeIn;
    return p;
}

Lista* novaLista(Processo* p, int t){
    /*Aloca a lista*/
    Lista* lista = malloc(sizeof(Lista));
    lista->p = p;
    lista->t = t;
    lista->next = NULL;
    return lista;
}

Lista* inserirLista(Processo* p, Lista* lista, int t){
    /*Insere ordenado por tempo*/
    if(lista == NULL){
        return novaLista(p, t);
    }else{
        if(p->timeIn >= lista->p->timeIn){
            lista->next = inserirLista(p, lista->next, lista->t);
        }else{
            Lista* aux = novaLista(p, t);
            aux->next = lista;
            return aux;
        }
        
        return lista;
    }
}

Lista* inserirFila(Processo* p, Lista* lista, int t){//pilha
    if(lista == NULL){
        return novaLista(p, t);
    }else{
            lista->next = inserirFila(p, lista->next, t);
            return lista;
    }
}

Lista* lerArquivo(FILE* arq, Lista* lista){
    /*Lê o arquivo*/
    int ti, e, prio, type;
    char pid[5];
    while (fscanf(arq, "%s %d %d %d %d", pid, &ti, &e, &prio, &type)!=EOF)
    {
        Processo* p = novoProcesso(pid, ti, e, prio, type, 0, 0);
        lista = inserirLista(p, lista, 0); //Insere ordenado por tempo de chegada
    }
    return lista;
}

Lista* abrirArquivo(char arq[], Lista* lista){
    /*Abre o arquivo*/
    FILE* f;
    f = fopen(arq, "r");
    lista = lerArquivo(f, lista);
    fclose(f);
    return lista;
}

Lista* remover_lista(Lista* lista, Processo* p){
    /*Remove da lista*/
    if(lista==NULL) return NULL;
    else{
        if(lista->p == p){
            Lista* n = lista->next;
            //free(lista);
            return n;
        }else{
            lista->next = remover_lista(lista->next, p);
            return lista;
        }
    }
}

Lista* atualiza_tempo_c(Lista* lista, int t){
    /*Atualiza os tempos do fcfs*/
    if(lista == NULL) return lista;

    lista->t += t;
    lista->p->run += ((lista->p->exec +t) - lista->p->timeIn);
    lista->p->wait += t;
    lista->next = atualiza_tempo_c(lista->next, (lista->t + lista->p->exec));
    return lista;
}

Lista* run_fcfs(Lista* fcfs, Lista* lista){
    /*Execução do fcfs*/
    while (lista!=NULL)
    {
        //A lista está ordenada por ordem de chegada
        fcfs = inserirLista(lista->p, fcfs, 0);//Insere
        lista = remover_lista(lista, lista->p);//Remove
    }
    fcfs = atualiza_tempo_c(fcfs, 0); //Atualiza os tempos de execução e de espera
    return fcfs;
}

void exibe(Lista* lista){
    /*Exibe a fila de processos*/
    if(lista!=NULL){
        Processo* aux = lista->p;printf("%s ", aux->PID);
        exibe(lista->next);
    }else{
        printf("\n");
    }
}

Lista* eliminar(Lista* lista, Processo* p){
    if(lista==NULL) return lista;

    if(lista->p == p){
        lista->p->tr = -1;
    }
    lista->next = eliminar(lista->next, p);
    return lista;
}

Tempo* soma_tempo(Lista* lista, Tempo* tempo){
    /*Soma os tempos de execução e espera*/
    double sum = 0.0, sum2 = 0.0;
    Lista* aux = lista;
        while (aux!=NULL)
        {
            if(aux->p->tr>=0){
                sum += aux->p->run;
                sum2 += aux->p->wait;  
                aux = eliminar(aux, aux->p);
            }
            aux = aux->next;
        }
        
    tempo->tt = sum;
    tempo->tw = sum2;
    return tempo;
}

int cont(int n, Lista* r){
    /*Conta a quantidade de entradas*/
    if(r == NULL){
        return n;
    }else{
        return cont(n+1, r->next);
    }
}

Processo* buscaMenorTempo(Lista* lista, Processo* p, int t){
    /*Retorna o processo com menor tempo no tempo atual*/
    if(lista == NULL){
        if(p->timeIn<=t){
            return p;
        }else{
            return NULL;
        }
    }
    if(lista->p->timeIn<=t){
         if(lista->p->exec<p->exec){
            p = lista->p;
         }
    }
    return buscaMenorTempo(lista->next, p, t);
    
}

Lista* atualiza_espera(Lista*lista, int t, int ta){
    if(lista==NULL) return lista;
    else{
        if(lista->p->tr!=-1 && lista->p->timeIn<=ta){
            lista->p->wait += t;
        }
        lista->next = atualiza_espera(lista->next, t, ta);
        return lista;
    }
}

Lista* run_sjf(Lista* sjf, Lista* lista){
    /*Execução do sjf*/
    int t=0;
    Processo* u=NULL;
    while (lista!=NULL)
    {
              Processo* aux = buscaMenorTempo(lista, lista->p, t); //Busca o processo com menor tempo
              if(aux){ //Se tiver processo no momento
                aux->run = (t+aux->exec)-aux->timeIn;
                aux->wait = t-aux->timeIn;
                sjf = inserirFila(aux, sjf, t);
                t += aux->exec;
                lista = remover_lista(lista, aux);
              }else{ //Se não tiver processo no momento
                 t++;
             }
    }
    return sjf;
}

Processo* chegouPrimeiro(Lista* lista, Processo* p, int t){
    /*Retorna o que chegou primeiro de acordo com o tempo atual*/
    if(lista==NULL){
        if(p->timeIn<=t){
            return p;
        }else{
            return NULL;
        }
    }
    else{
        if(lista->p->timeIn<=t){
            if(lista->p->timeIn==p->timeIn){
                if(lista->p->tr<p->tr){
                    p = lista->p;
                }
            }else if(lista->p->timeIn<p->timeIn){
                p = lista->p;
            }
        }
        return chegouPrimeiro(lista->next, p, t);
    }
}



Lista* run_rr(Lista* rr, Lista* lista, int quantum){
    int t=0, q;
    while (lista!=NULL)
    {
          Processo* p = chegouPrimeiro(lista, lista->p, t); //Pega o primeiro
          if(p){
            rr = inserirFila(p, rr, t); //Insere na fila
            p->wait += t - p->timeIn;
            if((p->exec-quantum)<=0){ //Se o processo estiver no fim
                q = quantum - p->exec;
    
                if(q==0){t+= quantum;}
                else{ t += q;}
    
                p->run = t - p->tr;
                lista = remover_lista(lista, p);
            }else{ //Se o processo ainda não estiver no fim
                p->exec = p->exec - quantum;
                p->timeIn = t + quantum;
                t += quantum;
                q = quantum;
            }
          }else{ //Se ninguem tiver chegado no momento incrementa o tempo
            t++;
          }
    }
    return rr;
}

Lista* run_srtf(Lista* srtf, Lista* lista, int quantum){
  int t=0;

  while(lista!=NULL){
        Processo* aux = buscaMenorTempo(lista, lista->p, t); //busca o processo que tem menor tempo no tempo atual
        if(aux){
            aux->wait += t-aux->timeIn;
            srtf = inserirFila(aux, srtf, t);
            if((aux->exec-quantum)<=0){
                //Se for o fim do processo
                int q = quantum - aux->exec;
                if(q==0){
                    t += quantum;
                }else{
                    t += q;
                }
                aux->exec = 0;
                aux->run = t - aux->tr;
                lista = remover_lista(lista, aux);
            }else{
                //Se o processo ainda continuar
                aux->exec = aux->exec - quantum;
                aux->timeIn = t + quantum;
                t += quantum;
            }
        }else{
          t++;
        }
  }
  return srtf;
}

Processo* maiorPrioridade(Lista* lista, Processo* p, int t){
    /*Retorna o processo de maior prioridade no tempo t*/
    if(lista == NULL){
        if(p->timeIn <= t) return p;
        else return NULL;
    }else{
        if(lista->p->timeIn <= t){
            if(lista->p->prio > p->prio){
                return maiorPrioridade(lista->next, lista->p, t);
            }else{
                return maiorPrioridade(lista->next, p, t);
            }
        }else{
            return maiorPrioridade(lista->next, p, t);
        }
    }
}

Lista* run_prioc(Lista* prioc, Lista* lista){
    int t=0; //tempo
    
    while (lista!=NULL)//Enquanto houver processos na lista
    {
        Processo* p = maiorPrioridade(lista, lista->p, t); //Pega o processo de maior prioridade no tempo t
        if(p!=NULL){ //se não houver processo só incrementa o tempo
            p->wait = t - p->timeIn; //atualiza o tw
            p->run = (t + p->exec) - p->timeIn; //Atualiza tt
            prioc = inserirFila(p, prioc, t); //Insere
            t += p->exec;
            lista = remover_lista(lista, p); //remove
            
        }else{
             t++;
        }
    }
    return prioc;
}

Lista* run_priop(Lista* priop, Lista* lista, int q){
    /*Função de execução do algoritmo de propriedade preemptiva*/
    int t = 0; //tempo atual
    while (lista!=NULL) //Enquanto a lista não é vazia
    {
        Processo* p = maiorPrioridade(lista, lista->p, t); //Verifica o algoritmo de maior prio no tempo atual
        if(p!=NULL){ //se não tiver processo no momento ele irá apenas incrementar o tempo
            if((p->exec - q)>0){ //Se o processo não terminar nessa etapa
                p->exec = p->exec - q; //decrementa a burst time
                p->wait = t - p->timeIn; //atualiza a espera
                p->timeIn += q; //atualiza o tempo de chegada (variavel auxiliar)
                priop = inserirFila(p, priop, t); //Insere na fila de processos
                t += q; //Incrementa o tempo
            }else{ //s[e o processo chegar ao fim
                p->run = (t+p->exec) - p->tr; //Atualiza o tempo de execução
                p->wait = t - p->timeIn; //Atualiza o tempo de espera
                priop = inserirFila(p, priop, t); // Insere na fula
                t +=  p->exec;//Incrementa tempo
                lista = remover_lista(lista, p); //Remove da lista
            }
        }else{
             t++;
        }
    }
    return priop; 
}

Quantidade* contador(Quantidade* q, Lista* lista){
    /*Função que conta a quantidade de tipos para fazer a predição*/
    if(lista == NULL) return q;
    else{
        if(lista->p->type == 1) q->cpu++;
        else if(lista->p->type == 2) q->io++;
        else if(lista->p->type == 3) q->ambos;

        return contador(q, lista->next);
    }
}

int main(int n, char *args[]){
    Lista* lista = NULL; //Lista com os processos de entrada
    Lista* final = NULL; //Lista final de processos
    Tempo* tempo = malloc(sizeof(Tempo)); //Guarda os tempos de execução e de espera

    double tt, tw, c;
    char nome[50];
    strcpy(nome, args[1]);

    Quantidade* quantidade = malloc(sizeof(Quantidade)); //Variavel que guarda os tipos
    quantidade->cpu = 0;
    quantidade->io = 0;
    quantidade->ambos = 0;

    lista = abrirArquivo(nome, lista); //Abre e lê o arquivo
    quantidade = contador(quantidade, lista); //Verifica a quantidade de tipos

    if((quantidade->cpu > quantidade->io) && (quantidade->cpu > quantidade->ambos)){
        printf("Recomendacao de acordo com os tipos dos processos: \nFCFS(Melhor) \nSJF \nSRTF(menos indicado)\n");
    }else if((quantidade->io > quantidade->cpu) && (quantidade->io >quantidade->ambos)){
        printf("Recomendacao de acordo com os tipos dos processos: FCFS e RR");
    }else{
        printf("Recomendacao de acordo com os tipos dos processos: PrioP e PrioC\n");
    }
    printf("Voce pode escolher qualquer um para testar!\n\n");
    printf("Algoritmos de escalonamento: (1)FCFS, (2)SJF, (3)RR, (4)SRTF, (5)PrioC, (6)PrioP\n");
    printf("Escolha um algoritmo: ");
    int op;
    scanf("%d", &op);

    c = (double) cont(0, lista);
    if(op==1){
        final = run_fcfs(final, lista);
        printf("---FCFS---\n");
    }else if(op==2){
        final = run_sjf(final, lista);
        printf("---SJF---\n");
    }else if (op==3){
        int q;
        printf("Informe o quantum: ");
        scanf("%d", &q);
        final = run_rr(final, lista, q);
        printf("---RR---\n");
    }else if(op==4){
        int q;
        printf("Informe o quantum: ");
        scanf("%d", &q);
        final = run_srtf(final, lista, q);
        printf("---SRTF---\n");
    }else if(op==5){
        final = run_prioc(final, lista);
    }else if(op==6){
        int q;
        printf("Informe o quantum: ");
        scanf("%d", &q);
        final = run_priop(final, lista, q);
    }else{
        printf("Escolha um algoritmo  valida!\n");
        return 0;
    }

    exibe(final);
    tempo = soma_tempo(final, tempo); //Soma os tempos de espera e de execução
    double r, w;
    r = (double) (tempo->tt/c);        
    w = (double) (tempo->tw/c);

    printf("Tempo médio de execução (tt): %.2lf\n", r);
    printf("Tempo médio de espera (tw): %.2lf\n", w);
    return 1;
}