# Tetris - Sistemas Digitais (TP01)

<p align="center">
  <img src="imagens/top45_01.jpg" width = "600" />
</p>
<p align="center"><strong>Kit de desenvolvimento DE1-SoC</strong></p>


<h2>  Componentes da Equipe: <br></h2>
<uL> 
  <li><a href="https://github.com/Silva-Alisson">Alisson Silva</a></li>
  <li><a href="https://github.com/DaviOSC">Davi Oliveira</a></li>
  <li><a href="https://github.com/MrLaelapz">Kauã Quintella</a></li>
  <li><a href="https://github.com/Viktor-401">Sinval Victor</a></li>
</ul>

<div align="justify"> 

## Introdução

Este relatório técnico apresenta o desenvolvimento de um jogo no estilo Tetris utilizando a plataforma de desenvolvimento DE1-SoC com arquitetura ARMv8. E para isso, foi necessário a aplicação prática dos conceitos de programação em C pra fazer o Tetris, criação de biblioteca do acelerômetro usando mapeamento de memória via I2C, controle de dispositivos de entrada (botões e chaves) e saída (interface VGA) disponíveis na placa. Ao decorrer da leitura, são descritas as decisões tomadas, resultados obtidos e lições aprendidas durante o processo.

## Fundamentação Teórica

### Kit de desenvolvimento DE1-SoC

A placa que foi usada para executar o jogo possue uma arquitetura baseada na *Altera System-on-Chip* (SoC) FPGA, que combina um Cortex-A9 dual core com cores embarcados com lógica programável. Nela vem integrado o *HPS* (*Hard Processor System*) baseado em ARM, consistindo no processador, periféricos como o acelerômetro (ADLX456) ultilizado e a interface de memória. O sistema do Hard Processor vem ligado perfeitamente à estrutura da FPGA usando um backbone interconectado de alta-bandalarga. (DE1-SoC Manual, 2019)

Ademais, o DE1-SoC possui as seguintes especificações gerais, de acordo com o FPGA Academy:

- Main Features:
	- Intel® Cyclone V SoC FPGA
		- 85k logic-element FPGA
		- ARM Cortex A9 MPCORE
	- 1 GB DDR, 64 MB SDRAM
	- Micro SD Card


- Basic IO:
	- 10 slide switches, 4 pushbuttons
	- 10 LEDs, six 7-segment displays

- IO Devices:
	- Audio in/out
	- VGA Video out, S-Video in
	- Ethernet, USB, Accelerometer
	- A/D converter, PS/2

<p align="center">
  <img src="imagens/kitDesenvolvimentoTopView.png" width = "800" />
</p>
<p align="center"><strong>Layout e componentes do DE1-SoC</strong></p>


### G-Sensor ADXL345

Esse sensor é um acelerômetro de 3 eixos, que realiza medições de alta resolução. A saída digitalizada é formatada em 16 bits com complemento de dois e pode ser acessada via interface I2C e seu endereço é 0x53.

Para a comunicação com o acelerômetro, informações obtidas no datasheet do ADXL345 e nas aulas de Arquitetura de Computadores foram de extrema importância. 
	
Mais adiante, será explicado o passo a passo para a comunicação.

### Protocolo I2C

O I2C (*Inter-Integrated Circuit*), é um protocolo de comunicação serial síncrono, bastante utilizado na interação entre dispositivos periféricos e processadores ou microcontroladores. A comunicação ocorre utilizando dois fios: o SDA, que transporta os dados, e o SCL, responsável pelo sinal de clock que sincroniza a troca de informações. Quando aplicamos isso no G-Sensor, ele opera como um dispositivo *slave* dentro do barramento, enquanto o processador atua como *master*, controlando toda a comunicação e o envio de comandos.

Cada dispositivo conectado ao barramento I2C possui um endereço de 7 bits, o que facilita a identificação. A interação acontece quando o processador, na função de master, envia o endereço do acelerômetro e, a partir daí, realiza operações de leitura ou escrita nos registradores internos, permitindo, por exemplo, a configuração de parâmetros ou a coleta de dados do sensor.

### Bibliotecas para acesso de periféricos

Na placa, já veio disponiblizado bibliotecas que fazer a comunicação com os periféricos que são usados como os botões e o VGA. Para saber como ultiliza-los, o FPGA Academy foi essencial nessa etapa.

### Linguagem de Programação C

O jogo foi elaborado em lingagem C por ser um requisito do problema, sendo usado o GCC para a compilação. A IDE ultilizada foi o Visual Studio Code.

## Metodologia 

Inicialmente, foi feito o levantamento de requisitos do problema, onde foi obtida uma divisão de processos para abstração do projeto. A primeira parte é o <a href="#acelerômetro"> acelerômetro</a>, caracterizada pela comunicação do programa em C com o próprio via I2C. Já a segunda parte é o jogo <a href="#tetris"> Tetris</a>, onde engloba todas as nuâncias do jogo.  E partindo dessa abstração, a equipe se dividiu para encontrar soluções.

A seguir, é demonstrado a descrição em alto nível de cada etapa citada.

### Descrição de alto nível

#### Acelerômetro

###### Etapas para comunicação com ADXL456

Através das fontes, foi descoberto que se faz necessário seguir um passo a passo para se conectar ao G-Sensor:

<div id="sumarioEtapasAcelerometro">
<ul>
	<li>Obter o banco de registradores (Register Map) para facilitar o mapeamento;</li>
	<li>Abrir a pasta "/dev/mem" e mapeia a memória do I2C;</li>
	<li>Inicializar o I2C, habilitando o controlador, definindo a taxa de clock e o endereço de destino do acelerômetro (0x53). Os registradores dessas informações correspondem de dentro do bloco I2C;</li>
	<li>Configurar o acelerômetro para o modo de medição com sensibilidade de ±16g e frequência de 200 Hz;</li>
	<li>Obter o valor ajustado do eixo X</li>
</ul>	
</div>

###### Banco de registradores

O primeiro passo foi **obter o banco de registradores** (Register Map) do ADXL456, conforme descrito no código <a href="https://github.com/svamuefs/Tetris-em-uma-De1SoC/blob/main/address.c">address.c</a>. Esse arquivo contém a definição dos principais registradores utilizados para configurar e ler dados do acelerômetro.

Além disso, o código também define os registradores do controlador I2C, como **I2C0_CON**, **I2C0_TAR**, **I2C0_ENABLE**, entre outros, que são utilizados para realizar a comunicação entre o processador e o acelerômetro via protocolo I2C.

###### Mapeamento da memória do I2C

Referente ao mapeamento, o acesso aos registradores do controlador I2C do processador é feito diretamente através do mapeamento de memória física. Isso é realizado utilizando o */dev/mem*, um arquivo especial no Linux que expõe áreas de memória física do sistema para leitura e escrita.

A função *mmap* mapeia essa área de memória para o espaço de endereços do processo do usuário, permitindo o acesso direto aos registradores de controle de hardware. O procedimento foi realizado com a função *open_and_map()* com o retorno de um inteiro, representando o descritor do arquivo.

###### Inicialização do protocolo

Já o processo de inicialização do I2C, envolve a configuração de valores específicos nos registradores do controlador no processador. Isso inclui habilitar o controlador I2C, definir o clock (configurando os registradores **I2C0_FS_SCL_HCNT** e **I2C0_FS_SCL_LCNT**), além de atribuir o endereço do acelerômetro, que é **0x53**.

Os principais registradores utilizados incluem:

- **I2C0_ENABLE**: Habilita o controlador I2C.
- **I2C0_TAR:** Define o endereço do dispositivo slave.
- **I2C0_CON:** Configura o modo de operação (master/slave) e outras características do controlador.

Esses registradores são acessados diretamente por meio do ponteiro *i2c0_regs*, que aponta para a memória mapeada do controlador.

###### Configuração do Acelerômetro

Para realizar o ajuste do acelerômetro, faz-se necessário escrever um valor no registrador dele. E isso se da através de uma escrita dos dados que deseja enviar no registrador de comando do I2C (**I2C0_DATA_CMD**). Similarmente, para ler um registrador, o processador envia uma solicitação de leitura via I2C e, em seguida, aguarda até que o dado solicitado seja recebido no registrador **I2C0_RXFLR**.

Os principais registradores do acelerômetro utilizados neste processo incluem:

- **DATA_FORMAT (0x31)**: Configura o formato de dados do acelerômetro (sensibilidade ±16g no caso).
- **BW_RATE (0x2C)**: Define a taxa de amostragem (200 Hz no caso).
- **DATA_X0 (0x32)** e DATA_X1 (0x33): Contêm os dados do eixo X em dois bytes (pouca e muita ordem).

###### Calibração e leitura ajustada do Eixo X

Antes das leituras definitivas do acelerômetro, faz-se necessário uma calibração do eixo X para ajustar as futuras leituras. Essa calibração é realizada armazenando o valor do eixo X quando a placa estava posicionada em 180 graus, e utilizando esse valor como offset nas leituras subsequentes.

Para leitura do valor do eixo X, os registradores DATA_X0 e DATA_X1 são combinados em um valor de 16 bits. Esse processo desloca o byte mais significativo para a esquerda e soma o byte menos significativo, formando o valor completo do eixo X.

#### Tetris

Para recriar uma versão fiel do jogo, foi feita uma analise no jogo original. As percepções dos mecanismos usados foram traduzidas para uma aplicação prática no projeto. No projeto, foi aplicado os conceitos do Tetris da seguinte forma. 
	
1. Tabuleiro:
	- Feito por uma matriz com a área do jogo com as paredes e chão.


2. Peças:
	- A peça, ou tetrominó como é chamado, é um Struct que armazena uma matriz para formato, cor, cordenadas onde o jogo gera a peça de acordo com movimento e spawn dela.


3. Movimento da peça:
	- A gravidade, a qual atua de _n_ em _n_ tempos, é considerado uma parte importante do sistema de movimentação e faz as peças flutuantes cairem uma unidade.
	- O comando de ir para a esquerda e direita é alterado mediante à comando do jogador identificado pelo G-Sensor.
	- O usuário do game pode mudar o sentido da peça virando para a esquerda ou direita clicando em botões na placa.


4. Colisão:
	- Colisão nas verticais, podendo ser peças e paredes, impede a realização do movimento adiante, mas continua sofrendo ação da gravidade.
	- Se colidir no chão, a peça é congelada, já se tocar no teto, o jogo encerra.


5. Pontuação:
	- Se o usuário completar uma linha, ele faz 100 pontos.


6. Pause e encerrar jogo:
	- Para pausar, basta clicar em um botão da placa.
	- Quando uma peça colide com outra no momento do respawn, o jogo encerra.

Para aplicar essa estrutura, foram criados dois arquivos de código C, um tem o intuito de armazenar as configurações do projeto (config.c) como altura e largura da tela do VGA, construção do tetrôminó entre outras. Já o outro arquivo (main.c) executa o jogo, armazenando as funções implicitas nos conceitos expostos acima.

O primeiro passo do código é conseguir manipular o Acelerômetro para obter dados de movimentação do usuário através da abertura das pasta "/dev/mem" e mapeamento da memória do I2C. Depois, 



<div align="justify">

## Bibliografia

Using the Accelerometer on DE-Series Boards. Disponível em: https://github.com/fpgacademy/Tutorials/releases/download/v21.1/Accelerometer.pdf. Acessado em: 23 de setembro de 2024.

TERASIC. DE1-SoC User-Manual. Disponível em: https://drive.google.com/file/d/1HzYtd1LwzVC8eDobg0ZXE0eLhyUrIYYE/view. Acessado em: 26 de setembro de 2024.

FPGA Academy. Disponível em: https://fpgacademy.org/. Acessado em: 26 de setembro de 2024.

SOUZA, Fábio. Comunicação I2C. Postado em: 03 de janeiro de 2023. Disponível em: https://embarcados.com.br/comunicacao-i2c/. Acessado em: 26 de setembro de 2024.

PATTERSON, David A.; HENNESSY, John L. Computer Organization and Design: The Hardware Software Interface, ARM Edition. 2016. Morgan Kaufmann. ISBN: 978-0-12-801733-3.
