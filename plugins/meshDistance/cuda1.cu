#include <stdio.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include "UI.h"

// Funkcja wykonywana r�wnolegle na wielu danych
// przez jednostki strumieniowe GPU
__global__ void square_array(float *a, int N)
{
	// Obliczamy numer w�tku na podstawie numeru bloku,
	// numeru w�tku w bloku i ilo�ci w�tk�w w bloku
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	// Warunek, aby nie wychodzi� poza zakres tablicy,
	// kt�r� b�dziemy przetwarza� oraz w�a�ciwe obliczenie
	// czyli obliczenie kwadratu danego pola, za kt�re
	// odpowiedzialny jest dany w�tek
	if (idx<N) a[idx] = a[idx] * a[idx];
}

// Funkcja wywo�ywana na CPU
void cuda_test()
{
	float *a_h, *a_d;  // Wska�niki do wykorzystywanych tablic
	const int N = 100;  // D�ugo�� tablic
	size_t size = N * sizeof(float); // Rozmiar tablic w bajtach

	a_h = (float *)malloc(size);        // Alokacja pami�ci przez CPU
	cudaMalloc((void **)&a_d, size);   // Alokacja pami�ci przez GPU

									   // Inicjalizacja danych przez CPU
	for (int i = 0; i<N; i++) a_h[i] = (float)i;
	// Skopiowanie danych do GPU
	cudaMemcpy(a_d, a_h, size, cudaMemcpyHostToDevice);

	// Ustalenie rozmiaru bloku
	int block_size = 4;
	// Obliczenie liczby blok�w koniczecznych do przetworzenia ca�ej tablicy
	int n_blocks = N / block_size + (N%block_size == 0 ? 0 : 1);

	// Specjalna sk�adnia wywo�ania podprogramu dla GPU z podaniem
	// nazwy funkcji, liczby i rozmiaru bloku oraz parametr�w funkcji
	square_array << < n_blocks, block_size >> > (a_d, N);
	// Pobranie danych wynikowych z GPU do CPU
	cudaMemcpy(a_h, a_d, sizeof(float)*N, cudaMemcpyDeviceToHost);

	// Wypisanie wynik�w
	for (int i = 0; i<N; i++) UI::STATUSBAR::printf("%d %f\n", i, a_h[i]);
	// Zwalnianie
	free(a_h); cudaFree(a_d);
}
