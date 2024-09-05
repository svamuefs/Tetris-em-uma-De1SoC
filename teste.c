// #include <intelfpgaup/KEY.h> 
// #include <intelfpgaup/accel.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){

    int sair;
    int valorBotoes;

    int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;

    KEY_open();
    accel_open();

    accel_init();
    accel_format(1, 2);
    accel_calibrate();

    while (!sair)
    {
        printf("Sair?");
        scanf("%d", &sair);

        KEY_read(&valorBotoes);
        printf("%d", valorBotoes);

        accel_read(&acel_rdy, &acel_tap, &acel_dtap, &acel_x, 
                   &acel_y, &acel_z,&acel_mg);
        printf("%d %d %d %d %d %d %d",
                acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg);

    }
}