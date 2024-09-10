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
#define BLOCOS_POR_PECA 4 // Quantidade de blocos que compõe as peças
#define SPAWN_BLOCK_X 3 //Coordenada X de surgimento do tetromino
#define SPAWN_BLOCK_Y 0 //Coordenada Y de surgimento do tetromino
#define LINHAS_TABULEIRO 15 // Quantidade de colunas de blocos no tabuleiro, contando com chão
#define COLUNAS_TABULEIRO 12 // Quantidade de linhas de blocos no tabuleiro, contando com paredes
#define QUANTIDADE_TETROMINOS 7 // Quantidade de tetrominos no jogo
#define QUADRADO_LADO 10 // Tamanho em pixels do lado dos quadrados a serem mostrados na tela
#define TICKS 60// Quantas vezes a main é executada em um único segundo
#define COOLDOWN_GRAVIDADE 2 // Tempo em ticks do cooldown da gravidade
#define COOLDOWN_INPUT 12 // Tempo em ticks do cooldown para input do jogador
#define INPUT_INCLINACAO 30 // Inclinação necessaria para aceitar o input do jogador
#define GAMEOVER_TEXT "     ░▒▓██████▓▒░   ░▒▓██████▓▒░  ░▒▓██████████████▓▒░  ░▒▓████████▓▒░        ░▒▓██████▓▒░  ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓████████▓▒░ ░▒▓███████▓▒░  \n    ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░              ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░        ░▒▓█▓▒░░▒▓█▓▒░ \n    ░▒▓█▓▒░        ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░              ░▒▓█▓▒░░▒▓█▓▒░  ░▒▓█▓▒▒▓█▓▒░  ░▒▓█▓▒░        ░▒▓█▓▒░░▒▓█▓▒░ \n    ░▒▓█▓▒▒▓███▓▒░ ░▒▓████████▓▒░ ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ ░▒▓██████▓▒░         ░▒▓█▓▒░░▒▓█▓▒░  ░▒▓█▓▒▒▓█▓▒░  ░▒▓██████▓▒░   ░▒▓███████▓▒░  \n    ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░              ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▓█▓▒░   ░▒▓█▓▒░        ░▒▓█▓▒░░▒▓█▓▒░ \n    ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░              ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▓█▓▒░   ░▒▓█▓▒░        ░▒▓█▓▒░░▒▓█▓▒░ \n     ░▒▓██████▓▒░  ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ ░▒▓████████▓▒░        ░▒▓██████▓▒░     ░▒▓██▓▒░    ░▒▓████████▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ \n"                                                                                                                                        

// Tetromino é o nome das peças do tetris :)
typedef struct {
  	int formato[BLOCOS_POR_PECA][BLOCOS_POR_PECA]; //0 não tem bloco, 1 tem bloco
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
void DesenharTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY);
void LimpaLinhas(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[BLOCOS_POR_PECA]);
void VerificaLinhaCheia(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[BLOCOS_POR_PECA]);
bool TemInput(int *direcao);
void interpreta_botoes(int *botoes, bool *pause, bool *reset, int *sentido);
void delay(int segundos);
void ExibeTetrominoFlutuante(Tetromino *tetrominoFlutuanteTipo, int tetrominoFlutuanteX, int tetrominoFlutuanteY, short cores[]); 
void rotacaoTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetrominoFlutuanteTipo, int sentido);
void GerarTetromino(Tetromino listaTetrominos[QUANTIDADE_TETROMINOS]);

int main() {
	//Setup
	srand(time(NULL)); 
	if (KEY_open() == 0 || video_open() == 0 || accel_open() == 0) {
		//printf("Erro na inicialização de periféricos.\n");
		return -1;
	}

	int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO];
	IniciarMapaColisao(matriz); //inicia mapa vazio do jogo
	bool pecaFlutuanteExiste = false;
	bool gameOver = false;

	Tetromino listaTetrominos[] = {tetrominoO, tetrominoL, tetrominoJ, tetrominoI, tetrominoS, tetrominoZ, tetrominoT};
	int quantidadeTetrominos = sizeof(listaTetrominos)/sizeof(listaTetrominos[0]);
	int linhasCheias[BLOCOS_POR_PECA];
	int cooldownGravidade = 0;
	int cooldownMovimento = 0;

	//Movimento
	int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;
	int sentido;

	accel_init();
    accel_format(1, 2);
    accel_calibrate();

	//Jogo iniciou
	int botoes;
    bool pause = false, reset = false, fim_de_jogo = false, vitoria = false, derrota = false; // Variáveis de controle

	int nLoops = 0;

	while(nLoops != 100000)
	{
		delay(1);
		nLoops ++;
		if (reset)
		{
			IniciarMapaColisao(matriz); //inicia mapa vazio do jogo		
			pecaFlutuanteExiste = false;
			reset = false;
		}
		
		if (pause)
		{
			interpreta_botoes(&botoes, &pause, &reset, &sentido);
			if (gameOver)
			{
				video_text(10, 10, "Aa_-$#|\\/;.,<>");
			}
			
		}
		
		while (!reset && !pause)
		{

			//printf("Loop: %d \n", nLoops);

			// if (nLoops%10 == 0)
			// {
			// 	//printf("Sair do loop?\n");
			// 	scanf("%d", &sairLoop);
			// }
			
			//o loop será executado TICKS vezes em um segundo
			delay(1/TICKS);

			//signal(SIGINT, catchSIGINT);

			video_erase();
			video_clear();

			if (pecaFlutuanteExiste)
			{
				//printf("Peça flutuante existe\n");
				
				//Gravidade
				if (cooldownGravidade == COOLDOWN_GRAVIDADE)
				{
					//printf("%d", cooldownGravidade);
					//printf("Gravidade Ativada\n");
					//mover para baixo
					if (!Mover(matriz, &tetrominoFlutuanteTipo,
						0, &tetrominoFlutuanteX, &tetrominoFlutuanteY))
					{
						//printf("Peça Congelada\n");
						//peça foi congelada
						pecaFlutuanteExiste = false;
					}
					cooldownGravidade = 0;
				}
				else 
				{
					//printf("Gravidade NÃO Ativada\n");
					cooldownGravidade++;
				}

				//Movimento
				if (cooldownMovimento == COOLDOWN_INPUT)
				{
					//printf("Movimento Permitido\n");
					//usar a funcao TemInput para acessar o acelerometro
					//retornar booleano indicando se há input do jogador
					//definir direcao como 1 ou -1

					accel_read(&acel_rdy, &acel_tap, &acel_dtap, &acel_x, &acel_y, &acel_z,&acel_mg);
					//printf("\n acel_rdy: %d \n X: %d \n acel_tap: %d \n acel_dtap: %d \n", acel_rdy, acel_tap, acel_dtap, acel_x);

					if (acel_x < -INPUT_INCLINACAO)
					{
						Mover(matriz, &tetrominoFlutuanteTipo,
						-1, &tetrominoFlutuanteX, &tetrominoFlutuanteY);
						cooldownMovimento = 0;
					} 
					else if (acel_x > INPUT_INCLINACAO) 
					{
						Mover(matriz, &tetrominoFlutuanteTipo,
						1, &tetrominoFlutuanteX, &tetrominoFlutuanteY);
						cooldownMovimento = 0;
					} 

				}
				else 
				{
					//printf("Movimento NÃO Permitido\n");
					cooldownMovimento++;
				}
			}
			else 
			{
				//printf("Peça flutuante NÃO existe\n");
				VerificaLinhaCheia(matriz, linhasCheias);
				LimpaLinhas(matriz, linhasCheias);

				// int indexAleatorio = rand() % quantidadeTetrominos;
				// memcpy(&tetrominoFlutuanteTipo, &listaTetrominos[indexAleatorio], sizeof(listaTetrominos[indexAleatorio]));
				// tetrominoFlutuanteX = SPAWN_BLOCK_X;
				// tetrominoFlutuanteY = SPAWN_BLOCK_Y;

				GerarTetromino(listaTetrominos);

				if (TestarColisao(matriz, &tetrominoFlutuanteTipo, tetrominoFlutuanteX, tetrominoFlutuanteY))
				{
					// Colisão no surgimento do tetromino
					gameOver = true;
					reset = true;
					pause = true;
					printf("GameOver\n");

				}
				else
				{
					pecaFlutuanteExiste = true;
				}
			}

			interpreta_botoes(&botoes, &pause, &reset, &sentido);
			rotacaoTetromino(matriz, &tetrominoFlutuanteTipo, sentido);

			imprimirMatriz(matriz, cores);
			ExibeTetrominoFlutuante(&tetrominoFlutuanteTipo, tetrominoFlutuanteX, tetrominoFlutuanteY, cores);

			video_show();
			//printf("Fim\n");
		}
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

void GerarTetromino(Tetromino listaTetrominos[QUANTIDADE_TETROMINOS])
{
	int indexAleatorio = rand() % QUANTIDADE_TETROMINOS;
	memcpy(&tetrominoFlutuanteTipo, &listaTetrominos[indexAleatorio], sizeof(listaTetrominos[indexAleatorio]));
	tetrominoFlutuanteX = SPAWN_BLOCK_X;
	tetrominoFlutuanteY = SPAWN_BLOCK_Y;
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
	//printf("IniciarMapaColisão()");
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
	//printf("Mover()");

    if(direcao == 0)
    {
        if(TestarColisao(matriz, tetrominoFlutuanteTipo, *tetrominoFlutuanteX, *tetrominoFlutuanteY+1))
        {
			//printf("1");
            //Há colisão e o movimento não foi realizado
			//congelar o tetromino
			DesenharTetromino(matriz, tetrominoFlutuanteTipo, *tetrominoFlutuanteX, *tetrominoFlutuanteY);
            return false;
        }
        else
        {
			//printf("2");
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
			//printf("3");
            //Há colisão e o movimento não foi realizado
            return false;
        }
        else
        {
			//printf("4");
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
void rotacaoTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetrominoFlutuanteTipo, int sentido)
{
	if (sentido != 0)
	{
		//printf("rotacaoTetromino(), %d", sentido);
		int i;
		int j;

		int matrizTemp[BLOCOS_POR_PECA][BLOCOS_POR_PECA];
		int posXTemp = tetrominoFlutuanteX;
		int posYTemp = tetrominoFlutuanteY;

		memcpy(&matrizTemp, tetrominoFlutuanteTipo->formato, sizeof(tetrominoFlutuanteTipo->formato));

		for(i = 0; i < BLOCOS_POR_PECA; i++)
		{
			for(j = 0; j < BLOCOS_POR_PECA; j++)
			{	
				if (sentido == 1)
				{
					tetrominoFlutuanteTipo->formato[BLOCOS_POR_PECA-j-1][i] = matrizTemp[i][j];
				}
				else if (sentido == -1)
				{
					tetrominoFlutuanteTipo->formato[j][BLOCOS_POR_PECA-i-1] = matrizTemp[i][j];
				}
				
			}
		}

		i = 0;
		while (TestarColisao(matriz, tetrominoFlutuanteTipo, tetrominoFlutuanteX, tetrominoFlutuanteY))
		{
			//da pra melhorar, talvez não seja muito confiavel com tabuleiros de poucas colunas
			if (tetrominoFlutuanteX > COLUNAS_TABULEIRO/2)
			{
				tetrominoFlutuanteX--;
			}
			else
			{
				tetrominoFlutuanteX++;
			}	
			if (i == 2) 
			{
				printf("Não girar");
				tetrominoFlutuanteX = posXTemp;
				tetrominoFlutuanteY = posYTemp;
				memcpy(tetrominoFlutuanteTipo->formato, &matrizTemp, sizeof(tetrominoFlutuanteTipo->formato));
				break;
			}
			i++;
		}
	}
}

void DesenharTetromino(int matriz[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY) 
{
	//printf("DesenharTetromino()");
    int i;
    int j;
    for(i = 0; i < BLOCOS_POR_PECA; i++)
    {
        for(j = 0; j < BLOCOS_POR_PECA; j++)
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
	//printf("TestarColisao()");
	int i;
  	int j;

  	for (i = 0; i < BLOCOS_POR_PECA; i++) 
	{
		for (j = 0; j < BLOCOS_POR_PECA; j++) 
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

void VerificaLinhaCheia(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[BLOCOS_POR_PECA])
{
	//printf("VerificaLinhaCheia()");
	int i;
	int j;
	bool cheia;

	//limpar lista de linhas a serem limpadas
	for(i = 0; i < BLOCOS_POR_PECA; i++)
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
		
		if (indexLinhasCheias == BLOCOS_POR_PECA)
		{
			break;
		}
	}
}

//limpa a linha do tabuleiro ao completar
void LimpaLinhas(int matrizColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[BLOCOS_POR_PECA])
{
	//printf("LimpaLinhas()");
	int i;
	int j;
	int k;
	
	//percorre todas as linhas da lista de linhas para tirar
	for(k = 0; k < BLOCOS_POR_PECA; k++)
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
	//printf("imprimirMatriz()");
	int i;
	int j;

	for (i = 0; i < LINHAS_TABULEIRO; i++) 
	{
		//printf("\n");
		for (j = 0; j < COLUNAS_TABULEIRO; j++) 
		{

			if (matriz[i][j] > 0)
			{
				video_box((j*QUADRADO_LADO), (i*QUADRADO_LADO),
					QUADRADO_LADO+(j*QUADRADO_LADO), 
					QUADRADO_LADO+(i*QUADRADO_LADO), cores[matriz[i][j]]);
			}
			//printf("%d", matriz[i][j]);
		}
	}
}

void ExibeTetrominoFlutuante(Tetromino *tetrominoFlutuanteTipo, int tetrominoFlutuanteX, int tetrominoFlutuanteY, short cores[]) 
{
	//printf("ExibeTetrominoFlutuante()");
	int i;
	int j;

	for (i = 0; i < BLOCOS_POR_PECA; i++) 
	{
		//printf("\n");
		for (j = 0; j < BLOCOS_POR_PECA; j++) 
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
	//printf("delay()");
	// converter segundos para microsegundos
    int microSegundos = 1000000 * segundos;

    // tempo inicial
    clock_t start_time = clock();

	// loop até o delay necessário
    while (clock() < start_time + microSegundos);
}
