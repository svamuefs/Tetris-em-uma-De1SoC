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

## Introdução:

Este relatório técnico apresenta o desenvolvimento de um jogo no estilo Tetris utilizando a plataforma de desenvolvimento DE1-SoC com arquitetura ARMv8. E para isso, foi necessário a aplicação prática dos conceitos de programação em C pra fazer o Tetris, criação de biblioteca do acelerômetro usando mapeamento de memória via I2C, controle de dispositivos de entrada (botões e chaves) e saída (interface VGA) disponíveis na placa. Ao decorrer da leitura, são descritas as decisões tomadas, resultados obtidos e lições aprendidas durante o processo.

## Fundamentação Teórica

### Kit de desenvolvimento DE1-SoC

A placa que foi usada para executar o jogo possue uma arquitetura baseada na *Altera System-on-Chip* (SoC) FPGA, que combina um Cortex-A9 dual core com cores embarcados com lógica programável. Nela vem integrado o *HPS* (*Hard Processor System*) baseado em ARM, consistindo no processador, periféricos como o acelerômetro (ADLX456) ultilizado e a interface de memória. O sistema do Hard Processor vem ligado perfeitamente à estrutura da FPGA usando um backbone interconectado de alta-bandalarga. (DE1-SoC Manual, 2019)

Ademais, o DE1-SoC possui as seguintes especificações gerais, de acordo com o FPGA Academy:

	• Main Features:

		- Intel® Cyclone V SoC FPGA
			* 85k logic-element FPGA
			* ARM Cortex A9 MPCORE
		- 1 GB DDR, 64 MB SDRAM
		- Micro SD Card

	• Basic IO:

		- 10 slide switches, 4 pushbuttons
		- 10 LEDs, six 7-segment displays

	• IO Devices:

		- Audio in/out
		- VGA Video out, S-Video in
		- Ethernet, USB, Accelerometer
		- A/D converter, PS/2
	
	Fonte: FPGA Academy

<p align="center">
  <img src="imagens/kitDesenvolvimentoTopView.png" width = "800" />
</p>
<p align="center"><strong>Layout e componentes do DE1-SoC</strong></p>


### G-Sensor ADXL345

Esse sensor é um acelerômetro de 3 eixos, que realiza medições de alta resolução. A saída digitalizada é formatada em 16 bits com complemento de dois e pode ser acessada via interface I2C.

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

O jogo foi elaborado em lingagem C por ser um requisito do problema, sendo usado o GCC para a compilação. A IDE ultilizada foi o Visual Studio Code.

## Metodologia 

Inicialmente, foi feito o levantamento de requisitos do problema, onde foi obtida uma divisão de processos para abstração do projeto.

A primeira parte é o jogo <a href="#-tetris"> Tetris</a>, onde engloba todas as nuâncias do jogo e dos requisitos do problema.

Já a segunda parte é o <a href="#-acelerômetro"> acelerômetro</a>, caracterizada pela comunicação do programa em C com o próprio via I2C. E partindo dessa abstração, a equipe se dividiu para encontrar soluções.

Ao seguir a leitura, você encontrará a descrição em alto nível de cada etapa citada.

### Descrição de alto nível

#### • Tetris


##### Sumário

No Tetris

<div id="sumarioTetris">
	<ul>
        <li>Descrição dos Equipamentos e Software Utilizados</li>
	</ul>	
</div>



#### • Acelerômetro


## Conclusão

<div align="justify">

## Bibliografia

####
