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
        // printf("Sair?");
        // scanf("%d", &sair);

        // KEY_read(&valorBotoes);
        // printf("%d", valorBotoes);

        // accel_read(&acel_rdy, &acel_tap, &acel_dtap, &acel_x, 
        //            &acel_y, &acel_z,&acel_mg);
        // printf("%d %d %d %d %d %d %d",
        //         acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg);

        video_open();
        // video_erase();
        // video_clear();
    video_text(0,10, "  /$$$$$$   /$$$$$$  /$$      /$$ /$$$$$$$$    ");
    video_text(0,11, " /$$__  $$ /$$__  $$| $$$    /$$$| $$_____/    ");
    video_text(0,12, "| $$  ||__/| $$  || $$| $$$$  /$$$$| $$            ");
    video_text(0,13, "| $$ /$$$$| $$$$$$$$| $$ $$/$$ $$| $$$$$       ");
    video_text(0,14, "| $$|_  $$| $$__  $$| $$  $$$| $$| $$__/        ");
    video_text(0,15, "| $$  || $$| $$  | $$| $$||  $ | $$| $$            ");
    video_text(0,16, "|  $$$$$$/| $$  | $$| $$ ||/  | $$| $$$$$$$$     ");
    video_text(0,17, " ||______/ |__/  |__/|__/     |__/|________/      ");

    video_text(10,20, "  /$$__  $$| $$   | $$| $$_____/| $$__  $$    ");
    video_text(10,21, " | $$  || $$| $$   | $$| $$      | $$  || $$");
    video_text(10,22, " | $$  | $$|  $$ / $$/| $$$$$   | $$$$$$$/");
    video_text(10,23, "  | $$  | $$ ||  $$ $$/ | $$__/   | $$__  $$");
    video_text(10,24, " | $$  | $$  ||  $$$/  | $$      | $$  || $$");
    video_text(10,25, "  |  $$$$$$/   ||  $/   | $$$$$$$$| $$  | $$");
    video_text(10,26, " ||______/     ||_/    |________/|__/  |__/");



  

  




        video_show();
        video_close();
    }
}
