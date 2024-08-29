# Tetris-em-uma-De1-SoC

# Planejamento
## Jogo
### Tabuleiro
- Matriz
	- Área do jogo
	- Paredes
- Gravidade
	- Atua de n em n tempos
	- faz as peças flutuantes cairem uma unidade
### Peças
- Struct
	- Matriz para formato
	- Cor
	- Coordenadas da ancora
		- determina o ponto por onde o jogo gera a peça
- Coordenadas da ancora da peça flutuante, para manipulação posterior
### Movimento
- configurar acelerometro
### Colisão
- Colisão com paredes e outras peças impedem a realização do movimento
#### Colisão Vertical
- Quando a colisão for vertical, congelar a peça
### Pontuação
- 1 linha = 1 ponto
## Biblioteca Acelerometro

# main.c
## Sequência de funções
- se: sem peça caindo
	- limpar linhas completas
	- gerar proxima peça
		- se: colisão
			- fim de jogo
		- se não: 
			- gerar peça
			- resetar tempo da gravidade
- se não:
	- verificar se a gravidade atua ou não, se não, incrementar tempo +1
		- Caso atue:
			- se: colisão vertical
				- congelar peça
	- receber inputs
		- validar inputs
			- se: colisão
				- não realizar input
			- se não: 
				- Realizar o movimento

Trello: https://trello.com/b/MT18QH97/sd
