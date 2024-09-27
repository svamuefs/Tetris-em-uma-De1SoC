		// /* Criação de um jogo inspirado em Tetris para ser executado em uma DE1-SoC
//  - Os tetrominos serão movimentados pelo jogador por meio do acelerômetro
//  - Todos os itens visuais serão exibidos por meio da interface VGA
//  - Botões serão utilizados para: reiniciar, pausar e continuar o jogo
//  - O jogo é encerrado de vez pela utilização de ^C
// */

#include <intelfpgaup/KEY.h> 
#include <intelfpgaup/video.h>
#include <intelfpgaup/SW.h>

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "config.c"	
#include <unistd.h>

#include "accel.c"

bool sair = false;

void encerrarJogo()
{
	sair = true;
	raise(SIGTERM);
}

//Funções

//do Sistema

void IniciarTabuleiro(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]);
void Delay(float segundos);
void ImprimirTabuleiro(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]);
void ImprimirTetromino(Tetromino *tetromino, int x, int y); 
void ImprimirTela(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetrominoFlutuante,
					Tetromino *tetrominoHold, Tetromino tetrominoPreview[TAMANHO_PREVIEW], int *score);
void ImprimirGameOver();
void ImprimirPause();
void ImprimirGameTitle(int indexCor);
void Resetar(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], bool *pecaFlutuanteExiste, Tetromino tetrominoPreview[TAMANHO_PREVIEW]);
void Pause();

//do Jogo

void GerarTetromino(Tetromino *tetromino, Tetromino tetrominoPreview[TAMANHO_PREVIEW]);
void PreencherPreview(Tetromino tetrominoPreview[TAMANHO_PREVIEW]);
bool TestarColisao(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int x, int y);	
void CongelarTetromino(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino);
void VerificaLinhaCheia(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[BLOCOS_POR_PECA]);
void LimpaLinhas(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[BLOCOS_POR_PECA], int *score);

//do Jogador

void ReceberInput(bool *gameOver, bool *hold, bool *flip, int *sentido);
bool Mover(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int direcao);
void RotacaoTetromino(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int sentido);
void Hold(Tetromino *tetromino, Tetromino *hold, bool *canHold, Tetromino tetrominoPreview[TAMANHO_PREVIEW]);

int main() {
	//Setup

	//Configurar signal para encerrar jogo ao usuario usar Ctrl + C
	signal(SIGINT, encerrarJogo);
	srand(time(NULL)); //seed de aleatoriedade

	//Mapeamento e acesso do /dev/mem para acessar o acelerometro via I2C
	int fd = open_and_map();

	//Verificação dos periféricos da DE1-SoC 
	if (KEY_open() == 0 || video_open() == 0 || fd == -1 || SW_open() == 0) {
		printf("Erro na inicialização de periféricos.\n");
		return -1;
	}

    I2C0_init();
    accel_init();

	Tetromino tetrominoFlutuante; // Tetromino controlado pelo jogador
	Tetromino tetrominoHold = {{-1}}; // Tetromino guardado  

	Tetromino tetrominoPreview[TAMANHO_PREVIEW]; // Futuros tetrominos flutuantes

	int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]; // Tabuleiro do jogo

	// Variáveis de controle do jogo

		bool pecaFlutuanteExiste = false;  
		bool gameOver = false;

		int linhasCheias[BLOCOS_POR_PECA];
		int cooldownGravidade = 0;
		int cooldownMovimento = 0;

	// Variáveis controladas pelos periféricos
	
    int accel_x, inputSW, inputKEY; // Recebe input dos periféricos
	int sentido; // Sentido do giro do peça: -1 = anti horário, 1 = horário
	bool hold = false; // True quando usuario pressionar hold

	//True se o usuario ainda não tiver usado hold no tetromino flutuante atual
	bool canHold = true; 
	bool flip = false; // True quando usuario pressionar o botão de flip
    bool pause = true, reset = true; // Controlados pelos switches

	int score = 0;

	FILE *pFileScore;

	// Resetar/Preparar jogo
	Resetar(tabuleiro, &pecaFlutuanteExiste, tetrominoPreview);

	video_erase();
	video_clear();
	video_show();

	KEY_read(&inputKEY);
	int indexCor = 1;

	while (inputKEY == 0)
	{
		KEY_read(&inputKEY);
		Delay(0.3);

		video_clear();
		ImprimirGameTitle(indexCor);
		video_text(29,37,"Press any key to start!");
		if (indexCor < 9) 
		{
			indexCor ++;
		} 	
		else 
		{
			indexCor = 1;
		}
		video_show();
	}

	Resetar(tabuleiro, &pecaFlutuanteExiste, tetrominoPreview);
	score = 0;
	video_erase();
	gameOver = false;
	
	// Loop Principal
	while(!sair)
	{
		//Loop do jogo
		while (!gameOver && !sair)
		{
			//o loop será executado TICKS vezes em um segundo
			Delay(1/TICKS);

			video_clear();

			if (pecaFlutuanteExiste)
			{	
				//Gravidade ativada
				if (cooldownGravidade == COOLDOWN_GRAVIDADE)
				{
					/* Se não for possível mover para baixo a peça é automaticamente
					congelada*/
					if (!Mover(tabuleiro, &tetrominoFlutuante, 0))
					{
						canHold = true; // O jogador poderar usar o hold novamente
						hold = false; // Evita que a peça seja imediatamente trocada no resurgimento
						pecaFlutuanteExiste = false;
					}
					cooldownGravidade = 0;
				}
				else 
				{
					cooldownGravidade++;
				}
				//Movimento
				if (cooldownMovimento == COOLDOWN_INPUT)
				{
					accel_x = get_calibrated_accel_x(); // Receber inclinação da plana

					if (accel_x < -INPUT_INCLINACAO)
					{
						// Movimento para a esquerda
						Mover(tabuleiro, &tetrominoFlutuante, -1);
						cooldownMovimento = 0;
					} 
					else if (accel_x > INPUT_INCLINACAO) 
					{
						// Movimento para a direita
						Mover(tabuleiro, &tetrominoFlutuante, 1);
						cooldownMovimento = 0;
					} 
				}
				else 
				{
					cooldownMovimento++;
				}
			}
			// Se a peça flutuante não existir(foi congelada no frame anterior)
			else 
			{
				// Verificar se há linhas cheias e limpar-las
				VerificaLinhaCheia(tabuleiro, linhasCheias);
				LimpaLinhas(tabuleiro, linhasCheias, &score);

				// Gerar novo tetromino flutuante
				GerarTetromino(&tetrominoFlutuante, tetrominoPreview);

				// Verifica se há colisão no resurgimento do tetromino
				if (TestarColisao(tabuleiro, &tetrominoFlutuante, SPAWN_BLOCK_X, SPAWN_BLOCK_Y))
				{
					// caso sim, o jogo encerra
					gameOver = true;
				}
				else
				{
					pecaFlutuanteExiste = true;
				}
			}

			/* Modifica as variáveis passadas como parametros baseado no estado dos 
			periféricos*/
			ReceberInput(&gameOver, &hold , &flip, &sentido);

			// Rotação em 180 graus
			if (flip)
			{
				RotacaoTetromino(tabuleiro, &tetrominoFlutuante, 1);
				RotacaoTetromino(tabuleiro, &tetrominoFlutuante, 1);
				flip = false;
			}

			// Guardar peça
			if (hold && canHold && pecaFlutuanteExiste)
			{
				Hold(&tetrominoFlutuante, &tetrominoHold, &canHold, tetrominoPreview);
				hold = false;
			}
		
			// Gira a peça dependendo do valor de " sentido "
			RotacaoTetromino(tabuleiro, &tetrominoFlutuante, sentido);

			ImprimirTela(tabuleiro, &tetrominoFlutuante, &tetrominoHold, tetrominoPreview, &score);

			video_show();
		}

		// Gameover
		ImprimirGameOver();
		
		KEY_read(&inputKEY);
		SW_read(&inputSW);

		/* O reset deve está desligado e o jogador deve pressionar hold
		para reiniciar o jogo*/
		while (inputSW != 0 || inputKEY == 0)
		{
			SW_read(&inputSW);
			KEY_read(&inputKEY);
			Delay(1/10);
		}

		Resetar(tabuleiro, &pecaFlutuanteExiste, tetrominoPreview);	
		video_erase();

		pFileScore = fopen("scores.txt", "a");

		char textoScore[15];
   	 	sprintf(textoScore, "\n%d", score);

		fprintf(pFileScore, textoScore);

		fclose(pFileScore);	
        score = 0;

		gameOver = false;

	}

	close_and_unmap(fd);
	video_close();
	KEY_close();
	SW_close();
	return 0;
}

void ReceberInput(bool *gameOver, bool *hold, bool *flip, int *sentido) 
{
	int input;

	KEY_read(&input);

	*sentido = 0;

	if (input == 4) {
		*hold = true;
	}
	else if (input == 8)
	{
		*flip = true;
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
	case 4 : 
		sair = true;
		break;
	}
}

/* O jogador pode escolher guardar a peça flutuante atual, o jogador só poderá usar
esse mecânica novamente após posicionar a peça atual*/
void Hold(Tetromino *tetromino, Tetromino *hold, bool *canHold, Tetromino tetrominoPreview[TAMANHO_PREVIEW]) 
{
	if(*canHold)
	{
		//Variável auxiliar
		Tetromino temp = *tetromino;

		// verifica se o hold está vazio
		if (hold->formato[0][0] == -1) 
		{
			// hold vazio: gera novo tetromino
			GerarTetromino(tetromino, tetrominoPreview);
		}
		else
		{
			// coloca o tetromino no hold como tetromino flutuante
			memcpy(tetromino, hold, sizeof(*tetromino));
		}

		// coloca o tetromino flutuante no hold
		memcpy(hold, &temp, sizeof(*tetromino));

		//reseta coordenadas do tetromino no hold
		hold->x = SPAWN_BLOCK_X;
		hold->y = SPAWN_BLOCK_Y;

		*canHold = false;
    }
}

/*Gerar novo tetromino flutuante*/
void GerarTetromino(Tetromino *tetromino,  Tetromino tetrominoPreview[TAMANHO_PREVIEW])
{
	int i;
	int indexAleatorio;

	// Tetromino flutuante = tetromino na primeira posição do preview
	memcpy(tetromino, &(tetrominoPreview[0]), sizeof(*tetromino));

	// mudar posições dos tetrominos no preview em uma posição
	for(i = 0; i < TAMANHO_PREVIEW - 1; i ++)
	{
		memcpy(&(tetrominoPreview[i]),&(tetrominoPreview[i+1]), sizeof(*tetromino));
	}

	// gerar tetromino aleatório na ultima posição do preview
	indexAleatorio = rand() % QUANTIDADE_TETROMINOS;
	memcpy(&(tetrominoPreview[TAMANHO_PREVIEW-1]), LISTA_PONTEIROS_TETROMINOS[indexAleatorio], sizeof(*tetromino));
}

// preencher preview com tetrominos aleatórios
void PreencherPreview(Tetromino tetrominoPreview[TAMANHO_PREVIEW]) 
{
	int i;
	int indexAleatorio;
	for(i = 0; i < TAMANHO_PREVIEW; i ++)
	{
		indexAleatorio = rand() % QUANTIDADE_TETROMINOS;
		memcpy(&(tetrominoPreview[i]), LISTA_PONTEIROS_TETROMINOS[indexAleatorio], sizeof(tetrominoPreview[0]));
	}
}

// Resetar jogo
void Resetar(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], 
			bool *pecaFlutuanteExiste, Tetromino tetrominoPreview[TAMANHO_PREVIEW])
{
	IniciarTabuleiro(tabuleiro);	
	*pecaFlutuanteExiste = false;
	PreencherPreview(tetrominoPreview);
}

// pausar jogo
void Pause()
{
	int input = 2;
	int indexCor = 1;

	video_clear();
	video_erase();
	ImprimirPause();
	// Receber input até que o jogo seja despausado
	while (input != 0)
		{
			Delay(0.3);

			//Mostrar tela de pause
			video_clear();
			ImprimirGameTitle(indexCor);
			if (indexCor < 9) 
			{
				indexCor ++;
			} 
			else 
			{
				indexCor = 1;
			}
			video_show();

			SW_read(&input);
		}

	// video_clear();
	video_erase();
}

/*
Inicia o tabuleiro, gerando o chão e as paredes do tabuleiro do jogo. 
Ex:
1 0 0 0 1 
1 0 0 0 1 
1 0 0 0 1
1 1 1 1 1
uma tabuleiro composto de 0s e 1s
1 : significa uma parede    
0 : espaço livre
*/
void IniciarTabuleiro(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO])
{
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
Função para mover a peça para a esquerda, direita ou para baixo
Parametros: 
int direcao : -1 = esquerda , 0 = baixo ou 1 = direita
int tabuleiro : tabuleiro do jogo
Tetromino *tetromino : tetromino flutuante
*/
bool Mover(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino, int direcao) 
{
	// Para baixo
    if(direcao == 0)
    {
        if(TestarColisao(tabuleiro, tetromino, tetromino->x, tetromino->y+1))
        {
            //Há colisão e o movimento não foi realizado
			//Congelar o tetromino
			CongelarTetromino(tabuleiro, tetromino);
            return false;
        }
        else
        {;
            //Não há colisão, movimento realizado
			tetromino->y = tetromino->y + 1;
            return true;
        }
    }
	// Para os lados
    else
    {
        if(TestarColisao(tabuleiro, tetromino, tetromino->x + direcao, tetromino->y))
        {
            //Há colisão e o movimento não foi realizado
            return false;
        }
        else
        {
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
		int i;
		int j;

		// Variáveis auxiliares
		int matrizTemp[BLOCOS_POR_PECA][BLOCOS_POR_PECA];
		int posXTemp = tetromino->x;

		// Guardar formato atual do tetromino
		memcpy(&matrizTemp, tetromino->formato, sizeof(tetromino->formato));

		// Rotacionar tetromino
		for(i = 0; i < BLOCOS_POR_PECA; i++)
		{
			for(j = 0; j < BLOCOS_POR_PECA; j++)
			{	
				if (sentido == 1)
				{
					// rotacionar matriz no sentido horário
					tetromino->formato[j][BLOCOS_POR_PECA-i-1]  = matrizTemp[i][j];
				}
				else if (sentido == -1)
				{
					// rotacionar matriz no sentido anti horário
					tetromino->formato[BLOCOS_POR_PECA-j-1][i] = matrizTemp[i][j];
				}
				
			}
		}

		// Testar colisão com matriz rotacionada
		i = 0;
		while (TestarColisao(tabuleiro, tetromino, tetromino->x, tetromino->y))
		{
			//Há colisão

			if (tetromino->x > COLUNAS_TABULEIRO/2)
			{
				/* Mover tetromino para esquerda se ele 
				estiver no lado direito do tabuleiro*/
				tetromino->x--;
			}
			else
			{
				// Mover para o outro lado
				tetromino->x++;
			}	
			// Tentar mover o tetromino apenas duas vezes
			if (i == 2) 
			{
				//Rotação falhou

				// Desfazer todas as alterações feitas no tetromino
				tetromino->x = posXTemp;
				memcpy(tetromino->formato, &matrizTemp, sizeof(tetromino->formato));
				break;
			}
			i++;
		}
	}
}

/*O tetromino flutuante só é escrito na matriz do tabuleiro
quando é congelado*/
void CongelarTetromino(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetromino) 
{
    int i;
    int j;

    for(i = 0; i < BLOCOS_POR_PECA; i++)
    {
        for(j = 0; j < BLOCOS_POR_PECA; j++)
        {
			if(tetromino->formato[i][j] == 1)
			{
				/* De acordo com o formato do tetromino e sua posição relativa ao
				tabuleiro o tetromino é escrito no tabuleiro com o inteiro que representa
				sua cor*/
            	tabuleiro[tetromino->y+i][tetromino->x+j] = tetromino->cor;
			}
        }
    }
}

/*
Baseando se na matriz do formato do tetromino e sua posição, essa função busca
no tabuleiro se há alguma parede na mesma posição, caso o
tetromino se movesse para as coordenadas x e y.
Parametros: 
int tabuleiroColisao[][] : tabuleiro de colisao a ser consultado
Tetromino *tetromino : ponteiro apontando para o tetromino a ser movido
int x e int y : coordenadas da possível nova posição
*/
bool TestarColisao(int tabuleiroColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], 
Tetromino *tetromino, int x, int y)
{	
	int i;
  	int j;

  	for (i = 0; i < BLOCOS_POR_PECA; i++) 
	{
		for (j = 0; j < BLOCOS_POR_PECA; j++) 
		{
			// valores maiores que 0 no tabuleiro significam colisão
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

/*Verifica se há uma linha horizontal completa, sem qualquer descontinuidade.
Parâmetros: 
	- tabuleiroColisão: tabuleiro do jogo
	- linhasCheias: lista para que a função retorne os
	indices das linhas completas no tabuleiro*/
void VerificaLinhaCheia(int tabuleiroColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhasCheias[BLOCOS_POR_PECA])
{
	int i;
	int j;
	bool cheia;

	//limpar lista de linhas a serem limpadas
	for(i = 0; i < BLOCOS_POR_PECA; i++)
	{
		linhasCheias[i] = -1;
	}

	int indexLinhasCheias = 0;

	//buscar no tabuleiro por linhas cheias
	i = 0;
	for(i = 0; i < LINHAS_TABULEIRO-1; i++)
	{
		// valor padrão
		cheia = true;

		for(j = 1; j < COLUNAS_TABULEIRO-1; j++)
		{
			if(tabuleiroColisao[i][j] == 0)
			{
				// caso encontre um buraco
				cheia = false;
				break;
			}
		}
		if(cheia)
		{
			linhasCheias[indexLinhasCheias] = i;
			indexLinhasCheias ++;
		}
		
		// máximo de linhas cheias possíveis
		if (indexLinhasCheias == BLOCOS_POR_PECA)
		{
			break;
		}
	}
}

//limpa a linha do tabuleiro ao completar
void LimpaLinhas(int tabuleiroColisao[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], int linhas[BLOCOS_POR_PECA], int *score)
{
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
			*score += 100*(k+1);
			video_erase();
		}
	}
}

/*Mostra o tabuleiro na tela. Utilizando a biblioteca de video*/
void ImprimirTabuleiro(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO]) 
{
	int i;
	int j;

	for (i = 0; i < LINHAS_TABULEIRO; i++) 
	{
		for (j = 0; j < COLUNAS_TABULEIRO; j++) 
		{

			if (tabuleiro[i][j] > 0)
			{
				video_box(
					((MARGEM_ESQUERDA_TABULEIRO + j)*QUADRADO_LADO), 
					((MARGEM_TOPO_TABULEIRO + i)*QUADRADO_LADO),

					QUADRADO_LADO+((MARGEM_ESQUERDA_TABULEIRO + j)*QUADRADO_LADO) - 1, 
					QUADRADO_LADO+((MARGEM_TOPO_TABULEIRO + i)*QUADRADO_LADO) - 1,

					LISTA_CORES[tabuleiro[i][j]]);
			}
		}
	}
}

/*Imprime um tetromino na tela nas coordenadas x e y*/
void ImprimirTetromino(Tetromino *tetromino, int x, int y) 
{
	int i;
	int j;

	for (i = 0; i < BLOCOS_POR_PECA; i++) 
	{
		for (j = 0; j < BLOCOS_POR_PECA; j++) 
		{
			if (tetromino->formato[i][j])
			{
				video_box(
					((x + j)*QUADRADO_LADO), 
					((y + i)*QUADRADO_LADO),

					QUADRADO_LADO+(((x + j)*QUADRADO_LADO)-1) - 1, 
					QUADRADO_LADO+(((y + i)*QUADRADO_LADO)-1) - 1, 

					LISTA_CORES[tetromino->cor]);
			}
		}
	}
}

void ImprimirGameOver()
{
	int i;
	for(i = 0; i < 19; i++)
	{
		video_text(MARGEM_ESQUERDA_GAMEOVER, MARGEM_TOPO_GAMEOVER + i , GAMEOVER_GRAPHIC[i]);
	}

	video_show();
}

void ImprimirPause()
{
	int i;
	for(i = 0; i < 13; i++)
	{
		video_text(MARGEM_ESQUERDA_PAUSE, MARGEM_TOPO_PAUSE + i , PAUSE_GRAPHIC[i]);
	}

	video_show();
}

void ImprimirGameTitle(int indexCor)
{
	int i;
	int j;
	for(i = 0; i < 5; i++) 
	{
		for(j = 0; j < 35; j++) 
		{
			if(GAME_TITLE_MATRIX[i][j] == 1)
			{
				video_box((MARGEM_ESQUERDA_TITLE + j)*9, (MARGEM_TOPO_TITLE + i)*9, (MARGEM_ESQUERDA_TITLE + j)* 9 + 9 , (MARGEM_TOPO_TITLE + i) * 9 + 9, LISTA_CORES[indexCor]);
			}
		}
	}
}

/*Imprimir tela de jogo completa*/
void ImprimirTela(int tabuleiro[LINHAS_TABULEIRO][COLUNAS_TABULEIRO], Tetromino *tetrominoFlutuante,
					Tetromino *tetrominoHold, Tetromino tetrominoPreview[TAMANHO_PREVIEW], int *score)
{
	ImprimirTabuleiro(tabuleiro);
	ImprimirTetromino(tetrominoFlutuante, MARGEM_ESQUERDA_TABULEIRO +  tetrominoFlutuante->x, MARGEM_TOPO_TABULEIRO + tetrominoFlutuante->y);
	ImprimirTetromino(tetrominoHold, MARGEM_ESQUERDA_HOLD, MARGEM_TOPO_HOLD);

	int i;
	for(i = 0; i < TAMANHO_PREVIEW; i++)
	{
		ImprimirTetromino(&(tetrominoPreview[i]), MARGEM_ESQUERDA_PREVIEW, 
						(i*(BLOCOS_POR_PECA + SEPARACAO_PREVIEW)) + MARGEM_TOPO_PREVIEW);
	}
	
    char textoScore[15];
    sprintf(textoScore, "Score: %d", *score);
	video_text(MARGEM_ESQUERDA_SCORE, MARGEM_TOPO_SCORE, textoScore);
}

// função para gerar Delay, parametro é dado em segundos
void Delay(float segundos)
{	
	// converter segundos para microsegundos
    int microSegundos = 1000000 * segundos;

    // tempo inicial
    clock_t start_time = clock();

	// loop até o Delay necessário
    while (clock() < start_time + microSegundos);
}
