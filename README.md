# Painting with Sorting Algorithms

This is a quick program I made using the SDL2 Library & C++ in order to visualize Sorting Methods in action by rendering each correctly placed element in an array.

## Compilers

g++ quicksort.cpp -o quicksort.exe -Iinc -Llib -lSDL2 -lopengl32
g++ mergesort.cpp -o mergesort.exe -Iinc -Llib -lSDL2 -lopengl32
g++ bubblesort.cpp -o bubblesort.exe -Iinc -Llib -lSDL2 -lopengl32
g++ insertionsort.cpp -o insertionsort.exe -Iinc -Llib -lSDL2 -lopengl32
g++ selectionsort.cpp -o selectionsort.exe -Iinc -Llib -lSDL2 -lopengl32
g++ countingsort.cpp -o countingsort.exe -Iinc -Llib -lSDL2 -lopengl32
g++ shakersort.cpp -o shakersort.exe -Iinc -Llib -lSDL2 -lopengl32
g++ radixsort.cpp -o radixsort.exe -Iinc -Llib -lSDL2 -lopengl32
g++ bogosort.cpp -o bogosort.exe -Iinc -Llib -lSDL2 -lopengl32

## Instructions

When you compile them, open the executables and press Enter once the array is ready to be sorted. Once the executable finishes sorting the list, an image will be fully rendered, and a the console will log the time elapsed for the array to be sorted.

To change the number of arrays being sorted, replace 'image.bmp' with a copy of an image of your choice with a .bmp format, make sure to rename it 'image.bmp'.
