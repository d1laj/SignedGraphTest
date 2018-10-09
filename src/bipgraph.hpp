#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>

template <int n> struct BipGraph {
  std::array<std::array<int, n>, n> matrix;
  std::array<int, n> degA;
  std::array<int, n> degB;
  bool end = false;
  std::array<int, n> leadA;
  std::array<int, n> leadB;
  int nb_edges = 0;

  BipGraph() {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        matrix[i][j] = 0;
      }
      degA[i] = 0;
      degB[i] = 0;
      leadA[i] = n * n;
      leadB[i] = n * n;
    }
  }

  BipGraph &operator++() { return nextWithIndex(n * n - 1); }

  BipGraph &nextWithIndex(int i0) {
    for (int i = i0; i >= 0; i--) {
      // std::cout << "c" << i << '\n';
      if (i == 0) {
        end = true;
        break;
      }

      if ((libre_a(i) || libre_b(i)) && matrix[i / n][i % n] == 1) {
        matrix[i / n][i % n] = 0;
        nb_edges--;
        degA[i / n]--;
        degB[i % n]--;
        if (leadA[i / n] == i) {
          leadA[i / n] = n * n;
        }
        if (leadB[i % n] == i) {
          leadB[i % n] = n * n;
        }
        continue;
      }

      if (matrix[i / n][i % n] == 0 && (degA[i / n] >= 3 || degB[i % n] >= 3)) {
        continue;
      } /*else if (matrix[i / n][i % n] > 0 &&
                 (degA[i / n] > 3 || degB[i % n] > 3)) {
        matrix[i / n][i % n] = 0;
        nb_edges--;
        degA[i / n]--;
        degB[i % n]--;
      } */
      else {
        matrix[i / n][i % n]++;
        nb_edges++;
        degA[i / n]++;
        degB[i % n]++;
        if (leadA[i / n] > i) {
          leadA[i / n] = i;
          /*
          for (int j = i / n + 1; j < n; j++) {
            if (leadB[j] > (i / n) * n + n - 1) {
              matrix[i / n][i % n] = 0;
              nb_edges--;
              degA[i / n]--;
              degB[i % n]--;
              leadA[i / n] = n * n;
              if (leadB[i % n] == i) {
                leadB[i % n] = n * n;
              }
              continue;
            }
          }
          */
        }
        if (leadB[i % n] > i) {
          leadB[i % n] = i;
          for (int j = n - 1; j > i / n; j--) {
            if (leadA[j] > j * n + (i % n)) {
              // TODO
            }
          }
        }
      }

      if (matrix[i / n][i % n] > 2) {
        matrix[i / n][i % n] = 0;
        nb_edges -= 2;
        degA[i / n] -= 2;
        degB[i % n] -= 2;
      } else if (matrix[i / n][i % n] == 2) {
        nb_edges--;
        degA[i / n]--;
        degB[i % n]--;
        break;
      } else {
        break;
      }
    }
    return *this;
  }

  bool libre_a(int idx) { return leadA[idx / n] == idx; }

  bool libre_b(int idx) { return leadB[idx % n] == idx; }

  bool is_first() {
    for (int i = 0; i < n - 1; i++) {
      if (mod(leadA[i]) < mod(leadA[i + 1]) && leadA[i] != n * n) {
        return false;
      }
    }
    for (int i = 0; i < n - 1; i++) {
      if (leadB[i] / n < leadB[i + 1] / n && leadB[i] != n * n) {
        return false;
      }
    }
    return true;
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

  int mod(int i) {

    if (i == n * n) {
      return n;
    }
    return i % n;
  }
};

template <int n> std::ostream &operator<<(std::ostream &os, BipGraph<n> &G) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      os << G.matrix[i][j] << " ";
    }
    os << " | " << G.degA[i] << " | " << G.leadA[i] << "\n";
  }
  for (int j = 0; j < n; j++) {
    os << "_"
       << "_";
  }
  os << '\n';
  for (int j = 0; j < n; j++) {
    os << G.degB[j] << " ";
  }
  os << '\n';
  for (int j = 0; j < n; j++) {
    os << "_"
       << "_";
  }
  os << '\n';
  for (int j = 0; j < n; j++) {
    os << G.leadB[j] << " ";
  }
  os << '\n';
  return os;
}
#endif
