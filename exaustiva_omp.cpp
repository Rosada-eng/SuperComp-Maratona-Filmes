#include <bitset>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <vector>

#define NMAX 64
#define DAYHOURS 24

using namespace std;

struct Movie {
    int id;
    int category;
    bitset<DAYHOURS> time; // 24 bits para representar as 24h do dia
};

struct Result {
    long int idx;
    long int amount;
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

        // seta os bits correspondentes ao horário do filme
        for (int j = start_time; j < end_time; j++) {
            m.time.set(j);
        }

        book.push_back(m);
    }
}

void read_max_categories_amount(int k, vector<int> &cat_max_size) {

    for (int i = 0; i < k; i++) {
        int max_size = 0;
        cin >> max_size;
        cat_max_size.push_back(max_size);
    }
}

int main(int argc, char *argv[]) {
    /**
     * Obtém os filmes a partir de um arquivo de entrada no formato:
     * n, k
     * k1_max, k2_max, k3_max, ... , kN_max
     * start, end, category
     * ...
     * start, end, category
     */

    int n = 0;
    int k = 0;

    cin >> n;
    cin >> k;

    if (n > NMAX) {
        cout << "Erro: n > NMAX" << endl;
        return 1;
    }

    // carrega o máximo de cada categoria no vetor
    vector<int> categories_max_size;
    categories_max_size.reserve(k);
    read_max_categories_amount(k, categories_max_size);

    // carrega todos os filmes no vetor
    vector<Movie> book;
    read_movies_booking(n, book);

    // DEBUG -- imprime os filmes lidos
    // for (int i = 0; i < n; i++) {
    //     cout << book[i].id << " " << book[i].category << " " << book[i].time << endl;
    // }

    long long int all_combinations_size = pow(2, n);
    cout << "all_combinations_size: " << all_combinations_size << endl;

    //! problema: Criar um vetor do tamanho de bilhões!
    // vector<Result> movies_watched_amount;
    long int idx_max_combination = 0;
    uint max_movies_amount = 0;
    // [i] -> quantidade de filmes assistidos na combinação i

    // percorre todas as combinações possíveis de filmes e chega se são válidas

    long int i = 0;
#pragma omp parallel for shared(idx_max_combination, max_movies_amount) num_threads(8)
    for (i = 0; i < all_combinations_size; i++) {
        bitset<DAYHOURS> sessions(0);
        // produz um array de bits em q cada bit representa um filme
        // (1 = filme a assistir, 0 = filme a não assistir)

        bitset<NMAX> combination(i);

        // vetor que guarda a quantidade de filmes assistidos de cada categoria
        vector<int> current_categories_watched(k, 0);

        int debug_code = 0;
        for (int m_idx = 0; m_idx < n; m_idx++) {
            // Lê o betor de bits (de trás pra frente)
            // se o filme for a assistir,
            if (combination[m_idx] == 1) {
                Movie movie = book[m_idx];

                // checa capacidade de categorias
                if (current_categories_watched[movie.category] + 1 > categories_max_size[movie.category]) {
                    // DEBUG: retorna -2 no vetor de resultados (excesso de categorias)
                    debug_code = -2;
                    break;
                }

                // checa conflito de horários
                if ((sessions & movie.time).any()) {
                    // DEBUG: retorna -1 no vetor de resultados (conflito de horários)
                    debug_code = -1;
                    break;
                }

                // se não houver conflito de horários e a categoria tiver espaço, adiciona o filme
                current_categories_watched[book[m_idx].category]++;
                // ajusta os horários
                sessions |= movie.time;
            }
        }

        // se a combinação for válida, conta a quantidade de filmes assistidos
        if (debug_code >= 0) {
            long int movies_watched = combination.count();
#pragma omp critical
            {
                if (movies_watched > max_movies_amount) {
                    max_movies_amount = movies_watched;
                    idx_max_combination = i;
                }
            }

            // movies_watched_amount.push_back(Result{i, (long int)combination.count()});
        }
        // else {
        //     movies_watched_amount.push_back(Result{i, debug_code});
        // }
    }

    // encontra a combinação com a maior quantidade de filmes assistidos
    // int max_movies_watched = 0;
    // int max_movies_watched_idx = 0;
    // for (int i = 0; i < all_combinations_size; i++) {
    //     if (movies_watched_amount[i].amount > max_movies_watched) {
    //         max_movies_watched = movies_watched_amount[i].amount;
    //         max_movies_watched_idx = movies_watched_amount[i].idx;
    //     }
    // }

    // DEBUG -- imprime a combinação com a maior quantidade de filmes assistidos
    cout << "idx Combinação com maior quantidade de filmes assistidos: " << idx_max_combination << endl;

    // DEBUG -- imprime a quantidade de filmes assistidos
    cout << "Quantidade de filmes assistidos: " << max_movies_amount << endl;

    // DEBUG -- imprime os filmes assistidos
    cout << "Filmes assistidos: ";
    bitset<NMAX> combination(idx_max_combination);
    for (int i = 0; i < n; i++) {
        if (combination[i] == 1) {
            cout << book[i].id << " ";
        }
    }
    cout << endl;

    // // DEBUG -- imprime a quantidade de filmes assistidos de cada categoria
    cout << "Quantidade de filmes assistidos de cada categoria: ";
    vector<int> current_categories_watched(k, 0);
    for (int i = 0; i < n; i++) {
        if (combination[i] == 1) {
            current_categories_watched[book[i].category]++;
        }
    }
    for (int i = 0; i < k; i++) {
        cout << current_categories_watched[i] << " ";
    }
    cout << endl;

    // // DEBUG -- imprime as horas ocupadas assistindo filme
    bitset<DAYHOURS> sessions(0);
    for (int i = 0; i < n; i++) {
        if (combination[i] == 1) {
            sessions |= book[i].time;
        }
    }
    cout << "Horas ocupadas assistindo filme: ";
    cout << sessions.count() << endl;
    cout << sessions << endl;

    // DEBUG -- imprime os filmes assistidos com horário de início e fim
    cout << "Filmes assistidos com horário de início e fim: " << endl;
    for (int i = 0; i < n; i++) {
        if (combination[i] == 1) {
            cout << book[i].id << "- ";

            // find start time
            int start_time = 0;
            for (int j = 0; j < DAYHOURS; j++) {
                if (book[i].time[j] == 1) {
                    start_time = j;
                    break;
                }
            }

            // find end time
            int end_time = 0;
            for (int j = DAYHOURS - 1; j >= 0; j--) {
                if (book[i].time[j] == 1) {
                    end_time = j;
                    break;
                }
            }

            cout << start_time << " > " << end_time << endl;
        }
    }

    return 0;
}