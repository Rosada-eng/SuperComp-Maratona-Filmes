#include <iostream>
#include <vector>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/copy.h>

using namespace std;

#define MAXCATEGORIES 10

struct Movie {
    int id;
    int start;
    int end;
    int category;
};



void read_movies_booking(int size, vector<Movie> &book) {
    /**
     * Recebe um inteiro com a quantidade de filmes a ser lida
     * Recebe um vetor de book a ser preenchido com os filmes
     */
    for (int i = 0; i < size; i++) {
        Movie m;
        int start_time, end_time;
        m.id = i;
        cin >> start_time;
        cin >> end_time;

        if (end_time < start_time) {
            end_time = 23; // acaba na última hora do dia
        }

        int category = 0;
        cin >> category;

        // HARD FIX -> Alinha categoria com o índice dos arrays
        m.category = category - 1;

        m.start = start_time;
        m.end = end_time;

        book.push_back(m);
    }
}

long int get_exhibition_time(Movie movie) {
    /**
     * Recebe um vetor de inteiros target a ser preenchido com os horários de exibição
     * Recebe um vetor de filmes movies
     */
    int exhibition_time = 0;
    for (int j = movie.start; j <= movie.end; j++) {
        exhibition_time |= (1 << j);
    }

    return exhibition_time;
}

struct count_watched_movies {

    int n;
    int k;

    long int *exhibition_time;
    int *categories;

    int *categories_max_size;

    count_watched_movies(
        int n_,
        int k_,
        long int *exhibition_time_,
        int *categories_,
        int *categories_max_size_) :

        n(n_),
        k(k_),
        exhibition_time(exhibition_time_),
        categories(categories_),
        categories_max_size(categories_max_size_) {}

    __device__
    int operator()(const int &allocation) const
    {
        int watched_movies_count = 0;

        int categories_watched[MAXCATEGORIES];
        for (int i = 0; i < MAXCATEGORIES; i++) {
            categories_watched[i] = 0;
        }

        long int available_exhibition_time = 0;

        // para cada um dos filmes indicados como 1 (a assistir), checa se é possível assistir
        for (int i = 0; i < n; i++) {
            // caso o filme tenha sido alocado (1), analisa validade
            if (allocation & (1 << i)) {
                // checa se há disponibilidade de categoria
                if (categories_watched[categories[i]] < categories_max_size[categories[i]]) {
                    // checa se o horário de exibição está disponível
                    long int has_override = available_exhibition_time & exhibition_time[i];
                    if (has_override == 0)  {
                        watched_movies_count++;
                        categories_watched[categories[i]]++;
                        available_exhibition_time |= exhibition_time[i];
                    } else {

                        return -1;
                    }
                } else {
                    return -2;
                }
            }
        }

        return watched_movies_count;
    }

};
int main(int argc, char* argv[]){

    int n = 0;
    int k = 0;

    cin >> n;
    cin >> k;

    // Armazena o máximo de filmes para cada categoria
    thrust::host_vector<int> h_categories_max_size(k, 0);
    for (int i = 0; i < k; i++) {
        int max_size = 0;
        cin >> max_size;
        h_categories_max_size[i] = max_size;
    }

    // Lê os filmes e os armazena em um vetor
    vector<Movie> movies;
    read_movies_booking(n, movies);

    // Vamos criar alguns vetores: Categoria e Horários de Exibição

    thrust::host_vector<int> h_categories(n);
    thrust::host_vector<long int> h_exhibition_time(n);

    // preenche o vetor h_exhibition com um inteiro que representa o binário de horas em que o filme é exibido
    for (int i=0; i<n; i++){
        h_categories[i] = movies[i].category;
        h_exhibition_time[i] = get_exhibition_time(movies[i]);
    }


    // Vamos obter um vetor de inteiros, cuja transformação em bits representa cada filme a ser assistido
    
    thrust::device_vector<int> d_allocations(pow(2, n));
    thrust::sequence(d_allocations.begin(), d_allocations.end());

    // Vamos transformar os vetores para GPU
    thrust::device_vector<int> d_categories = h_categories;
    thrust::device_vector<long int> d_exhibition_time = h_exhibition_time;
    thrust::device_vector<int> d_categories_max_size = h_categories_max_size;

    // Vamos aplicar a função de verificação para testar cada possibilidade, se é válida e contar quantos filmes foram assistidos.
    thrust::transform(
        d_allocations.begin(),
        d_allocations.end(),
        d_allocations.begin(),
        count_watched_movies(
            n,
            k,
            thrust::raw_pointer_cast(d_exhibition_time.data()),
            thrust::raw_pointer_cast(d_categories.data()),
            thrust::raw_pointer_cast(d_categories_max_size.data())
        )
    );

    int max_watched_movies = thrust::reduce(d_allocations.begin(), d_allocations.end(), 0, thrust::maximum<int>());

    cout << "Máximo de filmes assistidos: " << max_watched_movies << endl;


    return 0;
}
