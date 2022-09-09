//---------------------------------------------------------------------
// Arquivo	: poker.h
// Conteudo	: definicoes do TAD POKER
// Autor	: Pedro Gomes Santiago
// Historico	: maio 2022 - arquivo criado
//---------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "msgassert.h"
#ifndef MAXTAM
#define MAXTAM 9
#endif
#ifndef MAO
#define MAO 5
#endif
//------------------------------------------------------------classes--------------------------------------
typedef struct mesa
{
    int num_rodadas;
    int din_init;
    int num_jogadores;
    int pingo;
    int pote;
} mesa_tipo;

typedef struct carta
{
    char nipe[1];
    int numero;
    int freq;
} carta_tipo;

typedef struct jogador
{
    char nome[100];
    char *clas;
    float blefe;
    float pontos_partida;
    int vit_round;
    int aposta;
    int montante;
    int valorGanho;
    int repet[MAO];
    int seq;
    int seqnipe;
    double pontos;
    carta_tipo carta[MAO];
} jogador_tipo;
//-------------------------------------------------------------------FUNCOES-----------------------------------------

//---------------------------------------------------------------------leitura e escrita de dados-------------------------------------------------------------------
void Partida(mesa_tipo *mesa, jogador_tipo *j, char entrada[100], char saida[100]);
void escreveSaida(mesa_tipo *m, jogador_tipo *j, FILE *arq2, int id_rodada, int qtd_jog, int sanidade);
void escreveSaidaDesafio(mesa_tipo *m, jogador_tipo *j, int id_rodada, int qtd_jog);
//----------------------------------------------------------------------estruturacao das cartas---------------------------------------
void ordenaCartas(jogador_tipo *j, int qtd_jog);
void frequenciaCarta(jogador_tipo *j, int qtd_jog);
void sequenciaCarta(jogador_tipo *j, int qtd_jog);
void normaliza(jogador_tipo *j, int qtd_jog);
//---------------------------------------------------------------------------rodando o jogo----------------------------------------------------------------------
void jogaRodada(mesa_tipo *m, jogador_tipo *j, int id_rodada, int jog_total);
void inicializaJogadorMesa(mesa_tipo *m, jogador_tipo *j);
void atualizaRodada(mesa_tipo *m, jogador_tipo *j, int jog_total);
void classificaBlefe(jogador_tipo *j, mesa_tipo *m);
void classificaJogador(jogador_tipo *j, mesa_tipo *m, int qtd_jog, int id_rodada);
//------------------------------------------------------------------------possiveis maos-------------------------------------------------------------------
void isOnePair(jogador_tipo *j, int qtd_jog);
void isTwoPair(jogador_tipo *j, int qtd_jog);
void isThreeKind(jogador_tipo *j, int qtd_jog);
void isStraight(jogador_tipo *j, int qtd_jog);
void isFlush(jogador_tipo *j, int qtd_jog);
void isFullHouse(jogador_tipo *j, int qtd_jog);
void isFourKind(jogador_tipo *j, int qtd_jog);
void isStraightFlush(jogador_tipo *j, int qtd_jog);
void isRoyalStraightFlush(jogador_tipo *j, int qtd_jog);
void isHighCard(jogador_tipo *j, int qtd_jog);
void classificaMao(jogador_tipo *j, mesa_tipo *m);
//-----------------------------------------------------------------------


