bool Mover(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int atualPosX, int atualPosY) {

    if(direcao == 0)
    {
        if(TestarColisao(matriz, tetromino, atualPosX, atualPosY-1))
        {
            //Há colisão e o movimento não foi realizado
            return false;
        }
        else
        {
            //Não há colisão, movimento realizado
            TranslacaoTetromino(matriz, tetromino, atualPosX, atualPosY, atualPosX, atualPosY-1);
            return true;
        }
        
    }
    else
    {
        if(TestarColisao(matriz, tetromino, atualPosX + direcao, atualPosY))
        {
            //Há colisão e o movimento não foi realizado
            return false;
        }
        else
        {
            //Não há colisão, movimento realizado
            TranslacaoTetromino(matriz, tetromino, atualPosX, atualPosY, atualPosX + direcao, atualPos);
            return true;
        }
    }
}

void TranslacaoTetromino(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int atualPosX, int atualPosY, int novaPosX, int novaPosY)  
{
    //Apagar posição anterior
    DesenharTetromino(matrz, tetromino, atualPosX, atualPos, 0);
    //Desenhar nova posição
    DesenharTetromino(matrz, tetromino, novaPosX, novaPos, 1);
}
    

void DesenharTetromino(int matriz[COLUNAS_TABULEIRO][LINHAS_TABULEIRO], Tetromino *tetromino, int PosX, int PosY, int cor) 
{
    int i;
    int j;
    for(i = 0; i < COLUNAS_TETROMINO; i++)
    {
        for(j = 0; j < LINHAS_TETROMINO; j++)
        {
            matriz[PosY+i][PosX+j] = cor
        }
    }
}

