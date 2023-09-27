#include <stdio.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include "UI.h"

// Funkcja wykonywana równolegle na wielu danych
// przez jednostki strumieniowe GPU
__global__ void square_array(float *a, int N)
{
	// Obliczamy numer w¹tku na podstawie numeru bloku,
	// numeru w¹tku w bloku i iloœci w¹tków w bloku
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	// Warunek, aby nie wychodziæ poza zakres tablicy,
	// któr¹ bêdziemy przetwarzaæ oraz w³aœciwe obliczenie
	// czyli obliczenie kwadratu danego pola, za które
	// odpowiedzialny jest dany w¹tek
	if (idx<N) a[idx] = a[idx] * a[idx];
}

// Funkcja wywo³ywana na CPU
void cuda_test()
{
	float *a_h, *a_d;  // WskaŸniki do wykorzystywanych tablic
	const int N = 100;  // D³ugoœæ tablic
	size_t size = N * sizeof(float); // Rozmiar tablic w bajtach

	a_h = (float *)malloc(size);        // Alokacja pamiêci przez CPU
	cudaMalloc((void **)&a_d, size);   // Alokacja pamiêci przez GPU

									   // Inicjalizacja danych przez CPU
	for (int i = 0; i<N; i++) a_h[i] = (float)i;
	// Skopiowanie danych do GPU
	cudaMemcpy(a_d, a_h, size, cudaMemcpyHostToDevice);

	// Ustalenie rozmiaru bloku
	int block_size = 4;
	// Obliczenie liczby bloków koniczecznych do przetworzenia ca³ej tablicy
	int n_blocks = N / block_size + (N%block_size == 0 ? 0 : 1);

	// Specjalna sk³adnia wywo³ania podprogramu dla GPU z podaniem
	// nazwy funkcji, liczby i rozmiaru bloku oraz parametrów funkcji
	square_array << < n_blocks, block_size >> > (a_d, N);
	// Pobranie danych wynikowych z GPU do CPU
	cudaMemcpy(a_h, a_d, sizeof(float)*N, cudaMemcpyDeviceToHost);

	// Wypisanie wyników
	for (int i = 0; i<N; i++) UI::STATUSBAR::printf("%d %f\n", i, a_h[i]);
	// Zwalnianie
	free(a_h); cudaFree(a_d);
}
