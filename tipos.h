#include <vector>
#include <stdio.h>

using namespace std;

#define DIAS_AULA 5
#define NUM_HORARIOS 6
#define NUM_SERIES 3
#define MAX_MATERIAS 20
#define MAX_PROFESSORES 40

//TABELA DE CARGAS HORARIAS
#define POUCAS_AULAS 1
#define NORMAL_AULAS 2
#define MUITAS_AULAS 4



struct Materia{
    char nome[12];
    int id; //Identificador único pra evitar comparação com string
    int cargaHoraria[NUM_SERIES];
};

struct Professor{
    char nome[12];
    int id; //Identificador único pra evitar comparação com string
    int cargaHorariaTotal, cargaHorariaAtendida;
    int materiaDada; //Identificador da materia
    int serieQueTrabalha;
    int diasQueTrabalha[DIAS_AULA];
    int qtdDiasQueTrabalha;
    //int cor;
    bool possivelAlocar;
    // função de avaliação
    int fA(){
        return (cargaHorariaTotal-cargaHorariaAtendida);
    };
    // função de custo
    int fC(Materia m){
        return m.cargaHoraria[serieQueTrabalha]+qtdDiasQueTrabalha;
    };
};

struct TimeTable{
    int table[NUM_SERIES][DIAS_AULA][NUM_HORARIOS]; //cada posição da table guarda o id de um professor
    bool ocupado[NUM_SERIES][DIAS_AULA][NUM_HORARIOS]; //cada posição diz se em table existe professor ou não
};

int getSlot(int sala,int dia, int horario){
    return sala+dia*NUM_SERIES+horario*NUM_SERIES*DIAS_AULA;
}


