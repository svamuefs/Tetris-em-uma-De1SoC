//Código públic0 no github que pegamos emprestado para fins didáticos.
//O produto final será completamente distinto e original.

/* Criação de um jogo inspirado em Tetris para ser executado em uma DE1-SoC
 - Os tetrominos serão movimentados pelo jogador por meio do acelerômetro
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
#include <time.h>
#include <stdlib.h>
#include <string.h>

//TODO: PESQUISAR
//Variável global para encerrar o programa

// volatile sig_atomic_t sair; 
// void catchSIGINT(int signum) { sair = 1; }

#define video_BLACK 0x00
#define LARGURA_TELA 319 // Tamanho da tela VGA
#define ALTURA_TELA 239 // Tamanho da tela VGA
#define LINHAS_TETROMINO 4 // Largura máxima do tetromino
#define COLUNAS_TETROMINO 4 // Altura máxima do tetromino
#define SPAWN_BLOCK_X 3 //Coordenada X de surgimento do tetromino
#define SPAWN_BLOCK_Y 0 //Coordenada Y de surgimento do tetromino
#define LINHAS_TABULEIRO 15 // Quantidade de colunas de blocos no tabuleiro
#define COLUNAS_TABULEIRO 10 // Quantidade de linhas de blocos no tabuleiro
#define MAX_LINHAS_LIMPAR 4 // imite maximo de linhas que podem ser limpas
#define QUANTIDADE_TETROMINOS 7 // Quantidade de tetrominos no jogo
#define QUADRADO_LADO 10 // Tamanho em pixels do lado dos quadrados a serem mostrados na tela
#define TICKS 60// Quantas vezes a main é executada em um único segundo
#define COOLDOWN_GRAVIDADE 90 // Tempo em ticks do cooldown da gravidade
#define COOLDOWN_INPUT 30 // Tempo em ticks do cooldown para input do jogador

// Tetromino é o nome das peças do tetris :)
typedef struct {
  	int formato[LINHAS_TETROMINO][COLUNAS_TETROMINO]; //0 não tem bloco, 1 tem bloco
  	int cor;        // Cor do bloco, o index da lista de cores abaixo
} Tetromino;

short cores[] = {video_BLACK, video_WHITE, video_YELLOW, video_ORANGE, video_BLUE, video_CYAN,
				 video_MAGENTA, video_GREEN, video_RED};

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

	int tetrominoFlutuanteX; // Coordenada X atual do tetromino  
	int tetrominoFlutuanteY; // Coordenada X atual do tetromino  
	Tetromino tetrominoFlutuanteTipo; // Coordenada X atual do tetromino  

void IniciarMapaColisao(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]);
void imprimirMatriz(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], short cores[]);
bool TestarColisao(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int x, int y);	
bool Mover(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int direcao, int *atualPosX, int *atualPosY);
// void TranslacaoTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int atualPosX, int atualPosY, int novaPosX, int novaPosY);
void DesenharTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY);
void LimpaLinhas(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[MAX_LINHAS_LIMPAR]);
void VerificaLinhaCheia(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[MAX_LINHAS_LIMPAR]);
bool TemInput(int *direcao);
void interpreta_botoes(int *botoes, bool *pause, bool *reset, int *sentido);
void delay(int segundos);
void ExibeTetrominoFlutuante(Tetromino *tetrominoFlutuanteTipo, int tetrominoFlutuanteX, int tetrominoFlutuanteY, short cores[]); 
void rotacaoTetromino(Tetromino *tetrominoFlutuanteTipo, int sentido);


int main() {
	//Setup
	srand(time(NULL)); 
	if (KEY_open() == 0 || video_open() == 0 || accel_open() == 0) {
		printf("Erro na inicialização de periféricos.\n");
		return -1;
	}

	int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO];
	IniciarMapaColisao(matriz); //inicia mapa vazio do jogo
	bool pecaFlutuanteExiste = false;
	bool gameOver = false;

	Tetromino listaTetrominos[QUANTIDADE_TETROMINOS] = {tetrominoO, tetrominoL, tetrominoJ, tetrominoI, tetrominoS, tetrominoZ, tetrominoT};
	int linhasCheias[MAX_LINHAS_LIMPAR];
	int cooldownGravidade = 0;
	int cooldownMovimento = 0;
	int direcao = 0;

	//Movimento
	int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;
	bool movimentoRealizado;
	int sentido;

	accel_init();
    accel_format(1, 2);
    accel_calibrate();

	//Jogo iniciou
	int botoes;
    bool pause = false, reset = false, fim_de_jogo = false, vitoria = false, derrota = false; // Variáveis de controle

	int nLoops = 0;
	//teste
	// // int sairLoop = 0;
	// int i;
	// for (i = 0; i < 5; i++)
	// {
	
	// int indexAleatorio = rand() % QUANTIDADE_TETROMINOS;
	// tetrominoFlutuanteTipo = listaTetrominos[indexAleatorio];
	// tetrominoFlutuanteX = SPAWN_BLOCK_X;
	// tetrominoFlutuanteY = SPAWN_BLOCK_Y;

	// nLoops = 0;
	
	//teste
	// int j;
	// for (j = 0; j < COLUNAS_TABULEIRO; j++) {
	// 	matriz[18][j] = 1;
	// 	matriz[19][j] = 1;
	// }
	
	while(nLoops != 100000)
	{
		printf("Loop: %d \n", nLoops);
		nLoops ++;

		// if (nLoops%10 == 0)
		// {
		// 	printf("Sair do loop?\n");
		// 	scanf("%d", &sairLoop);
		// }
		
		//o loop será executado TICKS vezes em um segundo
		delay(1/TICKS);

		//signal(SIGINT, catchSIGINT);

		video_erase();
		video_clear();

		if (pecaFlutuanteExiste)
		{
			printf("Peça flutuante existe\n");
			
			//Gravidade
			if (cooldownGravidade == COOLDOWN_GRAVIDADE)
			{
				printf("Gravidade Ativada\n");
				//mover para baixo
				movimentoRealizado = Mover(matriz, &tetrominoFlutuanteTipo,
					0, &tetrominoFlutuanteX, &tetrominoFlutuanteY);

				if (!movimentoRealizado)
				{
					printf("Peça Congelada\n");
					//peça foi congelada
					pecaFlutuanteExiste = false;
				}
				cooldownGravidade = 0;
			}
			else 
			{
				printf("Gravidade NÃO Ativada\n");
				cooldownGravidade++;
			}

			//Movimento
			if (cooldownMovimento == COOLDOWN_INPUT)
			{
				printf("Movimento Permitido\n");
				//usar a funcao TemInput para acessar o acelerometro
				//retornar booleano indicando se há input do jogador
				//definir direcao como 1 ou -1

				accel_read(&acel_rdy, &acel_tap, &acel_dtap, &acel_x, &acel_y, &acel_z,&acel_mg);
				printf("\n acel_rdy: %d \n X: %d \n acel_tap: %d \n acel_dtap: %d \n", acel_rdy, acel_tap, acel_dtap, acel_x);

				if (acel_x < -25)
				{
					direcao = -1;
					cooldownMovimento = 0;
				} 
				else if (acel_x > 25) 
				{
					direcao = 1;
					cooldownMovimento = 0;
				} 
				else 
				{
					direcao = 0;
				}

				Mover(matriz, &tetrominoFlutuanteTipo,
					direcao, &tetrominoFlutuanteX, &tetrominoFlutuanteY);

			}
			else 
			{
				printf("Movimento NÃO Permitido\n");
				cooldownMovimento++;
			}
		}
		else 
		{
			printf("Peça flutuante NÃO existe\n");
			VerificaLinhaCheia(matriz, linhasCheias);
			LimpaLinhas(matriz, linhasCheias);

			int indexAleatorio = rand() % QUANTIDADE_TETROMINOS;
			memcpy(&tetrominoFlutuanteTipo, &listaTetrominos[indexAleatorio], sizeof(listaTetrominos[indexAleatorio]));
			tetrominoFlutuanteX = SPAWN_BLOCK_X;
			tetrominoFlutuanteY = SPAWN_BLOCK_Y;

			if (TestarColisao(matriz, &tetrominoFlutuanteTipo, tetrominoFlutuanteX, tetrominoFlutuanteY))
			{
				// Colisão no surgimento do tetromino
				gameOver = true;
				printf("GameOver\n");

			}
			else
			{
				pecaFlutuanteExiste = true;
			}
		}

		interpreta_botoes(&botoes, &pause, &reset, &sentido);
		rotacaoTetromino(&tetrominoFlutuanteTipo, sentido);

		imprimirMatriz(matriz, cores);
		ExibeTetrominoFlutuante(&tetrominoFlutuanteTipo, tetrominoFlutuanteX, tetrominoFlutuanteY, cores);
		
		// Mover(matriz, &tetrominoFlutuanteTipo, 0,
		// 	&tetrominoFlutuanteX, &tetrominoFlutuanteY);

		// printf("\n X: %d ", tetrominoFlutuanteX);
		// printf(" Y: %d \n", tetrominoFlutuanteY);

		// VerificaLinhaCheia(matriz, linhasCheias);
		// LimpaLinhas(matriz, linhasCheias);

		video_show();
		printf("Fim\n");
	}

    // // signal(SIGINT, catchSIGINT);

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
void interpreta_botoes(int *botoes, bool *pause, bool *reset, int *sentido) {
	KEY_read(botoes);
	*sentido = 0;
	if (*botoes == 8) {
		if (*pause == false)
		*pause = true;
		else
		*pause = false;
	}
	else if (*botoes == 4)
	{
		*reset = true;
	}
	else if (*botoes == 2)
	{
		*sentido = -1;
	}
	else if (*botoes == 1)
	{
		*sentido = 1;
	}
}
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
	printf("IniciarMapaColisão()");
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
bool Mover(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetrominoFlutuanteTipo, int direcao, int *tetrominoFlutuanteX, int *tetrominoFlutuanteY) 
{
	printf("Mover()");

    if(direcao == 0)
    {
        if(TestarColisao(matriz, tetrominoFlutuanteTipo, *tetrominoFlutuanteX, *tetrominoFlutuanteY+1))
        {
			printf("1");
            //Há colisão e o movimento não foi realizado
			//congelar o tetromino
			DesenharTetromino(matriz, tetrominoFlutuanteTipo, *tetrominoFlutuanteX, *tetrominoFlutuanteY);
            return false;
        }
        else
        {
			printf("2");
            //Não há colisão, movimento realizado
            //TranslacaoTetromino(matriz, tetromino, tetrominoFlutuanteX, tetrominoFlutuanteY, tetrominoFlutuanteX, tetrominoFlutuanteY-1);
			*tetrominoFlutuanteY = *tetrominoFlutuanteY + 1;
            return true;
        }
    }
    else
    {
        if(TestarColisao(matriz, tetrominoFlutuanteTipo, *tetrominoFlutuanteX + direcao, *tetrominoFlutuanteY))
        {
			printf("3");
            //Há colisão e o movimento não foi realizado
            return false;
        }
        else
        {
			printf("4");
            //Não há colisão, movimento realizado
            //TranslacaoTetromino(matriz, tetromino, tetrominoFlutuanteX, tetrominoFlutuanteY, tetrominoFlutuanteX + direcao, tetrominoFlutuanteY);
			*tetrominoFlutuanteX = *tetrominoFlutuanteX + direcao;
            return true;
        }
    }
}

/*
Sentido horario : sentido = 1
Sentido anti-horario : sentido = 0
*/
void rotacaoTetromino(Tetromino *tetrominoFlutuanteTipo, int sentido)
{
	if (sentido != 0)
	{
		printf("rotacaoTetromino(), %d", sentido);
		int i;
		int j;

		int matrizTemp[LINHAS_TETROMINO][COLUNAS_TETROMINO];

		memcpy(&matrizTemp, tetrominoFlutuanteTipo->formato, sizeof(tetrominoFlutuanteTipo->formato));

		for(i = 0; i < LINHAS_TETROMINO; i++)
		{
			for(j = 0; j < COLUNAS_TETROMINO; j++)
			{	
				if (sentido == 1)
				{
					tetrominoFlutuanteTipo->formato[COLUNAS_TETROMINO-j-1][i] = matrizTemp[i][j];
				}
				else if (sentido == -1)
				{
					tetrominoFlutuanteTipo->formato[j][LINHAS_TETROMINO-i-1] = matrizTemp[i][j];
				}
				
			}
		}
	}
}

// void TranslacaoTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int atualPosX, int atualPosY, int novaPosX, int novaPosY)  
// {
// 	printf("TranslacaoTetromino()");
//     //Apagar posição anterior
//     DesenharTetromino(matriz, tetromino, atualPosX, atualPosY, 0);
//     //Desenhar nova posição
//     DesenharTetromino(matriz, tetromino, novaPosX, novaPosY, 2);
// }


void DesenharTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY) 
{
	printf("DesenharTetromino()");
    int i;
    int j;
    for(i = 0; i < LINHAS_TETROMINO; i++)
    {
        for(j = 0; j < COLUNAS_TETROMINO; j++)
        {
			if(tetromino->formato[i][j] == 1)
			{
            	matriz[PosY+i][PosX+j] = tetromino->cor;
			}
        }
    }
}

/*
1000000001
1001111001
1111111111
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
	printf("TestarColisao()");
	int i;
  	int j;

  	for (i = 0; i < LINHAS_TETROMINO; i++) 
	{
		for (j = 0; j < COLUNAS_TETROMINO; j++) 
		{
			if(matrizColisao[novaPosY+i][novaPosX+j] > 0 && tetromino->formato[i][j] == 1) 
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
	printf("VerificaLinhaCheia()");
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
	printf("LimpaLinhas()");
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
void imprimirMatriz(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], short cores[]) 
{
	printf("imprimirMatriz()");
	int i;
	int j;

	for (i = 0; i < LINHAS_TABULEIRO; i++) 
	{
		printf("\n");
		for (j = 0; j < COLUNAS_TABULEIRO; j++) 
		{

			if (matriz[i][j] > 0)
			{
				video_box((j*QUADRADO_LADO), (i*QUADRADO_LADO),
					QUADRADO_LADO+(j*QUADRADO_LADO), 
					QUADRADO_LADO+(i*QUADRADO_LADO), cores[matriz[i][j]]);
			}
			printf("%d", matriz[i][j]);
		}
	}
}

void ExibeTetrominoFlutuante(Tetromino *tetrominoFlutuanteTipo, int tetrominoFlutuanteX, int tetrominoFlutuanteY, short cores[]) 
{
	printf("ExibeTetrominoFlutuante()");
	int i;
	int j;

	for (i = 0; i < LINHAS_TETROMINO; i++) 
	{
		printf("\n");
		for (j = 0; j < COLUNAS_TETROMINO; j++) 
		{
			if (tetrominoFlutuanteTipo->formato[i][j])
			{
				video_box((tetrominoFlutuanteX+j)*QUADRADO_LADO, (tetrominoFlutuanteY+i)*QUADRADO_LADO,
					QUADRADO_LADO+((tetrominoFlutuanteX+j)*QUADRADO_LADO), 
					QUADRADO_LADO+((tetrominoFlutuanteY+i)*QUADRADO_LADO), cores[tetrominoFlutuanteTipo->cor]);
			}
		}
	}
}

// função para gerar delay, parametro é dado em segundos
void delay(int segundos)
{	
	printf("delay()");
	// converter segundos para microsegundos
    int microSegundos = 1000000 * segundos;

    // tempo inicial
    clock_t start_time = clock();

	// loop até o delay necessário
    while (clock() < start_time + microSegundos);
}
