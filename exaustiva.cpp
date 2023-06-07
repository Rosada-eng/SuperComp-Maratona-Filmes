#include <bitset>
#include <cmath>
#include <iostream>
#include <vector>

#define N 64

using namespace std;

struct Movie {
    int id;
    int category;
    bitset<48> time; // 48 bits para representar as 48h do fim de semana
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
            end_time += 24; // acaba no dia seguinte
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

    long int all_combinations_size = pow(2, n);

    vector<int> movies_watched_amount(all_combinations_size, 0);
    // [i] -> quantidade de filmes assistidos na combinação i

    // percorre todas as combinações possíveis de filmes e chega se são válidas

    long int i = 0;
    for (i = 0; i < all_combinations_size; i++) {
        bitset<48> sessions(0);
        // produz um array de bits em q cada bit representa um filme
        // (1 = filme a assistir, 0 = filme a não assistir)

        bitset<N> combination(i);

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
            }
        }

        // se a combinação for válida, conta a quantidade de filmes assistidos
        if (debug_code >= 0) {
            movies_watched_amount[i] = combination.count();
        } else {
            movies_watched_amount[i] = debug_code;
        }
    }

    // DEBUG -- imprime o vetor de resultados
    // for (int i = 0; i < all_combinations_size; i++) {
    //     cout << movies_watched_amount[i] << " ";
    // }
    // cout << endl;

    // encontra a combinação com a maior quantidade de filmes assistidos
    int max_movies_watched = 0;
    int max_movies_watched_idx = 0;
    for (int i = 0; i < all_combinations_size; i++) {
        if (movies_watched_amount[i] > max_movies_watched) {
            max_movies_watched = movies_watched_amount[i];
            max_movies_watched_idx = i;
        }
    }

    // DEBUG -- imprime a combinação com a maior quantidade de filmes assistidos
    cout << "idx Combinação com maior quantidade de filmes assistidos: " << max_movies_watched_idx << endl;

    // DEBUG -- imprime a quantidade de filmes assistidos
    cout << "Quantidade de filmes assistidos: " << max_movies_watched << endl;

    // DEBUG -- imprime os filmes assistidos
    cout << "Filmes assistidos: ";
    bitset<N> combination(max_movies_watched_idx);
    for (int i = 0; i < n; i++) {
        if (combination[i] == 1) {
            cout << book[i].id << " ";
        }
    }
    cout << endl;

    // DEBUG -- imprime a quantidade de filmes assistidos de cada categoria
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

    // DEBUG -- imprime as horas ocupadas assistindo filme
    bitset<48> sessions(0);
    for (int i = 0; i < n; i++) {
        if (combination[i] == 1) {
            sessions |= book[i].time;
        }
    }
    cout << "Horas ocupadas assistindo filme: ";
    cout << sessions.count() << endl;
    cout << sessions << endl;

    return 0;
}