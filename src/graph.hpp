#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>

template <int n> struct Graph {
  std::array<std::array<int, n>, n> matrix;
  bool end = false;
  int lead = n * n - 1;
  int nb_edges = 0;

  Graph() {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        matrix[i][j] = 0;
      }
    }
  }

  Graph &operator++() {
    for (int i = n * n - 1; i >= 0; i--) {
      // std::cout << "c" << i << '\n';
      if (i == 0) {
        end = true;
        break;
      }
      if (i / n <= i % n) {
        continue;
      }
      lead = (lead < i) ? lead : i;
      if (i == lead && matrix[i / n][i % n] == 1) {
        matrix[i / n][i % n] = 0;
        matrix[i % n][i / n] = 0;
        nb_edges--;
        continue;
      }
      matrix[i / n][i % n]++;
      matrix[i % n][i / n]++;
      nb_edges++;
      if (matrix[i / n][i % n] > 2) {
        matrix[i / n][i % n] = 0;
        matrix[i % n][i / n] = 0;
        nb_edges -= 2;
        if (i == 1) {
          end = true;
        }
      } else if (matrix[i / n][i % n] == 2) {
        nb_edges--;
        break;
      } else {
        break;
      }
    }
    return *this;
  }

  // ++G but with min/max edges
  Graph &next(const int a, const int b) {
    bool all_one_before = true;
    for (int i = n * n - 1; i >= 0; i--) {
      // std::cout << "c" << i << '\n';
      if (i == 0) {
        end = true;
        break;
      }
      if (i / n <= i % n) {
        continue;
      }
      lead = (lead < i) ? lead : i;
      if (i == lead && matrix[i / n][i % n] == 1) {
        matrix[i / n][i % n] = 0;
        matrix[i % n][i / n] = 0;
        all_one_before = false;
        nb_edges--;
        // std::cout << "lead" << i << '\n';
        continue;
      }
      if (nb_edges > b) {
        if (matrix[i / n][i % n]) {
          matrix[i / n][i % n] = 0;
          matrix[i % n][i / n] = 0;
          all_one_before = false;
          nb_edges--;
          // std::cout << "too much" << i << '\n';
          continue;
        } else {
          continue;
        }
      }
      if (nb_edges >= b) {
        if (!matrix[i / n][i % n]) {
          matrix[i / n][i % n] = 1;
          matrix[i % n][i / n] = 1;
          nb_edges++;
          // std::cout << "almost too much" << i << '\n';
          i = n * n - 1;
          continue;
        }
      }

      matrix[i / n][i % n]++;
      matrix[i % n][i / n]++;
      nb_edges++;
      if (matrix[i / n][i % n] > 2) {
        matrix[i / n][i % n] = 0;
        matrix[i % n][i / n] = 0;
        all_one_before = false;
        nb_edges -= 2;
        if (i == 1) {
          end = true;
        }
        // std::cout << "2->0" << i << '\n';
        continue;
      } else if (matrix[i / n][i % n] == 2) {
        all_one_before = false;
        nb_edges--;
        // std::cout << "1->2" << i << '\n';
      }
      if (nb_edges < a && !all_one_before) {
        all_one_before = true;
        i = n * n - 1;
        // std::cout << "c1" << i << '\n';
        continue;
      } else if (nb_edges < a && all_one_before) {
        // std::cout << "c2" << i << '\n';
        continue;
      }
      // std::cout << "break" << i << '\n';
      break;
    }
    return *this;
  }

  bool is_clique() {
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix[i][j] > 0) {
          continue;
        }
        bool good = false;
        for (int k = 0; k < n && !good; k++) {
          if (i != k && j != k) {
            if ((matrix[i][k] == 1 && matrix[j][k] == 2) ||
                (matrix[i][k] == 2 && matrix[j][k] == 1)) {
              good = true;
            }
          }
        }
        if (!good) {
          return false;
        }
      }
    }
    return true;
  }

  bool is_signed_clique() {
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix[i][j] > 0) {
          continue;
        }
        bool good = false;
        for (int k = 0; k < n && !good; k++) {
          if (i != k && j != k && matrix[i][k] && matrix[j][k]) {
            for (int l = 0; l < n && !good; l++) {
              if (l != i && l != j && l != k && matrix[i][l] && matrix[j][l]) {
                int nb = (matrix[i][k] == 2) + (matrix[j][k] == 2) +
                         (matrix[i][l] == 2) + (matrix[j][l] == 2);
                if (nb % 2) {
                  good = true;
                }
              }
            }
          }
        }
        if (!good) {
          return false;
        }
      }
    }
    return true;
  }

  bool operator==(Graph &g2) {
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix[i][j] != g2.matrix[i][j]) {
          return false;
        }
      }
    }
    return true;
  }
  /*
    bool is_eq(std::array<int, n> perm) {
      bool inverted = false;
      for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
          std::cout << matrix[i][j] << matrix[perm[i]][perm[j]] << std::endl;
          if (!inverted && matrix[i][j] != matrix[perm[i]][perm[j]]) {
            if (!matrix[i][j] || !matrix[perm[i]][perm[j]]) {
              return false;
            }
            inverted = true;
          }
          if (inverted && matrix[i][j] != (3 - matrix[perm[i]][perm[j]]) % 3) {
            return false;
          }
        }
      }
      return true;
    }
  */

  bool is_greater(std::array<int, n> perm) {
    for (int i = 0; i < n * n; i++) {
      int k = i / n;
      int j = i % n;
      if (matrix[k][j] < matrix[perm[k]][perm[j]]) {
        return true;
      } else if (matrix[k][j] > matrix[perm[k]][perm[j]]) {
        return false;
      }
    }
    return true;
  }

  bool is_greater_signed(std::array<int, n> perm, std::array<int, n> sign) {
    for (int i = 0; i < n * n; i++) {
      int k = i / n;
      int j = i % n;
      if (matrix[k][j] < ((sign[perm[k]] == sign[perm[j]])
                              ? matrix[perm[k]][perm[j]]
                              : (3 - matrix[perm[k]][perm[j]]) % 3)) {
        return true;
      } else if (matrix[k][j] > ((sign[perm[k]] == sign[perm[j]])
                                     ? matrix[perm[k]][perm[j]]
                                     : (3 - matrix[perm[k]][perm[j]]) % 3)) {
        return false;
      }
    }
    return true;
  }

  bool is_first() {
    std::array<int, n> perm;
    for (int i = 0; i < n; i++) {
      perm[i] = i;
    }

    bool result = true;
    do {
      result = result && this->is_greater(perm);
    } while (std::next_permutation(perm.begin(), perm.end()) && result);

    return result;
  }

  bool is_first_signed() {
    std::array<int, n> perm;
    for (int i = 0; i < n; i++) {
      perm[i] = i;
    }

    bool result = true;
    do {
      std::array<int, n> sign;
      for (int i = 0; i < n; i++) {
        sign[i] = 0;
      }
      bool stop = false;
      do {
        result = result && this->is_greater_signed(perm, sign);
        for (int i = n - 1; i >= 0; i--) {
          sign[i] = !sign[i];
          if (sign[i]) {
            break;
          }
          if (i == 0) {
            stop = true;
          }
        }
      } while (!stop && result);
    } while (std::next_permutation(perm.begin(), perm.end()) && result);

    return result;
  }

  bool is_complete_diff_of_kn() {
    bool diff = false;
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (!matrix[i][j]) {
          return false;
        }
        if (matrix[i][j] == 2) {
          diff = true;
        }
      }
    }
    return diff;
  }

  void save(std::string filename) {
    std::ofstream file(filename);
    file << "graph {" << '\n';
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix[i][j] == 1) {
          file << (char)('a' + i) << " -- " << (char)('a' + j) << ";\n";
        } else if (matrix[i][j] == 2) {
          file << (char)('a' + i) << " -- " << (char)('a' + j)
               << " [style=dotted];\n";
        }
      }
    }
    file << "}\n";
    file.close();
  }

  int edges() { return nb_edges; }
};

template <int n> std::ostream &operator<<(std::ostream &os, Graph<n> &G) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      os << G.matrix[i][j] << " ";
    }
    os << "\n";
  }
  return os;
}
#endif
