#ifndef GPIOV2R3_H
#define GPIOV2R3_H

#include "geralv2r1.h"															// Configuracao basica de elementos internos.

/* Entrada */
#define ___gpiCK	GPIO_NUM_12                       			                // Seleciona o pino de 'clock' para o registrador.
#define ___gpiDT	GPIO_NUM_13                       			                // Seleciona o pino de 'data in' para o registrador.
#define ___gpiLD	GPIO_NUM_14                       			                // Seleciona o pino de 'load' para o registrador.

/* Saida */
#define ___gpoCK	GPIO_NUM_12                       			                // Seleciona o pino de 'clock' para o registrador.
#define ___gpoDT	GPIO_NUM_27                       			                // Seleciona o pino de 'data out' para o registrador.
#define ___gpoLD	GPIO_NUM_14                       			                // Seleciona o pino de 'load' para o registrador.

/* Expansao */
#define ___expCK	GPIO_NUM_32                       			                // Seleciona o pino de 'clock' para o registrador.
#define ___expLD	GPIO_NUM_33                       			                // Seleciona o pino de 'load' para o registrador.
#define ___expWR	GPIO_NUM_25                       			                // Seleciona o pino de 'data out' para o registrador.
#define ___expRD	GPIO_NUM_26                       			                // Seleciona o pino de 'data in' para o registrador.

unsigned char ___charVlr=0;														// Variavel global para recerregar o valor da saida.
unsigned char ___expVlr1=0;														// Variavel global para recerregar o valor da expansao.

/* Geral */

/**
*	@brief Limpa os pinos do registrador(74HC595).
*/
void __regClear(void)															// Limpa o registrador.
{
	/* Limpa o registrador */
	unsigned char __tmp010;									                    // Var local temporaria.					
	for(__tmp010=0;__tmp010<8;__tmp010++)                                       // Laco para zerar o registrador.
	{
		gpio_set_level(___gpoCK,1);									            // Gera um pulso de clock no registrador.
		gpio_set_level(___gpoCK,0);                                             // 
	}
}

/* Bloco Inicio: GPIOs */

/**
*	@brief Inicializa os pinos do hardware e limpa os pinos do registrador(74HC595) da saida. Obrigatorio antes da funcao de escrita.
*/
void gpoIniciar(void)															// Inicializa o hardware da saida.
{
    gpio_reset_pin(___gpoCK);													// Limpa configuracoes anteriores.
	gpio_reset_pin(___gpoDT);													// Limpa configuracoes anteriores.
	gpio_reset_pin(___gpoLD);													// Limpa configuracoes anteriores.
    gpio_set_direction(___gpoCK, GPIO_MODE_OUTPUT);								// Configura o pino como saida.
    gpio_set_direction(___gpoDT, GPIO_MODE_OUTPUT);								// Configura o pino como saida.
    gpio_set_direction(___gpoLD, GPIO_MODE_OUTPUT);								// Configura o pino como saida.
	gpio_set_level(___gpoCK,0);													// Limpa o pino.
	gpio_set_level(___gpoDT,0);													// Limpa o pino.
	gpio_set_level(___gpoLD,0);													// Limpa o pino.
	__regClear();																// Limpa conteudo do registrador.
}

/**
*	@brief Inicializa os pinos do hardware e limpa os pinos do registrador(74HC165) da entrada. Obrigatorio antes da funcao de leitura.
*/
void gpiIniciar(void)															// Inicializa o hardware da entrada.
{
    gpio_reset_pin(___gpiCK);													// Limpa configuracoes anteriores.
	gpio_reset_pin(___gpiDT);													// Limpa configuracoes anteriores.
	gpio_reset_pin(___gpiLD);													// Limpa configuracoes anteriores.
    gpio_set_direction(___gpiCK, GPIO_MODE_OUTPUT);								// Configura o pino como saida.
    gpio_set_direction(___gpiDT, GPIO_MODE_INPUT);								// Configura o pino como entrada.
    gpio_set_direction(___gpiLD, GPIO_MODE_OUTPUT);								// Configura o pino como saida.
	gpio_set_level(___gpiCK,0);													// Limpa o pino.
	gpio_set_level(___gpiDT,0);													// Limpa o pino.
	gpio_set_level(___gpiLD,0);													// Limpa o pino.
	__regClear();																// Limpa conteudo do registrador.
}

/**
*	@brief Envia valor em binario para saida (U8 e U12, U16 e U19, U11 e U14).
*	@param vlrSaida Dado para enviar.
*/
void gpoDado(char vlrSaida)														// Envia um dado para o GPO (saida).
{
	unsigned char __tmp001;														// Var local temporaria.
	___charVlr=vlrSaida;														// Salva uma copia do valor da saida.  
	for(__tmp001=0;__tmp001<8;__tmp001++)										// Laco para serializar.
	{
		if(bitX(vlrSaida,(7-__tmp001)))gpio_set_level(___gpoDT,1);				// Verifica o valor do bit, se 1...
		else gpio_set_level(___gpoDT,0);										// ... e se for 0.				
		gpio_set_level(___gpoCK,1);									            // Gera um pulso de clock no registrador.
		gpio_set_level(___gpoCK,0);                                             // ...
	}							
	gpio_set_level(___gpoCK,0);													// Limpa o pino.
	gpio_set_level(___gpoDT,0);													// Limpa o pino.
	gpio_set_level(___gpoLD,1); 							                    // Gera um pulso para carregar o dado.
	gpio_set_level(___gpoLD,0);                                                 // ...
}

/**
*	@brief Faz a leitura do valor em binario da entrada (De E0 ate E7).
*	@param Retorna valor entre 0 e 255 (0x00 e 0xFF).
*/
unsigned char gpiDado(void)														// Le um dado da GPI (entrada).
{
	unsigned char entrada=0x00;				                                    // Var local temporaria para a entrada.
	unsigned char tmp002=0x00;				                                    // Var local temporaria para o laco.
	gpio_set_level(___gpiLD,0);													// Limpa o pino.
	gpio_set_level(___gpiLD,1);					                                // Ativa o pino da carga do dado.
	for(tmp002=0;tmp002<8;tmp002++)							                    // Laco para varrer os bits.
	{
		if(gpio_get_level(___gpiDT)==1) bit1(entrada,(7-tmp002));		        // Se o pino de entrada estiver ativado...
		else			                bit0(entrada,(7-tmp002));		        // ... senao...
        gpio_set_level(___gpiCK,1);					                            // Gera um pulso de clock no registrador.
        gpio_set_level(___gpiCK,0);					                            // ...
	}
	gpio_set_level(___gpiCK,0);													// Limpa o pino.
	gpio_set_level(___gpiDT,0);													// Limpa o pino.
	gpoDado(___charVlr);														// Recarrega o valor da saida.
	return (entrada);								                            // Retorna com o valor
}
/* Bloco Fim: GPIOs */

/* Bloco Inicio: Expansao */

/**
*	@brief Inicializa os pinos do hardware e limpa os pinos do registrador(74HC595) da expansao. Obrigatorio antes da funcao de escrita.
*/
void expIniciar(void)															// Inicializa o hardware da expansao.
{
	gpio_reset_pin(___expCK);													// Limpa configuracoes anteriores.
	gpio_reset_pin(___expWR);													// Limpa configuracoes anteriores.
	gpio_reset_pin(___expRD);													// Limpa configuracoes anteriores.
	gpio_reset_pin(___expLD);													// Limpa configuracoes anteriores.
    gpio_set_direction(___expCK, GPIO_MODE_OUTPUT);								// Configura o pino como saida.
	gpio_set_direction(___expWR, GPIO_MODE_OUTPUT);								// Configura o pino como saida.
    gpio_set_direction(___expRD, GPIO_MODE_INPUT);								// Configura o pino como entrada.
    gpio_set_direction(___expLD, GPIO_MODE_OUTPUT);								// Configura o pino como saida.
	gpio_set_level(___expCK,0);													// Limpa o pino.
	gpio_set_level(___expWR,0);													// Limpa o pino.
	gpio_set_level(___expRD,0);													// Limpa o pino.
	gpio_set_level(___expLD,0);													// Limpa o pino.
	__regClear();																// Limpa conteudo do registrador.
}

/**
*	@brief Envia valor em binario para expansao.
*	@param vlrOut Dado para enviar.
*/
void expGPO(char vlrOut)														// Envia um dado para saida da expansao.
{
	unsigned char __tmp003;														// Var local temporaria.
	___expVlr1=vlrOut;															// Salva uma copia do valor da expansao. 
	for(__tmp003=0;__tmp003<8;__tmp003++)										// Laco para serializar.
	{
		if(bitX(vlrOut,(7-__tmp003)))gpio_set_level(___expWR,1);				// Verifica o valor do bit, se 1...
		else gpio_set_level(___expWR,0);										// ... e se for 0.				
		gpio_set_level(___expCK,1);									            // Gera um pulso de clock no registrador.
		gpio_set_level(___expCK,0);                                             // ...
	}							
	gpio_set_level(___expLD,1); 							                    // Gera um pulso para carregar o dado.
	gpio_set_level(___expLD,0);                                                 // ...
}

/**
*	@brief Faz a leitura do valor em binario da expansao.
*	@param Retorna valor entre 0 e 255 (0x00 e 0xFF).
*/
unsigned char expGPI(void)																// Le um dado da entrada da expansao.
{
	unsigned char expIn=0x00;				                                    // Var local temporaria para a entrada.
	unsigned char tmp004=0x00;				                                    // Var local temporaria para o laco.

	gpio_set_level(___expLD,1);					                                // Ativa o pino da carga do dado.
	for(tmp004=0;tmp004<8;tmp004++)							                    // Laco para varrer os bits.
	{
		if(gpio_get_level(___expRD)==1) bit1(expIn,(7-tmp004));		        	// Se o pino de entrada estiver ativado...
		else			                bit0(expIn,(7-tmp004));		        	// ... senao...
        gpio_set_level(___expCK,1);					                            // Gera um pulso de clock no registrador.
        gpio_set_level(___expCK,0);					                            // ...
	}
	gpio_set_level(___expLD,0);						                            // Desativa o pino da carga do dado.
	expGPO(___expVlr1);															// Recarrega o valor da expansao.
	return (expIn);								                            	// Retorna com o valor
}

/* Bloco Fim: Expansao */

#endif