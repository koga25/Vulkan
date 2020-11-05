#include <stdio.h>
#include "Header.h"

int main()
{
    Run();
}


/*
    todo:
        -Remake recreation of swapchain to include dynamic states (if resize to larger screen then remake swapchain, if not only remake image)
        -use a different family queue to do the transfer operations (choose one that only does transfer operations.)
        -Create another command pool to implement faster transfer between cpu and gpu
        
*/