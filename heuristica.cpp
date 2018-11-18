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
//Quadro de horarios
TimeTable tabela;
int aulasAlocadas = 0;

//Variaveis auxiliares
int listasProfessoresDias[DIAS_AULA][MAX_PROFESSORES][NUM_SERIES];

void leitura(char arquivo_entrada[]){
    //Função de leitura
    FILE *in, *out;
    close(0);
    in = fopen (arquivo_entrada, "r");
    close(1);
    out = fopen ("saida.txt", "w");
    int i, j, mat;
    scanf("%d", &numMaterias); //Lendo as materias
    for(i = 0; i < numMaterias; i++){
        scanf("%s %d", materias[i].nome, &materias[i].id);
        for(j = 0; j < NUM_SERIES; j++){
            scanf("%d", &materias[i].cargaHoraria[j]);
        }
    }
    scanf("%d", &numProfessores); //Lendo os professores
    for(i = 0; i < numProfessores; i++){
        scanf("%s %d %d %d %d", professores[i].nome, &professores[i].id, &professores[i].cargaHorariaTotal, &professores[i].materiaDada, &professores[i].serieQueTrabalha);
        professores[i].cargaHorariaAtendida = 0;
        professores[i].qtdDiasQueTrabalha = 0;
        for(j = 0; j < DIAS_AULA; j++){
            scanf("%d", &professores[i].diasQueTrabalha[j]);
            if(professores[i].diasQueTrabalha[j])
                professores[i].qtdDiasQueTrabalha++;
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

// Busca realizada utilizando a estratégia de busca HEURÍSTICA A*
void busca1(){
    inicializaTabela();
    int i, j, k;
    int aux;
    //Separa os professores nos dias que eles dão aula
    int cont[DIAS_AULA][NUM_SERIES];
    int contahor[DIAS_AULA][NUM_SERIES];
    memset(cont, 0, sizeof(cont));
    int idx_serie;
    for(i = 0; i < numProfessores; i++){
        idx_serie = professores[i].serieQueTrabalha;
        for(j = 0; j < DIAS_AULA; j++){
            if(professores[i].diasQueTrabalha[j]){
                listasProfessoresDias[j][ cont[j][idx_serie] ][ idx_serie ] = professores[i].id;
                listasProfessoresDias[j][ cont[j][idx_serie]+1 ][ idx_serie ] = -1;
                cont[j][idx_serie]++;
            }
        }
    }
    //Aloca os professores usando A*
    bool continua[DIAS_AULA];
    memset(contahor, 0, sizeof(cont));
    memset(continua, false, sizeof(continua));
    int conta, pula;
    int mat, prof, melhor, custo, indiceMelhor, ultimo;
    for(i = 0; i < DIAS_AULA; i++){
        for(k = 0; k < NUM_SERIES; k++){
            conta = 0;
            pula = 0;
            continua[i] = true;
            while(continua[i]){
                continua[i] = false;
                custo = 9999;
                for(j = 0; listasProfessoresDias[i][j][k] != -1; j++){
                    if(listasProfessoresDias[i][j][k] < 0){
                        pula++;
                    }
                    if(contahor[i][k] >= NUM_HORARIOS)
                        continue;
                    prof = getProfessorIndex(listasProfessoresDias[i][j][k]);
                    mat = getMateriaIndex(professores[prof].materiaDada);
                    conta++;
                    if((professores[prof].fA()+professores[prof].fC(materias[mat]) < custo)&&(professores[prof].fA()>0)){ //Pegando o melhor
                        melhor = prof;
                        custo = professores[prof].fA()+professores[prof].fC(materias[mat]);
                        indiceMelhor = j;
                    }
                    if(listasProfessoresDias[i][j+1][k] == -1)
                        ultimo = j;
                }
                //Espaço para alocar as aulas
                if(professores[melhor].fA() == 0){
                    listasProfessoresDias[i][indiceMelhor][k] = -2;
                    continue;
                }
                if(conta != pula){
                    continua[i] = true;
                }else{
                    break;
                }
                if(contahor[i][professores[melhor].serieQueTrabalha] >= NUM_HORARIOS)
                        break;
                professores[melhor].cargaHorariaAtendida++;
                int serie_melhor = professores[melhor].serieQueTrabalha;
                // Atualiza a tabela de horarios
                tabela.table[ serie_melhor ][ i ][ contahor[i][serie_melhor] ] = professores[melhor].id;
                tabela.ocupado[ serie_melhor ][ i ][ contahor[i][serie_melhor] ] = true;
                contahor[i][serie_melhor]++;
                aulasAlocadas++;
            }
        }
    }

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

int main(int argc, char* argv[]){
    leitura(argv[1]);
    busca1();
    imprimirTabela();
    printf("\nAulas alocadas: %d\n", aulasAlocadas);
}
