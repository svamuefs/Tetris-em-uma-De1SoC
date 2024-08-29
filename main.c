//Código público no github que pegamos emprestado para fins didáticos.
//O produto final será completamente distinto e original.

/* Criação de um jogo inspirado em Tetris para ser executado em uma DE1-SoC
 - Os blocos serão movimentados pelo jogador por meio do acelerômetro
 - Todos os itens visuais serão exibidos por meio da interface VGA
 - Botões serão utilizados para: reiniciar, pausar e continuar o jogo
 - O jogo é encerrado de vez pela utilização de ^C
*/

#include <intelfpgaup/KEY.h> 
#include <intelfpgaup/accel.h>
#include <intelfpgaup/video.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>

//TODO: PESQUISAR
//Variável global para encerrar o programa
volatile sig_atomic_t sair; 
void catchSIGINT(int signum) { sair = 1; }

#define video_BLACK 0x00
#define LARGURA_TELA 319 // Tamanho da tela VGA
#define ALTURA_TELA 239 // Tamanho da tela VGA
#define LARGURA_BLOCO 4 // Largura máxima do bloco
#define ALTURA_BLOCO 2 // Altura máxima do bloco
#define SPAWN_BLOCK_X //Coordenada X de surgimento do bloco
#define SPAWN_BLOCK_Y //Coordenada Y de surgimento do bloco
#define LINHA 10 // Quantidade de linhas de blocos
#define COLUNA 15 // Quantidade de colunas de blocos 

// Estrutura de dados para os blocos
typedef struct Tetromino{
  	int formato[LARGURA_BLOCO][ALTURA_BLOCO]; //0 não tem bloco, 1 tem bloco
  	short cor;        // Cor do bloco
	int centroX;  //cordenadas de spawn do bloco
	int centroy;
} Tetromino;

typedef struct Tabuleiro{
  	int matriz[COLUNA][LINHA];
} Tabuleiro;

/*
0110
0110
*/
int matrizQuadrado[4][2] = {{0,0}, {1,1}, {1,1}, {0,0}};
short corQuadrado = video_YELLOW;

// void interpreta_botoes(int *botoes, bool *pause, bool *reset); // Função para interpretar os botões e gerar sinal de pause ou reset
// void gerar_blocos(Bloco blocos[COLUNA][LINHA], short cor); // Função para gerar os blocos
// void exibe_blocos(Bloco blocos[COLUNA][LINHA]); // Função para exibir os blocos
// void detecta_colisao(Bloco blocos[COLUNA][LINHA], int *quant_blocos, bool *vitoria, bool *derrota, 
//                     int bolaX, int bolaY, int *move_bolaX, int *move_bolaY, int *score); // Função para detectar colisão com os blocos

int main(int argc, char *argv[]) {
    signal(SIGINT, catchSIGINT);



    if (KEY_open() == 0 || video_open() == 0 || accel_open() == 0) {
        printf("Erro na inicialização de periféricos.\n");
        return -1;
    }
    printf("Periféricos inicializados.\n");

    video_erase();

    // Inicialização e calibração do acelerômetro
    accel_init();
    accel_format(1, 2);
    accel_calibrate();

	Tetromino quadrado = {matrizQuadrado, corQuadrado, 0 , 0};
	Tetromino l = {matrizQuadrado, corQuadrado, 0 , 0};
	Tetromino j = {matrizQuadrado, corQuadrado, 0 , 0};
	Tetromino i = {matrizQuadrado, corQuadrado, 0 , 0};
	Tetromino listaBlocos[] = {quadrado, l, j, i};

    int botoes, quant_blocos; 
    // bool pause = false, reset = false, fim_de_jogo = false, vitoria = false, derrota = false; // Variáveis de controle
    // int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;
	// int gravidade = -1;
	// float tempoGravidade = 0.75; //segundos

    // int score = 0; // Pontuação do jogador
    // char str[15]; // String para exibição da pontuação
    // char comeco_str[] = "---PLAY---!"; // String para exibição de início de jogo
    // char vitoria_str[] = "YOU WIN!"; // String para exibição de vitória
    // char derrota_str[] = "YOU LOSE!"; // String para exibição de derrota
    // char pause_str[] = "PAUSE!"; // String para exibição de pausa
    // char fim_str[] = "RESET?"; // String para exibição de possível reset
	// bool pecaCaindo = false;	

	return 0;
}
/* Função para leitura de entrada dos botões 
Pause funciona com lógica de alternância */
// void interpreta_botoes(int *botoes, bool *pause, bool *reset) {
// 	KEY_read(botoes);
// 	if (*botoes > 7) {
// 		if (*pause == false)
// 		*pause = true;
// 		else
// 		*pause = false;
// 	}
// 	if (*botoes % 2 != 0)
// 	{
// 		*reset = true;
// 		exibe_tetromino(listaBlocos[0]);
// 	}
// 	else
// 	{
// 		*reset = false;
// 		exibe_tetromino(listaBlocos[0]);
// 	}
// }

// void gerar_blocos()
// {

// }

// //verifica a colisão da proxima posição do bloco com as bordas ou com outros blocos
// bool onCollider(*Tabuleiro, int targetPosX, int targetPosY)
// {
// 	return true;
// }

// //limpa a linha do tabuleiro ao completar
// void limpaLinha()
// {

// }

// void gravidadeOn()
// {

// }

void exibe_tetromino(Tetromino tetromino) {
  int i;
  int j;
  for (i = 0; i < LARGURA_BLOCO; i++) {
    for (j = 0; j < ALTURA_BLOCO; j++) {
		if (tetromino.formato[i][j])
		{
			video_box(10*i, 10*j, 10*i+20, 10*j+20, tetromino.cor);
		}
    }
  }
}

