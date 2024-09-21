# Tetris-em-uma-De1-SoC

## Introdução:

Este relatório técnico apresenta o desenvolvimento de um jogo no estilo Tetris utilizando a plataforma de desenvolvimento DE1-SoC com arquitetura ARM. E para isso, foi necessário a aplicação prática dos conceitos de programação em C pra fazer o Tetris, criação de biblioteca do acelerômetro usando mapeamento de memória via I2C, controle de dispositivos de entrada (botões e chaves) e saída (interface VGA) disponíveis na placa. Ao decorrer da leitura, são descritas as decisões tomadas, resultados obtidos e lições aprendidas durante o processo.

## Fundamentação Teórica

### G-Sensor ADXL345

Para a comunicação com o acelerômetro, informações obtidas no datasheet do ADXL345 e nas aulas de Arquitetura de Computadores foram de extrema importância. Através dessas fontes, foi descoberto que se faz necessário: 

	• Obter o banco de registradores (Register Map) para facilitar o mapeamento;
	• Abrir a pasta "/dev/mem" e mapeia a memória do I2C;
	• Inicializar o I2C, habilitando o controlador, definindo a taxa de clock e o endereço de destino do acelerômetro (0x53). Os registradores dessas informações correspondem de dentro do bloco I2C;
	• Configurar o acelerômetro para o modo de medição com sensibilidade de ±16g e frequência de 200 Hz;
	• Obter o valor ajustado do eixo X

Mais adiante, será explicado detalhadamente essas etapas.

### Bibliotecas para acesso de periféricos

Na placa, já veio disponiblizado bibliotecas que fazer a comunicação com os periféricos que são usados como os botões e o VGA. Para saber como ultiliza-los, o FPGA Academy foi essencial nessa etapa.

### Linguagem de Programação C

O jogo foi elaborado em lingagem C por ser um requisito do problema.

## Metodologia 

### Descrição de alto nível

#### Acelerômetro

#### Tetris

## Conclusão

## Bibliografia

####

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
