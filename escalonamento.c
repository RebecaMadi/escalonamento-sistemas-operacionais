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
    int tr;
}Processo;

typedef struct l{
    Processo* p;
    int t;
    struct l* next;
}Lista;

typedef struct{
    double tt, tw;
}Tempo;

Processo* novoProcesso(char PID[], int timeIn, int exec, int prio, int type, int wait, int run){
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
    //printf("aaaaa\n");
    Lista* lista = malloc(sizeof(Lista));
    lista->p = p;
    lista->t = t;
    lista->next = NULL;
    return lista;
}

Lista* inserirLista(Processo* p, Lista* lista, int t){
    if(lista == NULL){
        return novaLista(p, t);
    }else{
        //printf("%s\n", lista->p->PID);
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
    int ti, e, prio, type;
    char pid[5];
   // printf("a\n");
    while (fscanf(arq, "%s %d %d %d %d", pid, &ti, &e, &prio, &type)!=EOF)
    {
        //printf("b\n");
        Processo* p = novoProcesso(pid, ti, e, prio, type, 0, 0);
        //printf("jj\n");
        lista = inserirLista(p, lista, 0);
       // printf("ss\n");
    }
   // printf("c\n");
    return lista;
}

Lista* abrirArquivo(char arq[], Lista* lista){
    FILE* f;
    f = fopen(arq, "r");
    lista = lerArquivo(f, lista);
    fclose(f);
    return lista;
}

Lista* remover_lista(Lista* lista, Processo* p){
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
    if(lista == NULL) return lista;

    lista->t += t;
    lista->p->run += ((lista->p->exec +t) - lista->p->timeIn);
    lista->p->wait += t;
    lista->next = atualiza_tempo_c(lista->next, (lista->t + lista->p->exec));
    return lista;
}

Lista* run_fcfs(Lista* fcfs, Lista* lista){
    while (lista!=NULL)
    {
       // printf("h\n");
        fcfs = inserirLista(lista->p, fcfs, 0);
        //printf("l\n");
        lista = remover_lista(lista, lista->p);
        //printf("m\n");
    }
    fcfs = atualiza_tempo_c(fcfs, 0);
    return fcfs;
}

void exibe(Lista* lista){
    if(lista!=NULL){
        Processo* aux = lista->p;
        //printf("PID: %s | ti: %d | e: %d | prio: %d | type: %d | w: %d | ex: %d\n", aux->PID, aux->timeIn, aux->exec, aux->prio, aux->type, aux->wait, aux->run);
        printf("%s ", aux->PID);
        exibe(lista->next);
    }else{
        printf("\n");
    }
    //printf("abczn");
}

int buscarLista(Lista* lista, Processo* p){
    if(lista == NULL){
        return 0;
    }else{
        if(lista->p==p) return 1;
        else return buscarLista(lista->next, p);
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
    if(r == NULL){
        return n;
    }else{
        return cont(n+1, r->next);
    }
}

Processo* buscaMenorTempo(Lista* lista, Processo* p, int t){
    if(lista == NULL) return p;
    if(lista->p->timeIn<=t){
         if(lista->p->exec<p->exec){
            p = lista->p;
         }
    }
    return buscaMenorTempo(lista->next, p, t);
    
}

Lista* buscaTempo(Lista* lista, Lista* list, int t){
    if(lista == NULL) return list;
    if(lista->p->timeIn==t){
        if(buscarLista(list, lista->p)==0){
            list = inserirLista(lista->p, list, t);
        }
    }
    list = buscaTempo(lista->next, list, t);   
    return list;
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
    int t=0;
    Processo* u=NULL;
    while (lista!=NULL)
    {
        //sjf = buscaTempo(lista, sjf, t);
        //if(sjf!=NULL){
            Processo* aux = buscaMenorTempo(lista, lista->p, t);
            aux->run = (t+aux->exec)-aux->timeIn;
            aux->wait = t-aux->timeIn;
            sjf = inserirFila(aux, sjf, t);
            t += aux->exec;
            lista = remover_lista(lista, aux);
            /*if(u==NULL) u = aux;
            else if(u!=aux){
                u = aux;
                sjf = inserirLista(aux, sjf, t);
                //sjf = atualiza_tempo_c(sjf, 0);
                
            }else if(u==aux){
                if(u->exec==(t-u->timeIn)){
                    u->run = t - u->timeIn;
                    u->timeIn = -1;
                    lista = atualiza_espera(lista, t-u->timeIn);
                    lista = remover_lista(lista, aux);
                    u = NULL;
                }else{
                    u->run++;
                    u->timeIn++;
                    u->exec--;
                }
            }*/
        //}
    }
    return sjf;
}

Processo* chegouPrimeiro(Lista* lista, Processo* p, int t){
    if(lista==NULL) return p;
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
        Processo* p = chegouPrimeiro(lista, lista->p, t);
        rr = inserirFila(p, rr, t);
        p->wait += t - p->timeIn;
        //printf("%s %d %d\n", p->PID, p->wait, t);
        if((p->exec-quantum)<=0){
            q = quantum - p->exec;

            if(q==0){t+= quantum;}
            else{ t += q;}

            p->run = t - p->tr;
            //printf("r: %s %d\n", p->PID, p->run);
            lista = remover_lista(lista, p);
        }else{
            p->exec = p->exec - quantum;
            p->timeIn = t + quantum;
            t += quantum;
            q = quantum;
        }
        /*
        for(int i=0; i<q; i++){
            int aux = p->tr;
            p->tr = -1;
            //exibe(rr);
            lista = atualiza_espera(lista, 1, t);
            p->tr = aux;
        }*/
    }
    return rr;
}

int main(){
    Lista* lista = NULL;
    Lista* final = NULL;
    Tempo* tempo = malloc(sizeof(Tempo));
    double tt, tw, c;
    char nome[50];
    printf("Informe o nome do arquivo (Ex: dados.txt): ");
    scanf("%s", nome);
    printf("Algoritmos de escalonamento: (1)FCFS, (2)SJF, (3)RR, (4)SRTF, (5)PrioC, (6)PrioP\n");
    printf("Escolha um algoritmo: ");
    int op;
    scanf("%d", &op);
    lista = abrirArquivo(nome, lista);
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

    }else if(op==5){

    }else if(op==6){

    }else{
        printf("Escolha um algoritmo  valida!\n");
        return 0;
    }

    exibe(final);
    tempo = soma_tempo(final, tempo);
    //printf("%lf %lf %lf", tt, tw, c);
    double r, w;
    r = (double) (tempo->tt/c);        
    w = (double) (tempo->tw/c);

    printf("Tempo médio de execução: %.2lf\n", r);
    printf("Tempo médio de espera: %.2lf\n", w);
    return 1;
}