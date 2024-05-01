#include <iostream>
#include <vector>
#include "omp.h"

using namespace std;

const int rows = 4; // Кількість рядків у двовимірному масиві
const int cols = 8; // Кількість колонок у двовимірному масиві

// Функція для встановлення кількості потоків
void set_num_threads(int num_threads) {
    omp_set_num_threads(num_threads); // Встановлює кількість потоків
}

// Ініціалізація двовимірного масиву
vector<vector<int>> init_matrix() {
    vector<vector<int>> matrix(rows, vector<int>(cols));

#pragma omp parallel for collapse(2) // Паралельна ініціалізація масиву
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (i + 1) * (j + 1); // Заповнення масиву деякими значеннями
        }
    }

    return matrix;
}

// Функція для знаходження загальної суми елементів двовимірного масиву
long long find_total_sum(const vector<vector<int>>& matrix) {
    long long sum = 0;

#pragma omp parallel for reduction(+:sum) collapse(2) // Паралельний цикл із редукцією
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            sum += matrix[i][j];
        }
    }

    return sum;
}

// Функція для знаходження рядка з мінімальною сумою
pair<int, long long> find_min_row_sum(const vector<vector<int>>& matrix) {
    int min_row = -1;
    long long min_sum = numeric_limits<long long>::max();

#pragma omp parallel for // Паралельний цикл
    for (int i = 0; i < rows; i++) {
        long long row_sum = 0;

        for (int j = 0; j < cols; j++) {
            row_sum += matrix[i][j];
        }

#pragma omp critical // Критична секція для уникнення гонок даних
        {
            if (row_sum < min_sum) {
                min_sum = row_sum;
                min_row = i;
            }
        }
    }

    return { min_row, min_sum };
}

int main() {
    vector<vector<int>> matrix = init_matrix(); // Ініціалізація масиву

    set_num_threads(2); // Встановлюємо кількість потоків

    double start_time = omp_get_wtime(); // Початковий час вимірювання

#pragma omp parallel sections // Паралельні секції для паралельного виконання
    {
#pragma omp section
        {
            long long total_sum = find_total_sum(matrix); // Обчислення загальної суми
            cout << "Total sum of all elements: " << total_sum << endl;
        }

#pragma omp section
        {
            pair<int, long long> min_row_sum = find_min_row_sum(matrix); // Знаходження рядка з мінімальною сумою
            cout << "Row with minimum sum: " << min_row_sum.first
                << " with sum: " << min_row_sum.second << endl;
        }
    }

    double end_time = omp_get_wtime(); // Кінцевий час вимірювання
    cout << "Total execution time: " << (end_time - start_time) << " seconds" << endl;

    return 0;
}
