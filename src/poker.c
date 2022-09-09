//---------------------------------------------------------------------
// Arquivo	: poker.c
// Conteudo	: jogo de poker
// Autor	: Pedro Gomes Santiago
// Historico	: maio 2022 - arquivo criado
//---------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "poker.h"
#include "msgassert.h"
#define Troca(A, B)       \
    {                     \
        carta_tipo c = A; \
        A = B;            \
        B = c;            \
    }
#define Trade(A, B)         \
    {                       \
        jogador_tipo c = A; \
        A = B;              \
        B = c;              \
    }
void Partida(mesa_tipo *mesa, jogador_tipo *j, char entrada[100], char saida[100])
// Descricao: le os dados e executa a partida
{
    int jogadores_total = 0, id = 0, teste_sanidade = 0;
    char nome[100];
    FILE *arq2;
    arq2 = fopen(saida, "w");
    // verifica se o arquivo existe
    erroAssert((arq2 != NULL), "Arquivo não abriu!");
    FILE *arq;
    arq = fopen(entrada, "r");
    // verifica se o arquivo existe
    erroAssert((arq != NULL), "Arquivo não abriu!");
    fscanf(arq, "%d %d", &mesa->num_rodadas, &mesa->din_init);
    //verifica a consistencia das variaveis
    erroAssert(mesa->num_rodadas > 0, "Rodadas nulas");
    erroAssert(mesa->din_init > 0, "Dinheiro nulo");
    for (int i = 0; i < mesa->num_rodadas; i++)
    {
        if(i==0)
        {
            fscanf(arq, "%d %d", &mesa->num_jogadores, &mesa->pingo);
            jogadores_total = mesa->num_jogadores;
            for (int k = 0; k < mesa->num_jogadores; k++)
            {
                j[k].montante += mesa->din_init;
                fscanf(arq, "%s", j[k].nome);
                fscanf(arq, "%d", &j[k].aposta);
                if(j[k].montante< (mesa->pingo + j[k].aposta) || (j[k].aposta%50)!= 0) //condicoes pra rodada ser invalida (sem dinheiro pra aposta+pingo) ou aposta nao ser multipla de 50
                    teste_sanidade = 1;
                for (int l = 0; l < 5; l++)
                {
                    fscanf(arq, "%d", &j[k].carta[l].numero);
                    fscanf(arq, "%s", j[k].carta[l].nipe);
                }
            }
            mesa->pote = 0; //zerando o pote antes da realização de cada rodada
            if(!teste_sanidade)
                jogaRodada(mesa, j, i, jogadores_total);
            escreveSaida(mesa, j, arq2, i, jogadores_total, teste_sanidade);
            escreveSaidaDesafio(mesa, j, i, jogadores_total); 
            teste_sanidade = 0;

        }
        else
        {
            fscanf(arq, "%d %d", &mesa->num_jogadores, &mesa->pingo);
            for (int k = 0; k < mesa->num_jogadores; k++)
            {
                fscanf(arq, "%s", nome);
                for (int z = 0; z < jogadores_total; z++)
                {
                    if(!strcmp(nome, j[z].nome))
                    {
                        id = z;
                    }
                }

                Trade(j[k], j[id]);
                fscanf(arq, "%d", &j[k].aposta);
                if(k == (mesa->num_jogadores - 1))
                {
                    for(int i = 0; i < jogadores_total; i++)
                        if(j[i].montante< (mesa->pingo + j[i].aposta) || (j[i].aposta%50)!= 0)
                        {
                            teste_sanidade = 1;                            
                        }

                }
                for (int l = 0; l < 5; l++)
                {
                    fscanf(arq, "%d",&j[k].carta[l].numero);
                    fscanf(arq, "%s",j[k].carta[l].nipe);
                }
            }
            mesa->pote = 0;
            if(!teste_sanidade)
                jogaRodada(mesa, j, i,jogadores_total);
            escreveSaida(mesa, j, arq2, i, jogadores_total, teste_sanidade);
            escreveSaidaDesafio(mesa, j, i, jogadores_total); 
            teste_sanidade = 0;
        }
    }
    fclose(arq);
    fclose(arq2);
    }
void ordenaCartas(jogador_tipo *j, int qtd_jog)
// Descricao: ordena a mao para facilitar a classificacao e definicao do vencedor
{
    for (int l = 0; l < qtd_jog; l++)
    {
        int Min;
        for (int i = 0; i < MAO - 1; i++)
        {
            Min = i;
            for (int k = i + 1; k < MAO; k++)
            {
                if (j[l].carta[k].numero < j[l].carta[Min].numero)
                    Min = k;
            }
            Troca(j[l].carta[i], j[l].carta[Min]);
        }
    }
}
void frequenciaCarta(jogador_tipo *j, int qtd_jog)
// Descricao: cria um vetor com as frequencias das cartas para auxiliar na classificacao
{
    for (int l = 0; l < qtd_jog; l++)
    {
        for (int i = 0; i < MAO; i++)
            j[l].repet[i] = 0;
        for (int i = 0; i < MAO; i++)
        {
            j[l].carta[i].freq = 0;
            for (int k = 0; k < MAO; k++)
            {
                if (j[l].carta[i].numero == j[l].carta[k].numero)
                    j[l].carta[i].freq++;
            }
            if (j[l].carta[i].freq == 1)
                j[l].repet[1]++;
            else if (j[l].carta[i].freq == 2)
                j[l].repet[2]++;
            else if (j[l].carta[i].freq == 3)
                j[l].repet[3]++;
            else if (j[l].carta[i].freq == 4)
                j[l].repet[4]++;
        }
    }
}
void sequenciaCarta(jogador_tipo *j, int qtd_jog)
// Descricao: identifica as sequencias de valores e de nipes para ajudar na classificacao
{
    for (int l = 0; l < qtd_jog; l++)
    {
        j[l].seq = 1;
        j[l].seqnipe = 1;
        for (int i = 1; i < MAO; i++)
        {
            if (j[l].carta[i].numero == j[l].carta[i - 1].numero + 1)
                j[l].seq++;
            else
                j[l].seq = 1;
            if (!strcmp(j[l].carta[i].nipe, j[l].carta[i - 1].nipe))
                j[l].seqnipe++;
        }
    }
}
void normaliza(jogador_tipo *j, int qtd_jog)
// Descricao: normaliza as maos definindo o as como 14 visto que eh a carta mais alta na maioria dos casos
{
    for (int l = 0; l < qtd_jog; l++)
    {
        for (int i = 0; i < MAO; i++)
        {
            if (j[l].carta[i].numero == 1)
                j[l].carta[i].numero = 14;
        }
    }
}
void isOnePair(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao eh um par
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if (j[l].repet[2] == 2 && j[l].repet[1] == 3)
        {
            j[l].clas = "OP";
            if (j[l].carta[0].numero == j[l].carta[1].numero)
                j[l].pontos = 100000000000 + 1000000 * j[l].carta[0].numero + j[l].carta[4].numero * 10000 + j[l].carta[3].numero * 100 + j[l].carta[2].numero;
            if (j[l].carta[1].numero == j[l].carta[2].numero)
                j[l].pontos = 100000000000 + 1000000 * j[l].carta[1].numero + j[l].carta[4].numero * 10000 + j[l].carta[3].numero * 100 + j[l].carta[0].numero;
            if (j[l].carta[2].numero == j[l].carta[3].numero)
                j[l].pontos = 100000000000 + 1000000 * j[l].carta[2].numero + j[l].carta[4].numero * 10000 + j[l].carta[1].numero * 100 + j[l].carta[0].numero;
            if (j[l].carta[3].numero == j[l].carta[4].numero)
                j[l].pontos = 100000000000 + 1000000 * j[l].carta[3].numero + j[l].carta[2].numero * 10000 + j[l].carta[1].numero * 100 + j[l].carta[0].numero;
        }
    }
}
void isTwoPair(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao sao dois pares
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if (j[l].repet[2] == 4 && j[l].repet[1] == 1)
        {
            j[l].clas = "TP";
            if (j[l].carta[0].numero != j[l].carta[1].numero)
                j[l].pontos = 200000000000 + j[l].carta[0].numero + 100 * j[l].carta[1].numero + 10000 * j[l].carta[4].numero;
            else if (j[l].carta[2].numero != j[l].carta[3].numero && j[l].carta[2].numero != j[l].carta[1].numero)
                j[l].pontos = 200000000000 + j[l].carta[2].numero + 100 * j[l].carta[0].numero + 10000 * j[l].carta[4].numero;
            else if (j[l].carta[4].numero != j[l].carta[3].numero)
                j[l].pontos = 200000000000 + j[l].carta[4].numero + 100 * j[l].carta[0].numero + 10000 * j[l].carta[3].numero;
        }
    }
}
void isThreeKind(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao eh uma trinca
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if (j[l].repet[3] == 3 && j[l].repet[1] == 2)
        {
            j[l].clas = "TK";
            if ((j[l].carta[0].numero == j[l].carta[1].numero) && (j[l].carta[0].numero== j[l].carta[2].numero))
                j[l].pontos = 300000000000 + 10000 * j[l].carta[0].numero + j[l].carta[4].numero*100 + j[l].carta[3].numero;
            else if ((j[l].carta[1].numero == j[l].carta[2].numero) && (j[l].carta[1].numero == j[l].carta[3].numero))
                j[l].pontos = 300000000000 + 10000 * j[l].carta[1].numero + 100 * j[l].carta[4].numero + j[l].carta[0].numero;
            else if ((j[l].carta[2].numero == j[l].carta[3].numero)&&(j[l].carta[2].numero== j[l].carta[4].numero))
                j[l].pontos = 300000000000 + 10000 * j[l].carta[2].numero + 100 * j[l].carta[1].numero + j[l].carta[0].numero;
        }
    }
}
void isStraight(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao tem uma sequencia de 5 cartas com nipes diferentes
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if ((j[l].seq == 5 && j[l].seqnipe != 5) || (j[l].carta[0].numero == 2 && j[l].carta[1].numero == 3 && j[l].carta[2].numero == 4 && j[l].carta[3].numero == 5 && j[l].carta[4].numero == 14 && j[l].seqnipe != 5))
        {
            j[l].clas = "S";
            if (j[l].carta[0].numero == 2 && j[l].carta[4].numero == 14)
                j[l].pontos = 400000000000 + j[l].carta[3].numero;
            else
                j[l].pontos = 400000000000 + j[l].carta[4].numero;
        }
    }
}
void isFlush(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao tem uma sequencia de 5 nipes sem sequencia de valores
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if (j[l].seq != 5 && j[l].seqnipe == 5 && !(j[l].carta[0].numero == 2 && j[l].carta[1].numero == 3 && j[l].carta[2].numero == 4 && j[l].carta[3].numero == 5 && j[l].carta[4].numero == 14))
        {
            j[l].clas = "F";
            j[l].pontos = 500000000000 + j[l].carta[4].numero*100000000 + j[l].carta[3].numero*1000000 + j[l].carta[2].numero*10000 + j[l].carta[1].numero*100 + j[l].carta[0].numero;
        }
    }
}
void isFullHouse(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao tem uma trinca e um par
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if (j[l].repet[3] == 3 && j[l].repet[2] == 2)
        {
            j[l].clas = "FH";
            if ((j[l].carta[0].numero == j[l].carta[1].numero) && (j[l].carta[0].numero == j[l].carta[2].numero))
                j[l].pontos = 600000000000 + 1000 * j[l].carta[0].numero +  j[l].carta[4].numero;
            else if ((j[l].carta[2].numero == j[l].carta[3].numero) &&(j[l].carta[2].numero == j[l].carta[4].numero))
                j[l].pontos = 600000000000 + 1000 * j[l].carta[4].numero + j[l].carta[0].numero;
        }
    }
}
void isFourKind(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao tem cartas de mesmo numero
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if (j[l].repet[4] == 4 && j[l].repet[1] == 1)
        {
            j[l].clas = "FK";
            if (j[l].carta[0].numero == j[l].carta[1].numero)
                j[l].pontos = 700000000000 + 1000 * j[l].carta[0].numero +  j[l].carta[4].numero;
            else if (j[l].carta[3].numero == j[l].carta[4].numero)
                j[l].pontos = 700000000000 + 1000 * j[l].carta[4].numero +  j[l].carta[0].numero;
        }
    }
}
void isStraightFlush(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao tem uma sequencia de 5 cartas com nipes iguais, diferentes de 10 ate as
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if ((j[l].seq == 5 && j[l].seqnipe == 5) || (j[l].carta[0].numero == 2 && j[l].carta[1].numero == 3 && j[l].carta[2].numero == 4 && j[l].carta[3].numero == 5 && j[l].carta[4].numero == 14 && j[l].seqnipe == 5))
        {
            j[l].clas = "SF";
            if(j[l].carta[4].numero != 14)
                j[l].pontos = 800000000000 + j[l].carta[4].numero;
            else
                j[l].pontos = 800000000000 + j[l].carta[3].numero;
        }
    }
}
void isRoyalStraightFlush(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao tem uma sequencia de 5 cartas com nipes iguais de 10 ate as
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if (j[l].seq == 5 && j[l].seqnipe == 5 && j[l].carta[4].numero == 14)
        {
            j[l].clas = "RSF";
            j[l].pontos = 900000000000;
        }
    }
}
void isHighCard(jogador_tipo *j, int qtd_jog)
// Descricao: verifica se a mao nao eh nenhuma das outras opcoes
{
    for (int l = 0; l < qtd_jog; l++)
    {
        if (j[l].repet[1] == 5 && j[l].seqnipe != 5 && j[l].seq != 5 && !(j[l].carta[0].numero == 2 && j[l].carta[1].numero == 3 && j[l].carta[2].numero == 4 && j[l].carta[3].numero == 5 && j[l].carta[4].numero == 14))
        {
            j[l].clas = "HC";
            j[l].pontos = j[l].carta[4].numero*100000000 + j[l].carta[3].numero*1000000 + j[l].carta[2].numero*10000 + j[l].carta[1].numero*100 + j[l].carta[0].numero;
        }
    }
}
void classificaMao(jogador_tipo *j, mesa_tipo *m)
// Descricao: utiliza as funcoes de classificacao pra definir a mao
{
    isHighCard(j, m->num_jogadores);
    isOnePair(j, m->num_jogadores);
    isTwoPair(j, m->num_jogadores);
    isThreeKind(j, m->num_jogadores);
    isStraight(j, m->num_jogadores);
    isFlush(j, m->num_jogadores);
    isFullHouse(j, m->num_jogadores);
    isFourKind(j, m->num_jogadores);
    isStraightFlush(j, m->num_jogadores);
    isRoyalStraightFlush(j, m->num_jogadores);
}
void inicializaJogadorMesa(mesa_tipo *m, jogador_tipo *j)
// Descricao: construtor pra jogador e mesa, inicializando atributos
{
    for(int l = 0; l < MAXTAM; l++)
    {
        j[l].pontos_partida = 0.0;
        j[l].vit_round = 0;
        j[l].blefe = 0.0;
        j[l].aposta = 0;
        j[l].seq = 0;
        j[l].seqnipe = 0;
        j[l].valorGanho = 0; 
        j[l].montante = 0;
        j[l].valorGanho = 0;
        j[l].pontos = 0;
        for(int i = 0; i<MAO; i++)
            j[l].repet[i] = 0;
    }
    m->pingo = 0;
    m->pote = 0;
    m->num_rodadas = 0;
    m->din_init = 0;
    m->num_jogadores=0;
}
void atualizaRodada(mesa_tipo *m, jogador_tipo *j, int jog_total) 
// Descricao: atualiza os valores para poder resetar a rodada
{
    for(int i = 0; i< jog_total; i++)
    {
        m->pote+=m->pingo;
        j[i].montante -=  m->pingo;
    }

    for(int i = 0; i < m->num_jogadores; i++)
    {
        j[i].montante -=  j[i].aposta;
        m->pote += j[i].aposta;
        j[i].aposta = 0;
    }
}
void classificaBlefe(jogador_tipo *j, mesa_tipo *m) 
//descricao: soma uma pontuação para jogadores que blefaram
{
    for(int i = 0; i < m->num_jogadores; i++)
    {
        if(j[i].pontos< 100000000000)
            j[i].blefe += (j[i].aposta/50)/(j[i].pontos/100000000);
    }
}
void classificaJogador(jogador_tipo *j, mesa_tipo *m, int qtd_jog, int id_rodada)
//descricao: soma uma pontuação para jogadores, pra descobrir o melhor da partida
{  
    float aux_mont = 0.0, aux_vit = 0;
    if(id_rodada == (m->num_rodadas -1))
        for(int i = 0; i <qtd_jog; i++)
        {
            aux_mont = (j[i].montante+m->din_init) / (float)m->din_init;
            aux_vit = (j[i].vit_round + m->num_rodadas)/ (float)m->num_rodadas;
            j[i].pontos_partida = aux_mont * (j[i].blefe + 1) * aux_vit;
        }
}
void jogaRodada(mesa_tipo *m, jogador_tipo *j, int id_rodada, int jog_total)
// Descricao: realiza uma rodada
{   
    normaliza(j, m->num_jogadores);
    ordenaCartas(j, m->num_jogadores);
    frequenciaCarta(j, m->num_jogadores);
    sequenciaCarta(j, m->num_jogadores);
    classificaMao(j, m);
    classificaBlefe(j, m);
    atualizaRodada(m, j, jog_total);
}
void escreveSaida(mesa_tipo *m, jogador_tipo *j, FILE *arq2, int id_rodada, int qtd_jog, int sanidade)
// Descricao: escreve todos os dados de saida em "saida.txt"
{
    if(sanidade) 
    {
        for(int i = 0; i < qtd_jog; i++)// zera as apostas em caso de teste de sanidade
            j[i].aposta = 0;
        fprintf(arq2, "0 0 I"); //formato de invalidade
        fprintf(arq2,"\n"); 
        if(id_rodada == (m->num_rodadas - 1))
        {
            fprintf(arq2, "####\n");
            for (int i = 0; i < qtd_jog - 1; i++)
                for (int k = 1; k < qtd_jog - i; k++)
                if (j[k].montante > j[k - 1].montante)
                    Trade(j[k - 1], j[k]);
            for (int i = 0; i < qtd_jog; i++)
                fprintf(arq2, "%s %d\n", j[i].nome, j[i].montante);
        }      
    }
    else
    {
        double max = j[0].pontos; //todas variaveis criadas para auxiliar na formatacao da saida
        int nv = 0, valor_vit = 0, aux_nv = 0;
        char *clasvit;
        for (int i = 0; i <  m->num_jogadores - 1; i++)
            for (int k = 1; k <  m->num_jogadores - i; k++)
                if (j[k].nome > j[k - 1].nome)
                    Trade(j[k - 1], j[k]); //ordenacao alfabetica por selecao
        for (int l = 0; l < m->num_jogadores; l++)
        {
            if (j[l].pontos > max)
                max = j[l].pontos;
        }
        for(int l = 0; l < m->num_jogadores; l++)
        {
            if (j[l].pontos == max)
            {
                nv++;
                clasvit = j[l].clas;
            }
        }
            valor_vit = m->pote/nv;
            fprintf(arq2, "%d %d %s", nv, valor_vit, clasvit);
            fprintf(arq2,"\n");
        for(int l = 0; l < m->num_jogadores; l++)
        {
            if (j[l].pontos == max)
            {
                j[l].vit_round++;
                j[l].valorGanho = m->pote/nv;
                j[l].montante+=j[l].valorGanho;
                fprintf(arq2,"%s", j[l].nome);
                aux_nv++;
                if(aux_nv != nv)
                    fprintf(arq2,"\n");
            } 

        }
        fprintf(arq2,"\n");
        if(id_rodada == (m->num_rodadas - 1))
        {
            fprintf(arq2, "####\n");
            for (int i = 0; i < qtd_jog - 1; i++)
                for (int k = 1; k < qtd_jog - i; k++)
                if (j[k].montante > j[k - 1].montante)
                    Trade(j[k - 1], j[k]);
            for (int i = 0; i < qtd_jog; i++)
                fprintf(arq2, "%s %d\n", j[i].nome, j[i].montante);
        }
    }
}
void escreveSaidaDesafio(mesa_tipo *m, jogador_tipo *j, int id_rodada, int qtd_jog)
{
    classificaJogador(j, m, qtd_jog, id_rodada);
    FILE *arq3;
    arq3 = fopen("saidaDesafio.txt", "w");
    // verifica se o arquivo existe
    erroAssert((arq3 != NULL), "Arquivo não abriu!");
    float max_blefe = 0;
    float max_pontos_partida = 0;
    if(id_rodada == (m->num_rodadas -1))
        for (int l = 0; l <qtd_jog; l++)
        {
            if (j[l].blefe > max_blefe)
                max_blefe = j[l].blefe;
            if (j[l].pontos_partida > max_pontos_partida)
                max_pontos_partida = j[l].pontos_partida;
        }
    fprintf(arq3, "O(s) maior(es) blefador(es): \n");
    for (int l = 0; l <qtd_jog; l++)
    {
        if (j[l].blefe == max_blefe)
            fprintf(arq3, "%s com %f pontos de blefe\n", j[l].nome, j[l].blefe);
    }
    fprintf(arq3, "O(s) maior(es) jogador(es): \n");
    for (int l = 0; l <qtd_jog; l++)
    {
        if (j[l].pontos_partida == max_pontos_partida)
            fprintf(arq3, "%s com %f pontos\n", j[l].nome, j[l].pontos_partida);
    }
    fclose(arq3);
}


//------------------------------------------------------------------main------------------------------------------
int main()
{
    //cria o numero maximo de jogadores possivel, cria a mesa, armazena os nomes de arquivos de entrada e saida e roda a partida
    jogador_tipo j[MAXTAM];
    mesa_tipo m;
    inicializaJogadorMesa(&m, j);
    char entrada[100] = "entrada.txt";
    char saida[100] = "saida.txt";
    Partida(&m, j, entrada, saida);
    return 0;
}