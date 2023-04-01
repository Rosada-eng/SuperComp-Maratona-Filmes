#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

bool DEBUG = false; // VARIABLE TO PRINT DEBUG MESSAGES

struct movie {
    int id;
    int start;
    int end;
    int category;
};

void read_movies_booking(int size, vector<movie> &book) {
    for (int i = 0; i < size; i++) {
        movie m;
        m.id = i;
        cin >> m.start;
        cin >> m.end;

        if (m.end < m.start) {
            m.end += 24; // acaba no dia seguinte
        }

        int category = 0;
        cin >> category;

        m.category = category - 1;

        // HARD FIX -> Alinha categoria com o índice dos arrays

        book.push_back(m);
    }
}

void sort_book_by_ending_time(vector<movie> &book, vector<int> &sorting_list) {
    sort(sorting_list.begin(), sorting_list.end(), [&](int i, int j) { return book[i].end < book[j].end; });
}

void book_a_movie(vector<movie> &book, vector<int> &movies_booked, vector<int> &categories_booked, int movie_id) {
    movies_booked.push_back(movie_id);
    categories_booked.push_back(book[movie_id].category);
}

double get_probability() {
    // srand (time(NULL)) gera um seed diferente para cada vez que executar o programa
    srand(time(NULL));

    // ! Para garantir reprodutibilidade, altere o seed para um valor FIXO
    unsigned int seed = rand();

    // tornar o engine statico garante que a geração de números aleatórios a cada chamada de função
    static default_random_engine generator(seed);
    static uniform_real_distribution<double> distribution(0.0, 1.0);

    return distribution(generator);
}

int get_random_index(int min, int max) {
    // srand (time(NULL)) gera um seed diferente para cada vez que executar o programa
    srand(time(NULL));

    // ! Para garantir reprodutibilidade, altere o seed para um valor FIXO
    unsigned int seed = rand();

    // tornar o engine statico garante que a geração de números aleatórios a cada chamada de função
    static default_random_engine generator(seed);
    static uniform_int_distribution<int> distribution(min, max);
    int random_index = distribution(generator);

    return random_index;
}

bool is_movie_available(vector<int> &movies_booked, vector<int> &categories_booked, vector<int> &categories_max_size, vector<movie> &book, int movie_id, int last_seen_finishes_at) {
    // checa se o filme já não foi assistido
    bool movie_is_taken = find(movies_booked.begin(), movies_booked.end(), movie_id) != movies_booked.end();

    // checa se estourou limite para categoria
    bool category_limit_exceeded = count(categories_booked.begin(), categories_booked.end(), book[movie_id].category) >= categories_max_size[book[movie_id].category];

    // checa se o filme anterior já acabou no horário de início do filme atual
    bool is_time_available = (movies_booked.size() == 0) || (book[movie_id].start >= last_seen_finishes_at);

    if (DEBUG) {
        cout << "M" << movie_id << "\t" << book[movie_id].start << "~" << book[movie_id].end << " C" << book[movie_id].category;

        if (!is_time_available) {
            cout << "\t time is not available "
                 << "(" << book[movies_booked.back()].end << ")";
        }
        if (category_limit_exceeded) {
            cout << "\t category limit exceeded";
        }
        if (movie_is_taken) {
            cout << "\t movie is taken";
        }
        if (!category_limit_exceeded && !movie_is_taken && is_time_available) {
            cout << "\t is available";
        }
        cout << endl;
    }

    // passa em todos os filtros
    if (!category_limit_exceeded && !movie_is_taken && is_time_available) {

        return true;
    }

    return false;
}

int main() {
    vector<movie> book;
    int n = 0;
    int k = 0;

    cin >> n;
    cin >> k;

    vector<int> categories_max_size;
    categories_max_size.reserve(k);

    for (int i = 0; i < k; i++) {
        int max_size = 0;
        cin >> max_size;
        categories_max_size.push_back(max_size);
    }

    cout << "max_by_category: ";
    for (int i = 0; i < k; i++) {
        cout << categories_max_size[i] << " ";
    }

    cout << endl;

    vector<int> movies_booked;
    vector<int> categories_booked;

    read_movies_booking(n, book);

    // utilizaremos ordenação indireta
    vector<int> sorting_movies;
    sorting_movies.reserve(n);

    for (int i = 0; i < n; i++) {
        sorting_movies.push_back(i);
    }

    sort_book_by_ending_time(book, sorting_movies);

    int last_seen_finishes_at = 0;
    for (int day = 0; day < 2; day++) {
        for (int i = 0; i < n; i++) {
            double probability = get_probability();

            int movie_id = sorting_movies[i];

            if (probability < 0.25) {
                // 25% de chance de pegar um filme aleatório

                int random_idx = get_random_index(i, n - 1);
                movie_id = sorting_movies[random_idx];
            }

            // 75% de chance de pegar o próximo filme que termina mais cedo

            bool is_available = is_movie_available(movies_booked, categories_booked, categories_max_size, book, movie_id, last_seen_finishes_at);

            if (is_available) {
                book_a_movie(book, movies_booked, categories_booked, movie_id);
                last_seen_finishes_at = book[movie_id].end;
            }
        }

        if (last_seen_finishes_at >= 24) {
            last_seen_finishes_at -= 24;
        }
    }

    for (uint i = 0; i < movies_booked.size(); i++) {
        cout << movies_booked[i] << " ";
    }

    cout << endl
         << "total: " << movies_booked.size() << endl;

    // TODO: implementar um mapa com os horários alocados para filmes

    cout << endl;
    return 0;
}