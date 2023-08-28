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
}Process;

typedef struct l{
    Process* p;
    int t;
    struct l* next;
}List;

typedef struct{
    int a;
    int fcfs_e;
    int fcfs_w;
    int sjf_e;
    int sjf_w;
    int rr_e;
    int rr_w;
    int srtf_e;
    int srtf_w;
    int prioc_e;
    int prioc_w;
    int priop_e;
    int priop_w;
}Timer;

Process* newProcess(char PID[], int timeIn, int exec, int prio, int type, int wait, int run){
    Process* p = malloc(sizeof(Process));
    strcpy(p->PID, PID);
    p->exec = exec;
    p->prio = prio;
    p->timeIn = timeIn;
    p->type = type;
    p->wait = wait;
    p->run = run;
    return p;
}

List* newList(Process* p, int t){
    printf("aaaaa\n");
    List* list = malloc(sizeof(List));
    list->p = p;
    list->t = t;
    list->next = NULL;
    return list;
}

List* insertList(Process* p, List* list, int t){
    if(list == NULL){
        return newList(p, t);
    }else{
        printf("%s\n", list->p->PID);
        if(p->timeIn >= list->p->timeIn){
            list->next = insertList(p, list->next, list->t);
        }else{
            List* aux = newList(p, t);
            aux->next = list;
            return aux;
        }
        
        return list;
    }
}

List* readFile(FILE* f, List* ready){
    int ti, e, prio, type;
    char pid[5];
    ready = NULL;
    printf("a\n");
    fflush(f);
    while (fscanf(f, "%s %d %d %d %d", pid, &ti, &e, &prio, &type)!=EOF)
    {
        printf("b\n");
        Process* p = newProcess(pid, ti, e, prio, type, 0, 0);
        printf("jj\n");
        ready = insertList(p, ready, 0);
        printf("ss\n");
    }
    printf("c\n");
    return ready;
}

List* openFile(char file[], List* ready){
    FILE* f;
    f = fopen(file, "r");
    ready = readFile(f, ready);
    fclose(f);
    return ready;
}

void print(List* list){
    if(list!=NULL){
        Process* aux = list->p;
        printf("PID: %s | ti: %d | e: %d | prio: %d | type: %d | w: %d | ex: %d\n", aux->PID, aux->timeIn, aux->exec, aux->prio, aux->type, aux->wait, aux->run);
        print(list->next);
    }
}

int count(int n, List* r){
    if(r == NULL){
        return n;
    }else{
        return count(n+1, r->next);
    }
}

List* remove_list(List* list){
    if(list==NULL) return NULL;

    List* n = list->next;
    //free(list);
    return n;
}

List* update_time_c(List* list, int t){
    if(list == NULL) return list;

    list->t = t;
    list->p->run = ((list->p->exec +t) - list->p->timeIn);
    list->p->wait = t;
    list->next = update_time_c(list->next, (list->t + list->p->exec));
    return list;
}

List* fcfs_run(List* ready, List* fcfs){
    while (ready!=NULL)
    {
        printf("h\n");
        fcfs = insertList(ready->p, fcfs, 0);
        printf("l\n");
        ready = remove_list(ready);
        printf("m\n");
    }
    fcfs = update_time_c(fcfs, 0);
    return fcfs;
}

int sum_time(int type, List* list){
    int sum = 0;
    if(type == 1){//run
        while (list!=NULL)
        {
            sum += list->p->run;
        }
        
    }else{//wait
        while (list!=NULL)
        {
            sum += list->p->wait;
        }
    }
    return sum;
}

List* remove_list_all(List* list){
    if(list==NULL) return NULL;

    List* n = list->next;
    free(list);
    return n;
}

List * clean(List* list){
    while (list!=NULL)
    {
        list = remove_list(list);
    }
    return list;
}

void layout_menu(){
    printf("        MENU\n");
    printf("1 - Inserir novos dados\n\n");
    printf("2 - FCFS\n\n");
    printf("3 - RR\n\n");
    printf("4 - SJF\n\n");
    printf("5 - SRTF\n\n");
    printf("6 - PrioC\n\n");
    printf("7 - PrioP\n\n");
    printf("8 - Comparação entre os Algoritmos\n\n");
    printf("10 - Imprimir prontos\n\n");
    printf("9 - Sair\n\n\n");
    printf("Selecione uma opcao: ");
}

void menu(List *ready, List *fcfs, List *rr, List *sjf, List* srtf, List* prioc, List* priop, int q, Timer* timer)
{
    layout_menu();
    
    int op;
    scanf("%d", &op);
    if(op==1){
        printf("Informe o quantum que será utilizado: ");
        scanf("%d", &q);
        printf("Informe caminho do arquivo com os dados (Ex: data.txt): ");
        char file[50];
        scanf("%s", file);

        //ready = clean(ready);
        ready = NULL;
        ready = malloc(sizeof(List));
        printf("gg\n");
        ready = openFile(file, ready);

        timer = malloc(sizeof(timer));
        timer->a = count(0, ready);
        timer->fcfs_e = 0;
        timer->fcfs_w = 0;
        timer->rr_e = 0;
        timer->rr_w = 0;
        timer->sjf_e = 0;
        timer->sjf_w = 0;
        timer->prioc_e = 0;
        timer->prioc_w = 0;
        timer->priop_e = 0;
        timer->priop_w = 0;
        getchar();
    }else if(op==9){
        return;
    }else{
        if(ready==NULL){
            printf("Não há dados de entrada, tente novamente após inserir dados!\n");
        }else if(op==2){
            //fcfs = clean(fcfs);
            //List* aux = ready;
            fcfs = NULL;
            fcfs = fcfs_run(ready, fcfs);
            print(ready);
            print(fcfs);

            timer->fcfs_e = sum_time(1, fcfs);
            timer->fcfs_w = sum_time(0, fcfs);
            double r, w;
            r = (double) (timer->fcfs_e/timer->a);
            w = (double) (timer->fcfs_w/timer->a);

            printf("Tempo médio de execução: %.2lf\n", r);
            printf("Tempo médio de espera: %.2lf\n", w);
            getchar();
            
        }else if(op==3){

        }else if(op==4){

        }else if(op==5){

        }else if(op==6){

        }else if(op==7){

        }else if(op==8){

        }else if(op==10){
            print(ready);
            getchar();
        }
    }
    getchar(); // system("pause");
    system("clear"); // system("cls"); // se for windows
    menu(ready, fcfs, rr, sjf, srtf, prioc, priop, q, timer);
}

int main(){
    List* ready = NULL;
    List* fcfs = NULL;
    List* rr = NULL;
    List* sjf = NULL;
    List* srtf = NULL;
    List* prioc = NULL;
    List* priop = NULL;
    Timer* timer = NULL;

    menu(ready, fcfs, rr, sjf, srtf, prioc, priop, 1, timer);

    return 1;
}