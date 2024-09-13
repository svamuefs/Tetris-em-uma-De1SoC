// //Código públic0 no github que pegamos emprestado para fins didáticos.
// //O produto final será completamente distinto e original.

// /* Criação de um jogo inspirado em Tetris para ser executado em uma DE1-SoC
//  - Os tetrominos serão movimentados pelo jogador por meio do acelerômetro
//  - Todos os itens visuais serão exibidos por meio da interface VGA
//  - Botões serão utilizados para: reiniciar, pausar e continuar o jogo
//  - O jogo é encerrado de vez pela utilização de ^C
// */

#include <intelfpgaup/KEY.h> 
#include <intelfpgaup/accel.h>
#include <intelfpgaup/video.h>
#include <intelfpgaup/SW.h>

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "config.c"
					 					
//Funções

//do Sistema
void IniciarTabuleiro(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]);
void Delay(int segundos);
void ImprimirTabuleiro(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]);
void ImprimirTetromino(Tetromino *tetromino, int x, int y); 
void Resetar(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], bool *pecaFlutuanteExiste, Tetromino tetrominoPreview[TAMANHO_PREVIEW]);
void Pause();

//do Jogo
void GerarTetromino(Tetromino *tetromino, Tetromino tetrominoPreview[TAMANHO_PREVIEW]);
void PreencherPreview(Tetromino tetrominoPreview[TAMANHO_PREVIEW]);
bool TestarColisao(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int x, int y);	
void CongelarTetromino(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino);
void VerificaLinhaCheia(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[BLOCOS_POR_PECA]);
void LimpaLinhas(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[BLOCOS_POR_PECA]);

//do Jogador
void ReceberInput(bool *gameOver, bool *hold, bool *duploGiro, int *sentido);
bool Mover(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int direcao);
void RotacaoTetromino(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int sentido);
void Hold(Tetromino *tetromino, Tetromino *hold, bool *canHold, Tetromino tetrominoPreview[TAMANHO_PREVIEW]);

int main() {
	//Setup
	//signal(SIGINT, SIGTERM);
	srand(time(NULL)); 
	if (KEY_open() == 0 || video_open() == 0 || accel_open() == 0 || SW_open() == 0) {
		printf("Erro na inicialização de periféricos.\n");
		return -1;
	}
  
	printf("Inicio do Setup");

	Tetromino tetrominoFlutuante;
	Tetromino tetrominoHold = {{-1}};

	Tetromino tetrominoPreview[TAMANHO_PREVIEW];

	int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO];

	bool pecaFlutuanteExiste = false;
	bool gameOver = false;

	int linhasCheias[BLOCOS_POR_PECA];
	int cooldownGravidade = 0;
	int cooldownMovimento = 0;


	//Configuração do acelerometro
	int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;
	accel_init();
    accel_format(1, 2);
    accel_calibrate();

	//Variaveis controladas pelos botões
	
    int inputSW, inputKEY;  // Valor retornado pelo KEY_read()
	int sentido; // Sentido do giro do peça: -1 = anti horário, 1 = horário
	bool hold, canHold,duploGiro;
    bool pause = true, reset = true;

	printf("Fim do Setup, Inicio do jogo");

	Resetar(tabuleiro, &pecaFlutuanteExiste, tetrominoPreview);

	while(true)
	{
		while (!gameOver)
		{
			
			//o loop será executado TICKS vezes em um segundo
			Delay(1/TICKS);

			//signal(SIGINT, catchSIGINT);

			video_erase();
			video_clear();

			if (pecaFlutuanteExiste)
			{
				printf("Peça flutuante existe\n");
				
				//Gravidade
				if (cooldownGravidade == COOLDOWN_GRAVIDADE)
				{
					printf("%d", cooldownGravidade);
					printf("Gravidade Ativada\n");
					//mover para baixo
					if (!Mover(tabuleiro, &tetrominoFlutuante, 0))
					{
						printf("Peça Congelada\n");
						canHold = true;
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

					if (acel_x < -INPUT_INCLINACAO)
					{
						Mover(tabuleiro, &tetrominoFlutuante, 1);
						cooldownMovimento = 0;
					} 
					else if (acel_x > INPUT_INCLINACAO) 
					{
						Mover(tabuleiro, &tetrominoFlutuante, 1);
						cooldownMovimento = 0;
					} 

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
				VerificaLinhaCheia(tabuleiro, linhasCheias);
				LimpaLinhas(tabuleiro, linhasCheias);

				// int indexAleatorio = rand() % quantidadeTetrominos;
				// memcpy(&tetrominoFlutuante, &listaTetrominos[indexAleatorio], sizeof(listaTetrominos[indexAleatorio]));
				// tetrominoFlutuanteX = SPAWN_BLOCK_X;
				// tetrominoFlutuanteY = SPAWN_BLOCK_Y;

				GerarTetromino(&tetrominoFlutuante, tetrominoPreview);

				printf("Y: %d\n", tetrominoFlutuante.y);
				printf("X: %d\n", tetrominoFlutuante.x);
				ImprimirTabuleiro(tabuleiro);
				if (TestarColisao(tabuleiro, &tetrominoFlutuante, SPAWN_BLOCK_X, SPAWN_BLOCK_Y))
				{
					// Colisão no surgimento do tetromino
					gameOver = true;
					printf("GameOver\n");
				}
				else
				{
					pecaFlutuanteExiste = true;
					printf("AQUI POHA KRALHO\n");

				}
			}

			printf("AQUI POHA\n");
			ReceberInput(&gameOver, &hold , &duploGiro, &sentido);

			if (duploGiro)
			{
				RotacaoTetromino(tabuleiro, &tetrominoFlutuante, 1);
				RotacaoTetromino(tabuleiro, &tetrominoFlutuante, 1);
				duploGiro = false;
			}
			
			if (hold)
			{
				Hold(&tetrominoFlutuante, &tetrominoHold, &canHold);
			}
		
			RotacaoTetromino(tabuleiro, &tetrominoFlutuante, sentido);

			ImprimirTabuleiro(tabuleiro);
			ImprimirTetromino(&tetrominoFlutuante, tetrominoFlutuante.x, tetrominoFlutuante.y);
			ImprimirTetromino(&tetrominoHold, 12, 0);
			int i;
			for(i = 0; i < TAMANHO_PREVIEW; i++)
			{
				ImprimirTetromino(&tetrominoPreview[i], 12, i + 3);
			}

			video_show();
			printf("Fim\n");
		}

		// video_text(10, 10, "Game Over");
		printf("Game Over, Desative o reset e pressione hold para jogar novamente");
		Resetar(tabuleiro, &pecaFlutuanteExiste, tetrominoPreview);
		while (inputSW != 0 && inputKEY != 8)
		{
			SW_read(&inputSW);
			KEY_read(&inputKEY);
			printf("%d\n", inputSW);
			printf("%d\n", inputKEY);
			Delay(1/10);
		}
		gameOver = false;

	}

    // // signal(SIGINT, catchSIGINT);

	return 0;
}
/*
Função para leitura de entrada dos botões 
Pause funciona com lógica de alternância 
*/
void ReceberInput(bool *gameOver, bool *hold, bool *duploGiro, int *sentido) 
{
	printf("ReceberInput()\n");
	int input;

	KEY_read(&input);

	*sentido = 0;

	if (input == 8) {
		*hold = true;
	}
	else if (input == 4)
	{
		*duploGiro = true;
	}
	else if (input == 2)
	{
		*sentido = -1;
	}
	else if (input == 1)
	{
		*sentido = 1;
	}

	SW_read(&input);

	switch (input)
	{
	case 1 : 
		*gameOver = true;
		break;
	case 2 : 
		Pause();
		break;
	}
	
}

void Hold(Tetromino *tetromino, Tetromino *hold, bool *canHold, Tetromino tetrominoPreview[TAMANHO_PREVIEW]) 
{
	if(*canHold)
	{
		Tetromino temp = *tetromino;
		if (*hold[0][0] == -1) // se hold estiver vazia
		{
			GerarTetromino(tetromino, tetrominoPreview);
		}
		else
		{
			memcpy(tetromino, hold, sizeof(*tetromino));
		}
		
		memcpy(hold, &temp, sizeof(*tetromino));
		*canHold = false;
    }
}

void GerarTetromino(Tetromino *tetromino,  Tetromino tetrominoPreview[TAMANHO_PREVIEW])
{
	int i;
	int indexAleatorio;

	memcpy(tetromino, &tetrominoPreview[0], sizeof(*tetromino));

	for(i = 0; i < TAMANHO_PREVIEW - 1; i ++)
	{
		indexAleatorio = rand() % QUANTIDADE_TETROMINOS;
		memcpy(&tetrominoPreview[i],&tetrominoPreview[i+1], sizeof(*tetromino));
	}

	indexAleatorio = rand() % QUANTIDADE_TETROMINOS;
	memcpy(&tetrominoPreview[TAMANHO_PREVIEW], LISTA_PONTEIROS_TETROMINOS[indexAleatorio], sizeof(*tetromino));
}

void PreencherPreview(Tetromino tetrominoPreview[TAMANHO_PREVIEW]) 
{
	int i;
	int indexAleatorio;
	for(i = 0; i < TAMANHO_PREVIEW; i ++)
	{
		indexAleatorio = rand() % QUANTIDADE_TETROMINOS;
		memcpy(&tetrominoPreview[i], LISTA_PONTEIROS_TETROMINOS[indexAleatorio], sizeof(tetrominoPreview[0]));
	}
}

void Resetar(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], 
			bool *pecaFlutuanteExiste, Tetromino tetrominoPreview[TAMANHO_PREVIEW])
{
	printf("Resetar()");
	IniciarTabuleiro(tabuleiro); //inicia mapa vazio do jogo		
	*pecaFlutuanteExiste = false;
	PreencherPreview(tetrominoPreview);
}

void Pause()
{
	int input = 2;
	while (input != 0)
		{
			Delay(1/10);
			SW_read(&input);
		}
}

/*
Inicia o mapa de colisao, gerando o chão e as paredes do tabuleiro do jogo. 
Ex:
1 0 0 0 1 
1 0 0 0 1 
1 0 0 0 1
1 1 1 1 1
uma tabuleiro composta de 0s e 1s
1 : significa uma parede 
0 : espaço livre
Ver a função TestarColisao() para entender melhor o funcionamento
Talvez seja desnecessário, discutiremos melhor
*/
void IniciarTabuleiro(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO])
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
				tabuleiro[i][j] = 1;
			}
			else 
			{
				tabuleiro[i][j] = 0;
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
int tabuleiroColisao : chamar a função TestarColisao()
Tetromino *tetromino : ponteiro apontando para o tipo de tetromino a ser movido
int atualPosX e int atualPosY : posição atual da ancora do tetromino na tabuleiro do jogo
*/
bool Mover(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int direcao) 
{
	printf("Mover()");

    if(direcao == 0)
    {
        if(TestarColisao(tabuleiro, tetromino, tetromino->x, tetromino->y+1))
        {
			printf("1");
            //Há colisão e o movimento não foi realizado
			//congelar o tetromino
			CongelarTetromino(tabuleiro, tetromino);
            return false;
        }
        else
        {
			printf("2");
            //Não há colisão, movimento realizado
			tetromino->y = tetromino->y + 1;
            return true;
        }
    }
    else
    {
        if(TestarColisao(tabuleiro, tetromino, tetromino->x + direcao, tetromino->y))
        {
			printf("3");
            //Há colisão e o movimento não foi realizado
            return false;
        }
        else
        {
			printf("4");
            //Não há colisão, movimento realizado
			tetromino->x = tetromino->x + direcao;
            return true;
        }
    }
}

/*
Sentido horario : sentido = 1
Sentido anti-horario : sentido = -1
*/
void RotacaoTetromino(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int sentido)
{
	if (sentido != 0)
	{
		printf("RotacaoTetromino(), %d \n", sentido);
		int i;
		int j;

		int tabuleiroTemp[BLOCOS_POR_PECA][BLOCOS_POR_PECA];
		int posXTemp = tetromino->x;

		memcpy(&tabuleiroTemp, tetromino->formato, sizeof(tetromino->formato));

		for(i = 0; i < BLOCOS_POR_PECA; i++)
		{
			for(j = 0; j < BLOCOS_POR_PECA; j++)
			{	
				if (sentido == 1)
				{
					tetromino->formato[BLOCOS_POR_PECA-j-1][i] = tabuleiroTemp[i][j];
				}
				else if (sentido == -1)
				{
					tetromino->formato[j][BLOCOS_POR_PECA-i-1] = tabuleiroTemp[i][j];
				}
				
			}
		}

		i = 0;
		while (TestarColisao(tabuleiro, tetromino, tetromino->x, tetromino->y))
		{
			//da pra melhorar, talvez não seja muito confiavel com tabuleiros de poucas colunas
			if (tetromino->x > COLUNAS_TABULEIRO/2)
			{
				tetromino->x--;
			}
			else
			{
				tetromino->x++;
			}	
			if (i == 2) 
			{
				printf("Não girar");
				tetromino->x = posXTemp;
				memcpy(tetromino->formato, &tabuleiroTemp, sizeof(tetromino->formato));
				break;
			}
			i++;
		}
	}
}

void CongelarTetromino(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino) 
{
	printf("CongelarTetromino()");
    int i;
    int j;

    for(i = 0; i < BLOCOS_POR_PECA; i++)
    {
        for(j = 0; j < BLOCOS_POR_PECA; j++)
        {
			if(tetromino->formato[i][j] == 1)
			{
            	tabuleiro[tetromino->y+i][tetromino->x+j] = tetromino->cor;
			}
        }
    }
}

/*
1000000001
1001111001
1111111111
Baseando se na tabuleiro do formato do tetromino, essa função busca
na tabuleiro de colisão se há alguma parede na mesma posição caso o
tetromino se movesse para as coordenadas novaPosX e novaPosY.
Parametros: 
int tabuleiroColisao[][] : tabuleiro de colisao a ser consultada
Tetromino *tetromino : ponteiro apontando para o tipo de tetromino a ser movido
int novaPosX e int novaPosY : coordenadas da possível nova posição

Obs: a posição do tetromino é determinada pela posição da ancora em relação
a tabuleiro de colisão
*/
bool TestarColisao(int tabuleiroColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], 
Tetromino *tetromino, int x, int y)
{	
	printf("TestarColisao()\n");
	int i;
  	int j;

  	for (i = 0; i < BLOCOS_POR_PECA; i++) 
	{
		printf("%d\n", i);

		for (j = 0; j < BLOCOS_POR_PECA; j++) 
		{
			printf("%d\n", j);

			if(tabuleiroColisao[y+i][x+j] > 0 && tetromino->formato[i][j] == 1) 
			{
				//colisão
				return true;
			}
		}
    }
	//não há colisão
	return false;	
}

void VerificaLinhaCheia(int tabuleiroColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[BLOCOS_POR_PECA])
{
	printf("VerificaLinhaCheia()");
	int i;
	int j;
	bool cheia;

	//limpar lista de linhas a serem limpadas
	for(i = 0; i < BLOCOS_POR_PECA; i++)
	{
		linhasCheias[i] = -1;
	}

	int indexLinhasCheias = 0;

	//buscar na tabuleiro por linhas cheias
	i = 0;
	for(i = 0; i < LINHAS_TABULEIRO-1; i++)
	{
		cheia = true;
		for(j = 1; j < COLUNAS_TABULEIRO-1; j++)
		{
			if(tabuleiroColisao[i][j] == 0)
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
void LimpaLinhas(int tabuleiroColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[BLOCOS_POR_PECA])
{
	printf("LimpaLinhas()");
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
					tabuleiroColisao[i][j] = tabuleiroColisao[i-1][j];
				}
			}
			j = 0;
			//define as bordas laterais do  tabuleiro e a primeira linha
			for (j = 1; j < COLUNAS_TABULEIRO-1; j++)
			{
				tabuleiroColisao[0][j] = 0;
			}
			tabuleiroColisao[0][0] = 1;
			tabuleiroColisao[0][COLUNAS_TABULEIRO] = 1;
		}
	}
}

//imprime a tabuleiro no console
void ImprimirTabuleiro(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]) 
{
	printf("ImprimirTabuleiro()");
	int i;
	int j;

	for (i = 0; i < LINHAS_TABULEIRO; i++) 
	{
		printf("\n");
		for (j = 0; j < COLUNAS_TABULEIRO; j++) 
		{

			if (tabuleiro[i][j] > 0)
			{
				video_box(
					((MARGEM_ESQUERDA_TABULEIRO + j)*QUADRADO_LADO), 
					((MARGEM_TOPO_TABULEIRO + i)*QUADRADO_LADO),

					QUADRADO_LADO+((MARGEM_ESQUERDA_TABULEIRO + j)*QUADRADO_LADO), 
					QUADRADO_LADO+((MARGEM_TOPO_TABULEIRO + i)*QUADRADO_LADO),

					LISTA_CORES[tabuleiro[i][j]]);
			}
			printf("%d", tabuleiro[i][j]);
		}
	}
}

void ImprimirTetromino(Tetromino *tetromino, int x, int y) 
{
	printf("Exibetetromino()\n");
	int i;
	int j;

	for (i = 0; i < BLOCOS_POR_PECA; i++) 
	{
		printf("\n");
		for (j = 0; j < BLOCOS_POR_PECA; j++) 
		{
			if (tetromino->formato[i][j])
			{
				video_box(
					(MARGEM_ESQUERDA_TABULEIRO + x + j)*QUADRADO_LADO, 
					(MARGEM_TOPO_TABULEIRO + y + i)*QUADRADO_LADO,

					QUADRADO_LADO+((MARGEM_ESQUERDA_TABULEIRO + x + j)*QUADRADO_LADO), 
					QUADRADO_LADO+((MARGEM_TOPO_TABULEIRO + y + i)*QUADRADO_LADO), 

					LISTA_CORES[tetromino->cor]);
			}
		}
	}
}

void ImprimirTela(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetrominoFlutuante,
					Tetromino *tetrominoHold, Tetromino tetrominoPreview[TAMANHO_PREVIEW])
{
	ImprimirTabuleiro(tabuleiro);
	ImprimirTetromino(&tetrominoFlutuante, tetrominoFlutuante.x, tetrominoFlutuante.y);
	ImprimirTetromino(&tetrominoHold, MARGEM_ESQUERDA_HOLD, MARGEM_TOPO_HOLD);
	int i;
	for(i = 0; i < TAMANHO_PREVIEW; i++)
	{
		ImprimirTetromino(&tetrominoPreview[i], MARGEM_ESQUERDA_PREVIEW, 
						(i*(BLOCOS_POR_PECA + SEPARACAO_PREVIEW)) + MARGEM_TOPO_PREVIEW);
	}
}

// função para gerar Delay, parametro é dado em segundos
void Delay(int segundos)
{	
	printf("Delay()");
	// converter segundos para microsegundos
    int microSegundos = 1000000 * segundos;

    // tempo inicial
    clock_t start_time = clock();

	// loop até o Delay necessário
    while (clock() < start_time + microSegundos);
}
