//Código público no github que pegamos emprestado para fins didáticos.
//O produto final será completamente distinto e original.

/* Criação de um jogo inspirado em Tetris para ser executado em uma DE1-SoC
 - Os tetrominos serão movimentados pelo jogador por meio do acelerômetro
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
#include <stdlib.h>

//TODO: PESQUISAR
//Variável global para encerrar o programa

// volatile sig_atomic_t sair; 
// void catchSIGINT(int signum) { sair = 1; }

#define video_BLACK 0x00
#define LARGURA_TELA 319 // Tamanho da tela VGA
#define ALTURA_TELA 239 // Tamanho da tela VGA
#define LINHAS_TETROMINO 2 // Largura máxima do bloco
#define COLUNAS_TETROMINO 4 // Altura máxima do bloco
#define SPAWN_BLOCK_X 4 //Coordenada X de surgimento do bloco
#define SPAWN_BLOCK_Y 0 //Coordenada Y de surgimento do bloco
#define LINHAS_TABULEIRO 15 // Quantidade de colunas de blocos 
#define COLUNAS_TABULEIRO 10 // Quantidade de linhas de blocos
#define MAX_LINHAS_LIMPAR 2 // imite maximo de linhas que podem ser limpas
#define QUANTIDADE_TETROMINOS 4 //Quantidade de tetrominos no jogo

// Tetromino é o nome das peças do tetris :)
typedef struct {
  	int formato[LINHAS_TETROMINO][COLUNAS_TETROMINO]; //0 não tem bloco, 1 tem bloco
  	short cor;        // Cor do bloco
} Tetromino;

typedef struct{
	short cor;
	bool solido;
} Celula;

//TETROMINOS

/*
Desenho:
0110
0110
*/
	Tetromino tetrominoO = {{{0,1,1,0}, {0,1,1,0}}, 0};
/*0001
  0111*/
	Tetromino tetrominoL = {{{0,0,0,1}, {0,1,1,1}}, 1};
/*1000
  1110*/
	Tetromino tetrominoJ = {{{1,0,0,0}, {1,1,1,0}}, 2};
/*1111
  0000*/
	Tetromino tetrominoI = {{{1,1,1,1}, {0,0,0,0}}, 3};

	int tetrominoFlutuanteX; // Coordenada X atual do tetromino  
	int tetrominoFlutuanteY; // Coordenada X atual do tetromino  
	Tetromino tetrominoFlutuanteTipo; // Coordenada X atual do tetromino  

void IniciarMapaColisao(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]);
void imprimirMatriz(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]);
bool TestarColisao(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int x, int y);	
bool Mover(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int direcao, int atualPosX, int atualPosY);
void TranslacaoTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int atualPosX, int atualPosY, int novaPosX, int novaPosY);
void DesenharTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY, int cor);
void LimpaLinhas(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[MAX_LINHAS_LIMPAR]);
void VerificaLinhaCheia(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[MAX_LINHAS_LIMPAR]);
bool TemInput(int *direcao);

int main() {
	//Setup
	srand(time(NULL)); 

	int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO];
	IniciarMapaColisao(matriz); //inicia mapa vazio do jogo
	bool pecaFlutuanteExiste = false;
	bool gameOver = false;

	Tetromino listaTetrominos[QUANTIDADE_TETROMINOS] = {tetrominoO, tetrominoL, tetrominoJ, tetrominoI};
	int linhasCheias[MAX_LINHAS_LIMPAR];
	int cooldownGravidade = 0;
	int cooldownMovimento = 0;
	int direcao = 0;

	//Jogo iniciou
	while(true)
	{
		//o loop será executado 60 vezes em um segundo
		delay(1/60);

		//signal(SIGINT, catchSIGINT);
		if (KEY_open() == 0 || video_open() == 0 || accel_open() == 0) {
		    printf("Erro na inicialização de periféricos.\n");
		    return -1;
		}

		printf("Periféricos inicializados.\n");

		if (pecaFlutuanteExiste)
		{
			//Gravidade
			if (cooldownGravidade == 60)
			{
				//mover para baixo
				if (Mover(matriz, &tetrominoFlutuanteTipo,
					0, tetrominoFlutuanteX, tetrominoFlutuanteY))
				{
					//peça foi congelada
					pecaFlutuanteExiste = false;
				}
				cooldownGravidade = 0;
			}
			else 
			{
				cooldownGravidade = cooldownGravidade + 1;
			}

			//Movimento
			if (cooldownMovimento == 30)
			{
				//usar a funcao TemInput para acessar o acelerometro
				//retornar booleano indicando se há input do jogador
				//definir direcao como 1 ou -1
				if (TemInput(&direcao))
				{
					Mover(matriz, &tetrominoFlutuanteTipo,
						direcao, tetrominoFlutuanteX, tetrominoFlutuanteY);
					
					cooldownMovimento = 0;
				}
			}
			else 
			{
				cooldownMovimento = cooldownMovimento + 1;
			}
		}
		else 
		{
			VerificaLinhaCheia(matriz, linhasCheias);
			LimpaLinhas(matriz, linhasCheias);

			int indexAleatorio = rand() % QUANTIDADE_TETROMINOS;
			tetrominoFlutuanteTipo = listaTetrominos[indexAleatorio];
			tetrominoFlutuanteX = SPAWN_BLOCK_X;
			tetrominoFlutuanteY = SPAWN_BLOCK_Y;

			if (TestarColisao(matriz, &tetrominoFlutuanteTipo, tetrominoFlutuanteX, tetrominoFlutuanteY))
			{
				// Colisão no surgimento do tetromino
				gameOver = true;
			}
		}

		// video_erase();
		// video_clear();
		// video_show();
	}
	//Testes

	int indexAleatorio = rand() % QUANTIDADE_TETROMINOS;

	DesenharTetromino(matriz, &listaTetrominos[indexAleatorio], 5, 5, 1);
	imprimirMatriz(matriz);
	printf("\n");

	int i;
	for (i = 0; i < COLUNAS_TABULEIRO; i++) {
		matriz[7][i] = 1;
		matriz[8][i] = 1;
	}

	imprimirMatriz(matriz);
	printf("\n");
	
	int linhasCheias[MAX_LINHAS_LIMPAR];
	VerificaLinhaCheia(matriz, linhasCheias);

	for (i = 0; i < MAX_LINHAS_LIMPAR; i++) {
		printf("%d",linhasCheias[i]);
	}

	LimpaLinhas(matriz, linhasCheias);
	imprimirMatriz(matriz);
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

	// Inicialização e calibração do acelerômetro
	// accel_init();
	// accel_format(1, 2);
	// accel_calibrate();
 	// int botoes, quant_blocos;

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
void IniciarMapaColisao(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO])
{
	int i;
	int j;

	for(i = 0; i < LINHAS_TABULEIRO; i++) 
	{
		for(j = 0; j < COLUNAS_TABULEIRO; j++) 
		{
			if(j == 0 || j == COLUNAS_TABULEIRO-1 || i == LINHAS_TABULEIRO-1) 
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
bool Mover(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetrominoFlutuanteTipo, int direcao, int tetrominoFlutuanteX, int tetrominoFlutuanteY) {

    if(direcao == 0)
    {
        if(TestarColisao(matriz, tetrominoFlutuanteTipo, tetrominoFlutuanteX, tetrominoFlutuanteY-1))
        {
            //Há colisão e o movimento não foi realizado
			//congelar o tetromino
			DesenharTetromino(matriz, tetrominoFlutuanteTipo, tetrominoFlutuanteX, tetrominoFlutuanteY, 1);
            return false;
        }
        else
        {
            //Não há colisão, movimento realizado
            //TranslacaoTetromino(matriz, tetromino, tetrominoFlutuanteX, tetrominoFlutuanteY, tetrominoFlutuanteX, tetrominoFlutuanteY-1);
			tetrominoFlutuanteY = tetrominoFlutuanteY - 1;
            return true;
        }
    }
    else
    {
        if(TestarColisao(matriz, tetrominoFlutuanteTipo, tetrominoFlutuanteX + direcao, tetrominoFlutuanteY))
        {
            //Há colisão e o movimento não foi realizado
            return false;
        }
        else
        {
            //Não há colisão, movimento realizado
            //TranslacaoTetromino(matriz, tetromino, tetrominoFlutuanteX, tetrominoFlutuanteY, tetrominoFlutuanteX + direcao, tetrominoFlutuanteY);
			tetrominoFlutuanteX = tetrominoFlutuanteX + direcao;
            return true;
        }
    }
}

void TranslacaoTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int atualPosX, int atualPosY, int novaPosX, int novaPosY)  
{
    //Apagar posição anterior
    DesenharTetromino(matriz, tetromino, atualPosX, atualPosY, 0);
    //Desenhar nova posição
    DesenharTetromino(matriz, tetromino, novaPosX, novaPosY, 2);
}
    

void DesenharTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY, int cor) 
{
    int i;
    int j;
    for(i = 0; i < LINHAS_TETROMINO; i++)
    {
        for(j = 0; j < COLUNAS_TETROMINO; j++)
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
bool TestarColisao(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], 
Tetromino *tetromino, int novaPosX, int novaPosY)
{	
	int i;
  	int j;

  	for (i = 0; i < LINHAS_TETROMINO; i++) 
	{
		for (j = 0; j < COLUNAS_TETROMINO; j++) 
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

void VerificaLinhaCheia(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[MAX_LINHAS_LIMPAR])
{
	int linha = -1;
	int i;
	int j;
	bool cheia;

	//limpar lista de linhas a serem limpadas
	for(i = 0; i < MAX_LINHAS_LIMPAR; i++)
	{
		linhasCheias[i] = -1;
	}

	int indexLinhasCheias = 0;

	//buscar na matriz por linhas cheias
	i = 0;
	for(i = 0; i < LINHAS_TABULEIRO-1; i++)
	{
		cheia = true;
		for(j = 1; j < COLUNAS_TABULEIRO-1; j++)
		{
			if(matrizColisao[i][j] == 0)
			{
				cheia = false;
				break;
			}

		}

		//caso encontre: adicionar o index da linha a lista de linhas cheiasn
		if(cheia)
		{
			linhasCheias[indexLinhasCheias] = i;
			indexLinhasCheias ++;
		}
		
		if (indexLinhasCheias == MAX_LINHAS_LIMPAR)
		{
			break;
		}
	}
}

//limpa a linha do tabuleiro ao completar
void LimpaLinhas(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[MAX_LINHAS_LIMPAR])
{
	int i;
	int j;
	int k;
	
	//percorre todas as linhas da lista de linhas para tirar
	for(k = 0; k < MAX_LINHAS_LIMPAR; k++)
	{
	//verifica se a linha é valida
		if(linhas[k]>=0)
		{
			//percorre o tabuleiro de baixo pra cima, a partir da linha a ser excluida		
			for(i = linhas[k]; i > 0; i--)
			{
				for (j = 0; j < COLUNAS_TABULEIRO; j++)
				{
					//copia os elementos da linha de cima para a linha de baixo
					matrizColisao[i][j] = matrizColisao[i-1][j];
				}
			}
			j = 0;
			//define as bordas laterais do  tabuleiro e a primeira linha
			for (j = 1; j < COLUNAS_TABULEIRO-1; j++)
			{
				matrizColisao[0][j] = 0;
			}
			matrizColisao[0][0] = 1;
			matrizColisao[0][COLUNAS_TABULEIRO] = 1;
		}
	}
}

//imprime a matriz no console
void imprimirMatriz(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]) 
{
	int i;
	int j;

	for (i = 0; i < LINHAS_TABULEIRO; i++) 
	{
		printf("\n");
		for (j = 0; j < COLUNAS_TABULEIRO; j++) 
		{
			printf("%d", matriz[i][j]);
			// video_box(100+(i*20), 100+(j*20), 120+(i*20), 120+(j*20), tetromino->cor);
		}
	}
}

// void ExibeTetrominoFlutuante(Tetromino *tetrominoFlutuanteTipo, int tetrominoFlutuanteX, int tetrominoFlutuanteY) 
// {
// 	video_box(100+(tetrominoFlutuanteY*20), 100+(tetrominoFlutuanteX*20), 
// 	120+(tetrominoFlutuanteY*20), 120+(tetrominoFlutuanteX*20), tetrominoFlutuanteTipo->cor);
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

