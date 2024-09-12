void hold(Tetromino *tetromino, Tetromino *hold, bool *canHold) 
{
    if(*canHold)
    {
        Tetromino temp = *tetromino;
        memcpy(tetromino, hold, sizeof(*tetromino));
        memcpy(hold, &temp, sizeof(*tetromino));
        *canHold = false;
    }
}

//peça congelada -> canHold = true
