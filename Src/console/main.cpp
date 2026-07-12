#include "console.h"

int main()
{
    Console console(
        1280,
        720,
        "TinyVM"
    );

    while (console.isOpen())
    {
        console.pollEvents();
        console.present();
    }



    return 0;
}