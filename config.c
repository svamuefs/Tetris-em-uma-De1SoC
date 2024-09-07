//Includes

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

    //Bibliotecas disponiveis na DE1-SoC
    #include <intelfpgaup/KEY.h> 
    #include <intelfpgaup/accel.h>
    #include <intelfpgaup/video.h>

//Defines

#define video_BLACK 0x00
#define LARGURA_TELA 319 // Tamanho da tela VGA
#define ALTURA_TELA 239 // Tamanho da tela VGA
#define BLOCOS_POR_PECA 4 // Quantidade de blocos que compõe as peças
#define SPAWN_BLOCK_X 3 //Coordenada X de surgimento do tetromino
#define SPAWN_BLOCK_Y 0 //Coordenada Y de surgimento do tetromino
#define LINHAS_TABULEIRO 15 // Quantidade de colunas de blocos no tabuleiro, contando com chão
#define COLUNAS_TABULEIRO 12 // Quantidade de linhas de blocos no tabuleiro, contando com paredes
#define QUANTIDADE_TETROMINOS 7 // Quantidade de tetrominos no jogo
#define QUADRADO_LADO 10 // Tamanho em pixels do lado dos quadrados a serem mostrados na tela
#define TICKS 60// Quantas vezes a main é executada em um único segundo
#define COOLDOWN_GRAVIDADE 30 // Tempo em ticks do cooldown da gravidade
#define COOLDOWN_INPUT 12 // Tempo em ticks do cooldown para input do jogador
#define INPUT_INCLINACAO 30 // Inclinação necessaria para aceitar o input do jogador
#define TUTORIAL_TEXT "Botão 1 : Pause/Start\nBotão 2 : Reset\nBotão 3 : Giro Anti Horário\nBotão 4 : Giro Horário\n"
#define GAMEOVER_TEXT "Fim de Jogo!\nPressione Reset(Botão 2)\n
#define PAUSE_TEXT "Jogo Pausado!\nPressione Pause ou Reset(Botão 1 e 2)"

//Structs

// Tetromino é o nome das peças do tetris :)
typedef struct {
  	int formato[BLOCOS_POR_PECA][BLOCOS_POR_PECA]; //0 não tem bloco, 1 tem bloco
  	int cor;        // Cor do bloco, o index da lista de cores
} Tetromino;

short cores[] = {video_BLACK, video_WHITE, video_YELLOW, video_ORANGE, video_BLUE,
                 video_CYAN, video_MAGENTA, video_GREEN, video_RED};

//TETROMINOS

Tetromino tetrominoO = {{{0,0,0,0} ,
                         {0,1,1,0} ,
                         {0,1,1,0} ,
                         {0,0,0,0}}, 2};

Tetromino tetrominoL = {{{0,0,0,0} ,
                         {0,0,0,1} , 
                         {0,1,1,1} ,
                         {0,0,0,0}}, 3};
                            
Tetromino tetrominoJ = {{{0,0,0,0} ,
                         {0,1,0,0} , 
                         {0,1,1,1} ,
                         {0,0,0,0}}, 4};

Tetromino tetrominoI = {{{0,0,0,0} , 
                         {1,1,1,1} , 
                         {0,0,0,0} ,
                         {0,0,0,0}}, 5};

Tetromino tetrominoT = {{{0,0,0,0} , 
                         {0,0,1,0} , 
                         {0,1,1,1} ,
                         {0,0,0,0}}, 6};

Tetromino tetrominoS = {{{0,0,0,0} , 
                         {0,0,1,1} , 
                         {0,1,1,0} ,
                         {0,0,0,0}}, 7};

Tetromino tetrominoZ = {{{0,0,0,0} , 
                         {0,1,1,0} , 
                         {0,0,1,1} ,
                         {0,0,0,0}}, 8};							 						

Tetromino listaTetrominos[] = {tetrominoO, tetrominoL, tetrominoJ, tetrominoI,
                                tetrominoS, tetrominoZ, tetrominoT};

#define QUANTIDADE_TETROMINOS = sizeof(listaTetrominos)/sizeof(listaTetrominos[0]);

//Variáveis

//tabuleiro do jogo
int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO];

//Tetromino Flutuante = O tetromino controlado pelo jogador
int tetrominoFlutuanteX; // Coordenada X atual do tetromino flutuante 
int tetrominoFlutuanteY; // Coordenada Y atual do tetromino flutuante 
Tetromino tetrominoFlutuanteTipo; // Tipo de tetromino flutuante  

    //Variaveis de controle

    bool pecaFlutuanteExiste = false, gameOver = false, pause = false, reset = false;
    int linhasCheias[BLOCOS_POR_PECA];
    int cooldownGravidade = 0;
    int cooldownMovimento = 0;
    int valorBotoes;
    int sentidoGiro;
    int score = 0; // Pontuação do jogador

    //Variáveis do acelerometro
    int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;

//FUNÇÕES

    //Funções de setup e sistema

void IniciarMapaColisao(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]);
void delay(int segundos);
void interpreta_botoes(int *botoes, bool *pause, bool *reset, int *sentido);

    //Funções de gameplay(mover e rotacionar peças)

bool TestarColisao(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int x, int y);	
bool Mover(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int direcao, int *atualPosX, int *atualPosY);
void DesenharTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY);
void rotacaoTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetrominoFlutuanteTipo, int sentido);

    //Funções de Pontuação

void LimpaLinhas(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[BLOCOS_POR_PECA]);
void VerificaLinhaCheia(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[BLOCOS_POR_PECA]);

    //Funções para exibir o jogo na tela

void imprimirMatriz(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], short cores[]);
void ExibeTetrominoFlutuante(Tetromino *tetrominoFlutuanteTipo, int tetrominoFlutuanteX, int tetrominoFlutuanteY, short cores[]);

//Definição das funções