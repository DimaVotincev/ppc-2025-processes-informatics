#include "votincev_d_alternating_values/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include "util/include/util.hpp"
#include "votincev_d_alternating_values/common/include/common.hpp"

namespace votincev_d_alternating_values {

// это у всех одинаковое
VotincevDAlternatingValuesMPI::VotincevDAlternatingValuesMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

// проверка данных на адекватность
bool VotincevDAlternatingValuesMPI::ValidationImpl() {
  return !(GetInput().empty());
}

// препроцессинг (например в КГ)
bool VotincevDAlternatingValuesMPI::PreProcessingImpl() {
  vect_data = GetInput();
  GetOutput() = -1;  // специальное значение
  return true;
}

// код MPI
bool VotincevDAlternatingValuesMPI::RunImpl() {
  // double start_time = 0, end_time = 0;
  // start_time = MPI_Wtime();

  int all_swaps = 0;
  // получаю кол-во процессов
  int process_n;
  MPI_Comm_size(MPI_COMM_WORLD, &process_n);

  // получаю ранг процесса
  int proc_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

  // если процессов больше, чем элементов
  if (static_cast<int>(vect_data.size()) < process_n) {
    process_n = static_cast<int>(vect_data.size());
  }

  int part_size;
  if (proc_rank == 0) {
    int base = vect_data.size() / process_n;    // минимум на обработку
    int remain = vect_data.size() % process_n;  // остаток (распределим)

    int start_id = 0;
    for (int i = 1; i < process_n; i++) {
      part_size = base;
      if (remain) {  // если есть остаток - то распределяем между первыми
        part_size++;
        remain--;
      }

      part_size++;  // цепляем правого соседа, 0-й будет последним - поэтому он будет последний кусок считать

      // Вместо пересылки данных - пересылаем индексы начала и конца
      int indices[2] = {start_id, start_id + part_size};
      MPI_Send(&indices[0], 2, MPI_INT, i, 0, MPI_COMM_WORLD);

      start_id += part_size - 1;

      // вычисляю для последнего
      if (i == (process_n - 1)) {
        part_size = base + remain;
      }
    }

    // 0й процесс считает свою часть
    for (size_t j = start_id + 1; j < vect_data.size(); j++) {
      if ((vect_data[j - 1] < 0 && vect_data[j] >= 0) || (vect_data[j - 1] >= 0 && vect_data[j] < 0)) {
        all_swaps++;
      }
    }

    // получаю посчитанные swap от процессов
    for (int i = 1; i < process_n; i++) {
      int tmp;
      // что , сколько, тип, кому, тег, коммуникатор
      MPI_Recv(&tmp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      all_swaps += tmp;
    }

    // на этом этапе 0-й процесс сделал всю работу
  }

  for (int i = 1; i < process_n; i++) {
    if (proc_rank == i) {
      // получаем индексы вместо данных
      int indices[2];
      MPI_Recv(indices, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      int start_index = indices[0];
      int end_index = indices[1];

      int swap_count = 0;
      // обрабатываем свой диапазон из глобального вектора v
      for (int j = start_index + 1; j < end_index; j++) {
        if ((vect_data[j - 1] < 0 && vect_data[j] >= 0) || (vect_data[j - 1] >= 0 && vect_data[j] < 0)) {
          swap_count++;
        }
      }

      MPI_Send(&swap_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
  }

  // только 0й процесс владеет правильным результатом, остальные - его формируют
  // если остальным посылать - будет проблема, на 20 процессах уже не работает
  if (proc_rank == 0) {
    GetOutput() = all_swaps;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  // end_time = MPI_Wtime();
  // if (proc_rank == 0) {
  //   std::cout << "MPI_was_working:" << (end_time - start_time) << "\n";
  // }

  MPI_Bcast(&all_swaps, 1, MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = all_swaps;
  MPI_Barrier(MPI_COMM_WORLD);

  // ========== пересылка, но не через Bcast
  //
  // if (proc_rank == 0) {
  //   // отправляем всем процессам корректный результат
  //   for (int i = 1; i < process_n; i++) {
  //     MPI_Send(&all_swaps, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
  //   }
  //   // сами устанавливаем значение
  //   GetOutput() = all_swaps;
  // }
  // for (int i = 1; i < process_n; i++) {
  //   if (proc_rank == i) {
  //     int allSw;
  //     MPI_Recv(&allSw, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //     GetOutput() = allSw;
  //   }
  // }
  //
  // ========== пересылка, но не через Bcast

  return true;
}

// удобно возвращаем данные (???)
bool VotincevDAlternatingValuesMPI::PostProcessingImpl() {
  return true;
}

}  // namespace votincev_d_alternating_values
