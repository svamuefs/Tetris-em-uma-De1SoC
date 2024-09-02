//Código público no github que pegamos emprestado para fins didáticos.
//O produto final será completamente distinto e original.

/* Criação de um jogo inspirado em Tetris para ser executado em uma DE1-SoC
 - Os blocos serão movimentados pelo jogador por meio do acelerômetro
 - Todos os itens visuais serão exibidos por meio da interface VGA
 - Botões serão utilizados para: reiniciar, pausar e continuar o jogo
 - O jogo é encerrado de vez pela utilização de ^C
*/

// #include <intelfpgaup/KEY.h> 
// #include <intelfpgaup/accel.h>
// #include <intelfpgaup/video.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

//TODO: PESQUISAR
//Variável global para encerrar o programa

// volatile sig_atomic_t sair; 
// void catchSIGINT(int signum) { sair = 1; }

#define video_BLACK 0x00
#define LARGURA_TELA 319 // Tamanho da tela VGA
#define ALTURA_TELA 239 // Tamanho da tela VGA
#define COLUNAS_TETROMINO 4 // Largura máxima do bloco
#define LINHAS_TETROMINO 2 // Altura máxima do bloco
#define SPAWN_BLOCK_X //Coordenada X de surgimento do bloco
#define SPAWN_BLOCK_Y //Coordenada Y de surgimento do bloco
#define TETROMINO_FLUTUANTE_X // Coordenada X atual do tetromino  
#define TETROMINO_FLUTUANTE_Y
#define COLUNAS_TABULEIRO 15 // Quantidade de colunas de blocos 
#define LINHAS_TABULEIRO 10 // Quantidade de linhas de blocos

// Tetromino é o nome das peças do tetris :)
typedef struct {
  	int formato[COLUNAS_TETROMINO][LINHAS_TETROMINO]; //0 não tem bloco, 1 tem bloco
  	short cor;        // Cor do bloco
} Tetromino;

//TETROMINOS

/*
Desenho:
0110
0110
*/
	Tetromino tetrominoO = {{{0,0}, {1,1}, {1,1}, {0,0}}, 0};
/*0001
  0111*/
	Tetromino tetrominoL = {{{0,0}, {0,1}, {0,1}, {1,1}}, 1};
/*1000
  1110*/
	Tetromino tetrominoJ = {{{1,1}, {0,1}, {0,1}, {0,0}}, 2};
/*1111
  0000*/
	Tetromino tetrominoI = {{{1,0}, {1,0}, {1,0}, {1,0}}, 3};

//!!IGNORAR!!
// void interpreta_botoes(int *botoes, bool *pause, bool *reset); // Função para interpretar os botões e gerar sinal de pause ou reset
// void gerar_blocos(Bloco blocos[COLUNA][LINHA], short cor); // Função para gerar os blocos
// void exibe_blocos(Bloco blocos[COLUNA][LINHA]); // Função para exibir os blocos
// void detecta_colisao(Bloco blocos[COLUNA][LINHA], int *quant_blocos, bool *vitoria, bool *derrota, 
//                     int bolaX, int bolaY, int *move_bolaX, int *move_bolaY, int *score); // Função para detectar colisão com os blocos

void IniciarMapaColisao(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO]);
void PrintMatriz(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO]);
bool TestarColisao(int matrizColisao[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int x, int y);	
bool Mover(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int direcao, int atualPosX, int atualPosY);
void TranslacaoTetromino(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int atualPosX, int atualPosY, int novaPosX, int novaPosY);
void DesenharTetromino(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY, int cor);

int main() {

	int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO];
	IniciarMapaColisao(matriz);
	PrintMatriz(matriz);
	printf("\n");
	DesenharTetromino(matriz, &tetrominoO, 5, 5, 1);
	PrintMatriz(matriz);
	printf("\n");
	Mover(matriz, &tetrominoO,-1, 5, 5);
	PrintMatriz(matriz);
	printf("\n");

	//teste
	if(TestarColisao(matriz, &tetrominoO, 0, 0)){
		printf("Há colisão");
	} else {
		printf("Não há colisão");
	}

	//!!IGNORAR TUDO COMENTADO ABAIXO!!

    // signal(SIGINT, catchSIGINT);
    // if (KEY_open() == 0 || video_open() == 0 || accel_open() == 0) {
    //     printf("Erro na inicialização de periféricos.\n");
    //     return -1;
    // }

    // printf("Periféricos inicializados.\n");

    // video_erase();
	// video_clear();

	// Tetromino tetrominos[] = {tetrominoO, tetrominoL, tetrominoJ, tetrominoI};

	// Inicialização e calibração do acelerômetro
	// accel_init();
	// accel_format(1, 2);
	// accel_calibrate();
 	// int botoes, quant_blocos;

	// while(1){
    // 	interpreta_botoes(&botoes, &tetrominoI, &tetrominoJ, &tetrominoL, &tetrominoO);
	// }
	// video_clear();
	// exibe_tetromino(&tetrominoO);
	// delay(1);
	// video_show();
	// video_clear();
	// exibe_tetromino(&tetrominoL);
	// delay(1);
	// video_show();
	// video_clear();
	// exibe_tetromino(&tetrominoJ);
	// delay(1);
	// video_show();
	// video_clear();
	// exibe_tetromino(&tetrominoI);
	// delay(1);
	// video_show();


    // bool pause = false, reset = false, fim_de_jogo = false, vitoria = false, derrota = false; // Variáveis de controle
    // // int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;

	int gravidade = -1;
	float tempoGravidade = 0.75; //segundos

    // int score = 0; // Pontuação do jogador
    // char str[15]; // String para exibição da pontuação
    // char comeco_str[] = "---PLAY---!"; // String para exibição de início de jogo
    // char vitoria_str[] = "YOU WIN!"; // String para exibição de vitória
    // char derrota_str[] = "YOU LOSE!"; // String para exibição de derrota
    // char pause_str[] = "PAUSE!"; // String para exibição de pausa
    // char fim_str[] = "RESET?"; // String para exibição de possível reset
	// bool pecaCaindo = false;	
	// video_close();
	return 0;
}

/*
sinval: função escrita pelos donos do repositório que "inspirou" esse código

Função para leitura de entrada dos botões 
Pause funciona com lógica de alternância 
*/
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

// !!IGNORAR!!
// void interpreta_botoes(int *botoes, Tetromino* tetrominoI, Tetromino* tetrominoJ, Tetromino* tetrominoL, Tetromino* tetrominoO) {
// 	KEY_read(botoes);
// 	if (*botoes ==1)
//   {
//     video_clear();
//     exibe_tetromino(&tetrominoO);
//     video_show();
//   }
//   else if(*botoes == 2)
//   { 
//     video_clear();
//     exibe_tetromino(&tetrominoI);
//     video_show();
//   }
//   else if(*botoes == 4)
//   { 
//     video_clear();
//     exibe_tetromino(&tetrominoL);
//     video_show();
//   }
//   else if(*botoes == 8)
//   { 
//     video_clear();
//     exibe_tetromino(&tetrominoJ);
//     video_show();
//   }

	
// }

/*
Inicia o mapa de colisao, gerando o chão e as paredes do tabuleiro do jogo. 
Ex:
1 0 0 0 1 
1 0 0 0 1 
1 0 0 0 1
1 1 1 1 1
uma matriz composta de 0s e 1s
1 : significa uma parede 
0 : espaço livre
Ver a função TestarColisao() para entender melhor o funcionamento

Talvez seja desnecessário, discutiremos melhor
*/
void IniciarMapaColisao(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO])
{
	int i;
	int j;

	for(i = 0; i < COLUNAS_TABULEIRO; i++) 
	{
		for(j = 0; j < LINHAS_TABULEIRO; j++) 
		{
			if(j == 0 || j == LINHAS_TABULEIRO-1 || i == COLUNAS_TABULEIRO-1) 
			{
				matriz[i][j] = 1;

			} 
			else 
			{
				matriz[i][j] = 0;
			}
		}
	}
}

//TODO gerar blocos aleatoriamente 
// void GerarBlocos()
// {

// }

/*
Função para mover a peça para a esquerda, direita ou pela gravidade
Talvez seja necessário separar essa função um duas: uma para lidar com colisão 
e outra para efetivamente mover a peça
Parametros: 
int direcao : -1 , 0 ou 1
int matrizColisao : chamar a função TestarColisao()
Tetromino *tetromino : ponteiro apontando para o tipo de tetromino a ser movido
int atualPosX e int atualPosY : posição atual da ancora do tetromino na matriz do jogo
*/
bool Mover(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int direcao, int atualPosX, int atualPosY) {

    if(direcao == 0)
    {
        if(TestarColisao(matriz, tetromino, atualPosX, atualPosY-1))
        {
            //Há colisão e o movimento não foi realizado
            return false;
        }
        else
        {
            //Não há colisão, movimento realizado
            TranslacaoTetromino(matriz, tetromino, atualPosX, atualPosY, atualPosX, atualPosY-1);
            return true;
        }
    }
    else
    {
        if(TestarColisao(matriz, tetromino, atualPosX + direcao, atualPosY))
        {
            //Há colisão e o movimento não foi realizado
            return false;
        }
        else
        {
            //Não há colisão, movimento realizado
            TranslacaoTetromino(matriz, tetromino, atualPosX, atualPosY, atualPosX + direcao, atualPosY);
            return true;
        }
    }
}

void TranslacaoTetromino(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int atualPosX, int atualPosY, int novaPosX, int novaPosY)  
{
    //Apagar posição anterior
    DesenharTetromino(matriz, tetromino, atualPosX, atualPosY, 0);
    //Desenhar nova posição
    DesenharTetromino(matriz, tetromino, novaPosX, novaPosY, 1);
}
    

void DesenharTetromino(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY, int cor) 
{
    int i;
    int j;
    for(i = 0; i < COLUNAS_TETROMINO; i++)
    {
        for(j = 0; j < LINHAS_TETROMINO; j++)
        {
			if(tetromino->formato[i][j] == 1)
			{
            	matriz[PosY+i][PosX+j] = cor;
			}
        }
    }
}

/*
Baseando se na matriz do formato do tetromino, essa função busca
na matriz de colisão se há alguma parede na mesma posição caso o
tetromino se movesse para as coordenadas novaPosX e novaPosY.
Parametros: 
int matrizColisao[][] : matriz de colisao a ser consultada
Tetromino *tetromino : ponteiro apontando para o tipo de tetromino a ser movido
int novaPosX e int novaPosY : coordenadas da possível nova posição

Obs: a posição do tetromino é determinada pela posição da ancora em relação
a matriz de colisão
*/
bool TestarColisao(int matrizColisao[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], 
Tetromino *tetromino, int novaPosX, int novaPosY)
{	
	int i;
  	int j;

  	for (i = 0; i < COLUNAS_TETROMINO; i++) 
	{
		for (j = 0; j < LINHAS_TETROMINO; j++) 
		{
			if(matrizColisao[novaPosY+i][novaPosX+j] == 1 && tetromino->formato[i][j] == 1) 
			{
				//colisão
				return true;
			}
		}
    }
	//não há colisão
	return false;	
}

//TODO limpa a linha do tabuleiro ao completar
// void limpaLinha()
// {

// }

//imprime a matriz no console
void PrintMatriz(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO]) 
{
	int i;
	int j;

	for (i = 0; i < COLUNAS_TABULEIRO; i++) 
	{
		printf("\n");
		for (j = 0; j < LINHAS_TABULEIRO; j++) 
		{
			printf("%d", matriz[i][j]);
		}
	}
}

//!!IGNORAR!!
// void exibe_tetromino(Tetromino* tetromino) {
//   int i;
//   int j;
//   for (i = 0; i < COLUNAS_TETROMINO; i++) {
//     for (j = 0; j < LINHAS_TETROMINO; j++) {
// 		if (tetromino->formato[i][j] == 1)
// 		{
// 			// video_box(100+(i*20), 100+(j*20), 120+(i*20), 120+(j*20), tetromino->cor);
// 		}
//     }
//   }
// }

// função para gerar delay, parametro é dado em segundos
void delay(int segundos)
{
	// converter segundos para microsegundos
    int microSegundos = 1000000 * segundos;

    // tempo inicial
    clock_t start_time = clock();

	// loop até o delay necessário
    while (clock() < start_time + microSegundos);
}

