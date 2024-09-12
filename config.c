#include <intelfpgaup/video.h>

//Defines

#define video_BLACK 0x00
#define LARGURA_TELA 319 // Tamanho da tela VGA
#define ALTURA_TELA 239 // Tamanho da tela VGA
#define QUADRADO_LADO 10 // Tamanho em pixels do lado dos quadrados a serem mostrados na tela

#define QUANTIDADE_TETROMINOS 7 //Quantidade de tetrominos no jogo
#define BLOCOS_POR_PECA 4 // Quantidade de blocos que compõe as peças
#define SPAWN_BLOCK_X 3 //Coordenada X de surgimento do tetromino
#define SPAWN_BLOCK_Y 0 //Coordenada Y de surgimento do tetromino
#define TAMANHO_PREVIEW 4 // Quantidades de peças no preview

#define LINHAS_TABULEIRO 15 // Quantidade de colunas de blocos no tabuleiro, contando com chão
#define COLUNAS_TABULEIRO 12 // Quantidade de linhas de blocos no tabuleiro, contando com paredes

#define TICKS 60// Quantas vezes a main é executada em um único segundo
#define COOLDOWN_GRAVIDADE 30 // Tempo em ticks do cooldown da gravidade
#define COOLDOWN_INPUT 12 // Tempo em ticks do cooldown para input do jogador
#define INPUT_INCLINACAO 30 // Inclinação necessaria para aceitar o input do jogador

#define TUTORIAL_TEXT "Botão 1 : Pause/Start\nBotão 2 : Reset\nBotão 3 : Giro Anti Horário\nBotão 4 : Giro Horário\n"
#define GAMEOVER_TEXT "Fim de Jogo!\nPressione Reset(Botão 2)\n"
#define PAUSE_TEXT "Jogo Pausado!\nPressione Pause ou Reset(Botão 1 e 2)"

//Structs

/*
Tetromino é o nome das peças do tetris :)
  	int formato[BLOCOS_POR_PECA][BLOCOS_POR_PECA]; : 0 não tem bloco, 1 tem bloco
  	int cor; : Cor do bloco, o index da lista de cores
	int x; : posição horizontal do tetromino no tabuleiro
	int y; : posição vertical do tetromino no tabuleiro
*/
typedef struct Tetromino{
  	int formato[BLOCOS_POR_PECA][BLOCOS_POR_PECA]; //0 não tem bloco, 1 tem bloco
  	int cor;        // Cor do bloco, o index da lista de cores
	int x;			
	int y;
} Tetromino;

// const short LISTA_CORES[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

const short LISTA_CORES[] = {video_BLACK, video_WHITE, video_YELLOW, video_ORANGE, video_BLUE,
                 video_CYAN, video_MAGENTA, video_GREEN, video_RED};

//TETROMINOS

/*
0000
1111
0000
0000*/
const Tetromino TETROMINO_I = {{ {0,0,0,0} , 
  						  		 {1,1,1,1} , 
  						  		 {0,0,0,0} ,
  						  		 {0,0,0,0} }, 5, SPAWN_BLOCK_X, SPAWN_BLOCK_Y};
/*
0000
0100
0111
0000*/
const Tetromino TETROMINO_J = {{ {0,0,0,0} ,
  						  		 {0,1,0,0} , 
  						  		 {0,1,1,1} ,
  						  		 {0,0,0,0} }, 4, SPAWN_BLOCK_X, SPAWN_BLOCK_Y};
/*
0000
0001
0111
0000*/
const Tetromino TETROMINO_L = {{ {0,0,0,0} ,
  						  		 {0,0,0,1} , 
  						  		 {0,1,1,1} ,
  						  		 {0,0,0,0} }, 3, SPAWN_BLOCK_X, SPAWN_BLOCK_Y};
/*
0000
0110
0110
0000*/
const Tetromino TETROMINO_O = {{ {0,0,0,0} ,
  						  		 {0,1,1,0} ,
  						  		 {0,1,1,0} ,
  						  		 {0,0,0,0} }, 2, SPAWN_BLOCK_X, SPAWN_BLOCK_Y};
/*
0000
0011
0110
0000*/
const Tetromino TETROMINO_S = {{ {0,0,0,0} , 
  						  		 {0,0,1,1} , 
  						  		 {0,1,1,0} ,
  						  		 {0,0,0,0} }, 7, SPAWN_BLOCK_X, SPAWN_BLOCK_Y};
/*
0000
0010
0111
0000*/
const Tetromino TETROMINO_T = {{ {0,0,0,0} , 
  						  		 {0,0,1,0} , 
  						  		 {0,1,1,1} ,
  						  		 {0,0,0,0} }, 6, SPAWN_BLOCK_X, SPAWN_BLOCK_Y};
/*
0000
0110
0011
0000*/
const Tetromino TETROMINO_Z = {{ {0,0,0,0} , 
  						  		 {0,1,1,0} , 
  						  		 {0,0,1,1} ,
  						  		 {0,0,0,0} }, 8, SPAWN_BLOCK_X, SPAWN_BLOCK_Y};

const Tetromino * LISTA_PONTEIROS_TETROMINOS[QUANTIDADE_TETROMINOS] = 
{
	&TETROMINO_I, &TETROMINO_J, &TETROMINO_L, &TETROMINO_O,
	&TETROMINO_S, &TETROMINO_T, &TETROMINO_Z
};							 						
