#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "tipos.h"

using namespace std;


//Lista de matérias e professores
Materia materias[MAX_MATERIAS];
Professor professores [MAX_PROFESSORES];
int numMaterias, numProfessores;
int impossiveisAlocar = 0;
//Quadro de horarios
TimeTable tabela;

void leitura(char arquivo_entrada[]){
    //Função de leitura
    FILE *in, *out;
    close(0);
    in = fopen (arquivo_entrada, "r");
    close(1);
    out = fopen ("saida2.txt", "w");
    int i, j, mat;
    scanf("%d", &numMaterias); //Lendo as materias
    for(i = 0; i < numMaterias; i++){
        scanf("%s %d", materias[i].nome, &materias[i].id);
        for(j = 0; j < NUM_SERIES; j++){
            scanf("%d", &materias[i].cargaHoraria[j]);
        }
    }
    scanf("%d", &numProfessores); //Lendo as materias
    for(i = 0; i < numProfessores; i++){
        scanf("%s %d %d %d %d", professores[i].nome, &professores[i].id, &professores[i].cargaHorariaTotal, &professores[i].materiaDada, &professores[i].serieQueTrabalha);
        professores[i].cargaHorariaAtendida = 0;
        professores[i].possivelAlocar = true;
        for(j = 0; j < DIAS_AULA; j++){
            scanf("%d", &professores[i].diasQueTrabalha[j]);
        }
    }
    printf("Total de %d materias e %d professores\n", numMaterias, numProfessores);
}

int getMateriaIndex(int id){
    int i;
    for(i = 0; i < numMaterias; i++)
        if(materias[i].id == id)
            return i;
    return -1;
}

int getProfessorIndex(int id){
    int i;
    for(i = 0; i < numProfessores; i++)
        if(professores[i].id == id)
            return i;
    return -1;
}

void inicializaTabela(){
    memset(tabela.ocupado, false, sizeof(tabela.ocupado)); //Seta todo mundo como não ocupado
}

void formata(char *in){
    char mask[15] = "|            |";
    if(in == NULL)
        printf("%s", mask);
    int i, tam;
    for(tam = 0; in[tam] != '\0'; tam++);
    for(i = 0; in[i] != '\0'; i++){
        mask[i+1+((8-tam)/2)] = in[i];
    }
    printf("%s", mask);
}

//Função para impressão do quadro de horários
void imprimirTabela(){
    int serie, hor, dia;
    int prof;
    for(serie = 0; serie < NUM_SERIES; serie++){
        printf("Serie: %d\n", serie+1);
        for(hor = 0; hor < NUM_HORARIOS; hor++){
            printf("\n");
            for(dia = 0; dia < DIAS_AULA; dia++){
                if(tabela.ocupado[serie][dia][hor]){
                    prof = getProfessorIndex(tabela.table[serie][dia][hor]);
                    formata(professores[prof].nome);
                }else{
                    printf("|            |");
                }
            }
        }
        printf("\n");
    }

}

std::pair<int,int> geraDiaHora(int i, int d, int h) {

    int j, k, dia = -1, hora = -1;
    for (j = 0; j < DIAS_AULA; j++) {
            if (professores[i].diasQueTrabalha[j] == 1) {
                dia = j;
                for (k = 0; k < NUM_HORARIOS; k++) {
                    if (tabela.ocupado[professores[i].serieQueTrabalha][dia][k] == false) {
                        hora = k;
                        return std::make_pair(dia, hora);
                    }
                }
            }
        }

        return std::make_pair(dia, hora);
        /* retorna o primeiro dia e hora disponiveis para alocar o professor de índice i baseado nas restricoes do professor
        exemplo: caso ele de aula apenas na segunda e terca, apenas slots disponiveis nesse dia serao alocados caso estejam disponiveis
        preferencia eh um slot mais a esquerda: exemplo, caso haja um slot disponivel nas segundas, a aula sera alocada nesse dia, independente de terca.*/
}

bool checaParada() {
    int i, cont;

    for (i = 0; i < numProfessores; i++) {
        if (professores[i].possivelAlocar == false)
            cont++;
    }

    if (cont == numProfessores)
        return true;

    return false;

}

void buscaCega_visit(int i) {

    // i - indice do professor atual a ser alocado
    int x = professores[i].cargaHorariaTotal - professores[i].cargaHorariaAtendida;
    // x = calcula quantas aulas faltam para serem alocadas para o professor de índice i

    int j, k;
    int dia = -1, hora = -1;
    //j e k sao apenas índices
    // dia e hora sao auto-explicativas. representam o indice na Tabela

    /*possivelAlocar - variavel booleana que indica se eh possivel alocar o professor. */
    if (professores[i].possivelAlocar) {

        std::pair<int, int> p = geraDiaHora(i,dia,hora);
        dia = p.first;
        hora = p.second;


        if ((dia != -1) && (hora != -1)) {
            //significa que um slot vago foi encontrado e o professor e aula podem ser alocados
            tabela.ocupado[professores[i].serieQueTrabalha][dia][hora] = true;
            tabela.table[professores[i].serieQueTrabalha][dia][hora] = professores[i].id;

            professores[i].cargaHorariaAtendida++;
             if (x == 1) {
                professores[i].possivelAlocar = false;
                impossiveisAlocar++; //agora o numero de aulas desse professor que faltam serem alocados é 0, logo ele nao eh mais possivel alocar simplesmente porque nao e mais necessario.
            }
        } else {
                professores[i].possivelAlocar = false;
                impossiveisAlocar++;
                // ultima aula foi alocada
                // nao ha slots disponiveis para alocar esse professor. suas aulas ja coincidem com outros horarios ja alocados

            if (checaParada()) {
                return;
            } //nenhum professor pode ser alocado. a busca PARA.
        }

        i++;
        k = 0;

        while (!(professores[i].possivelAlocar)) {
            // procurar o proximo professor no vetor de professores que seja possivel alocar
            i++;
            if (i >= numProfessores) {
                i = 0;
            }
            if (k >= numProfessores)
                return;
            // caso nao encontre, a busca PARA.
            k++;
        }

        buscaCega_visit(i); //novo professor para ser alocado

    }
}

void buscaCega() {
    inicializaTabela();

    buscaCega_visit(0); //inicia a Busca cega tomando como raiz o primeiro professor mais a esquerda no vetor de professores.
}

int main(int argc, char* argv[]){
    leitura(argv[1]);
    buscaCega();
    imprimirTabela();
}
