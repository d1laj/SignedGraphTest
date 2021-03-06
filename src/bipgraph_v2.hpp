#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

// Warning code dégueux extrait d'une api écrit en 50 av JC.

using BoostGraph = boost::adjacency_list<boost::vecS,
                         boost::vecS,
                         boost::undirectedS,
                         boost::property<boost::vertex_index_t, int>
                         >;

// Fin du code dégueux

// Warning code dégueux NON COMMENTER écrit par moi
// Âmes sensibles s'abstenir


#define DEGMAX 3

struct DoubleUC4 {
  int a, b;

  DoubleUC4(int _a, int _b) : a(_a), b(_b) {}
};

struct Matrix {
  int size;
  std::vector<int> mat;
  std::vector<int> degCol;
  std::vector<int> degLine;
  std::vector<int> strongCol;
  std::vector<int> strongLine;
  std::vector<int> compCol;
  std::vector<int> compLine;
  std::vector<bool> switchCol;
  std::vector<bool> switchLine;
	int nb_col_leq_2;

  Matrix(const int n)
      : size(n), mat(std::vector<int>(n * n, 0)),
        degCol(std::vector<int>(n, 0)), degLine(std::vector<int>(n, 0)),
        strongCol(std::vector<int>(n, n * n)),
        strongLine(std::vector<int>(n, n * n)),
        compCol(std::vector<int>(n, n * n)),
        compLine(std::vector<int>(n, n * n)),
        switchCol(std::vector<bool>(n, false)),
        switchLine(std::vector<bool>(n, false)),
				nb_col_leq_2(size) {}

  int &at(const int a,const  int b) { return mat[a * size + b]; }
  int at(const int a,const  int b) const{ return mat[a * size + b]; }
  int at_val(const int a,const  int b) const { return value(a * size + b); }
  int value(const int i) const {
    if (switchCol[i % size] == switchLine[i / size]) {
      return mat[i];
    } else {
      return (7 * mat[i] - 3 * mat[i] * mat[i]) / 2;
    }
  }

  int &operator[](int i) { return mat[i]; }

  int get_col(const int i) const { return i % size; }
  int get_line(const int i) const { return i / size; }

  int &degreCol(const int i) { return degCol[i % size]; };
  int &degreLine(const int i) { return degLine[i / size]; };
  int degreCol(const int i) const { return degCol[i % size]; };
  int degreLine(const int i) const { return degLine[i / size]; };

  int &get_strongCol(const int i) { return strongCol[i % size]; }
  int &get_strongLine(const int i) { return strongLine[i / size]; }
  int get_strongCol(const int i) const { return strongCol[i % size]; }
  int get_strongLine(const int i) const { return strongLine[i / size]; }

  int &get_compCol(const int i) { return compCol[i % size]; }
  int &get_compLine(const int i) { return compLine[i / size]; }
  int get_compCol(const int i) const { return compCol[i % size]; }
  int get_compLine(const int i) const { return compLine[i / size]; }

  bool is_strong(const int i) const{
    return (get_strongCol(i) == i) || (get_strongLine(i) == i);
  }

  void reset_switch() {
    for (int i = 0; i < size; i++) {
      switchCol[i] = false;
      switchLine[i] = false;
    }
  }

  bool next_switch(const int c1 = -1,const int c2= -1, const int c3 = -1, const int c4 = -1) {
    for (int i = 0; i < size; i++) {
      if (i == c1 || i == c2 || i == c3 || i == c4){
        continue;
      }
      switchCol[i] = !switchCol[i];
      if (switchCol[i] == true) {
        return false;
      }
    }
    for (int i = 0; i < size; i++) {
      switchLine[i] = !switchLine[i];
      if (switchLine[i] == true) {
        return false;
      }
    }
    return true;
  }

  int next_bit_on_line(const int i) const {
    int j = i + 1;
    while (j / size == i / size) {
      if (mat[j] != 0) {
        return j;
      }
      j++;
    }
    return size * size;
  }

  bool incr(const int i) {
    mat[i]++;
    if (mat[i] > 2) {
      mat[i] = 0;
			if (degreCol(i) == 3){
				nb_col_leq_2++;
			}
      degreCol(i)--;
      degreLine(i)--;
      if (get_strongCol(i) == i) {
        get_strongCol(i) = size * size;
      }
      if (get_strongLine(i) == i) {
        get_strongLine(i) = size * size;
      }
      if (get_compCol(i) == i) {
        get_compCol(i) = size * size;
      }
      if (get_compLine(i) == i) {
        get_compLine(i) = get_strongLine(i);
      }
      return true;
    } else if (mat[i] == 1) {
      degreCol(i)++;
      degreLine(i)++;
			if (degreCol(i) == 3){
				nb_col_leq_2--;
			}
      if (get_strongCol(i) > i) {
        get_strongCol(i) = i;
      }
      if (get_strongLine(i) > i) {
        get_strongLine(i) = i;
      }
    }

    // Comp avec prev colonne
    if (get_col(i) != 0) {
      if (i - 1 < get_compCol(i - 1)) {
        zero(i);
        return true;
      } else if (i - 1 == get_compCol(i - 1)) {
        if (mat[i - 1] == mat[i]) {
          get_compCol(i - 1) = size * size;
        } else if (mat[i - 1] < mat[i]) {
          zero(i);
          return true;
        }
      }
    }

    // Comp colonne actuelle
    if (get_compCol(i) >= i && mat[i] == 0) {
      get_compCol(i) = size * size;
    }
    if (mat[i] != 0 && get_compCol(i) > i) {
      get_compCol(i) = i;
    }

    // Comp avec prev ligne
    if (get_line(i) != 0) {
      if (i - size < get_compLine(i - size)) {
        zero(i);
        return true;
      } else if (i - size == get_compLine(i - size)) {
        if (mat[i - size] == mat[i]) {
          get_compLine(i - size) = next_bit_on_line(i - size);
        } else if (mat[i - size] < mat[i]) {
          zero(i);
          return true;
        }
      }
    }

    // Comp ligne actuelle
    if (get_compLine(i) == i && mat[i] == 0) {
      get_compLine(i) = size * size;
    }
    if (get_compLine(i) > i) {
      get_compLine(i) = get_strongLine(i);
    }
    // if (mat[i] != 0 && get_compLine(i) > i) {
    //  get_compLine(i) = i;
    //}

    return false;
  }

  void zero(const int i) {
		if ((mat[i] % 3) != 0) {
			if (degreCol(i) == 3){
				nb_col_leq_2++;
			}
      degreCol(i)--;
      degreLine(i)--;
    }
    if (get_strongCol(i) == i) {
      get_strongCol(i) = size * size;
    }
    if (get_strongLine(i) == i) {
      get_strongLine(i) = size * size;
    }

    if (i == get_compCol(i)) {
      get_compCol(i) = size * size;
    }
    if (i == get_compLine(i)) {
      get_compLine(i) = get_strongLine(i);
    }
    mat[i] = 0;
  }

	void populate_boost_graph(BoostGraph & g){
		for (int i=0;i<size;i++){
			for (int j=0;j<size;j++){
				if (at(i,j)){
					boost::add_edge(i,size+j,g);
				}
			}
		}
	}

	bool as_at_least_4_deg_2(){
		int nb = 0;
		for (int i =0; i<size;i++){
			if (degCol[i] == 2) nb++;
		}
		return nb >= 4;
	}
};

struct BipGraph {
  Matrix matrix;
  int n;
  bool end = false;

  BipGraph(int _n) : matrix(_n), n(_n) {}

  BipGraph &operator++() {
    next(n * n - 1);
    return *this;
  }
  friend std::ostream &operator<<(std::ostream &os, BipGraph &G);

	bool is_planar(){
		BoostGraph g(2*n);
		matrix.populate_boost_graph(g);
	  return boyer_myrvold_planarity_test(g);
	}

	bool as_at_least_4_deg_2(){
		return matrix.as_at_least_4_deg_2();
	}
  void next(const int i) {
    // Si on atteint un indice négatif, on a dépacé le bit de poinds le plus
    // fort
    if (i < 0) {
      end = true;
      return;
    }

    if (matrix.incr(i)) { // Si on a une retenue, on avance
      next(i - 1);
    } else if (matrix.degreCol(i) > DEGMAX ||
               matrix.degreLine(i) >
                   DEGMAX || matrix.nb_col_leq_2 < 4) { // Si le degre est trop grand on propage une retenue
      matrix.zero(i);
      next(i - 1);
    } else if (matrix.is_strong(i) && matrix[i] == 1) {
      // matrix.zero(i);
      next(i);
    }

    return;
  }

  bool tec_to_UC4() const {
    for (int i = 0; i < n * n; i++) {
      if (matrix.value(i) != 2) {
        continue;
      }
      for (int j = i + 1; j < n * n; j++) {
        if (j / n == i / n || j % n == i % n || matrix.value(j) != 2) {
          continue;
        }
        if (matrix.value((j / n) * n + (i % n)) == 1 ||
            matrix.value((i / n) * n + (j % n)) == 1) {
          return false;
        }
      }
    }
    return true;
  }

  bool sign_to_UC4() {
    while (!matrix.next_switch()) {
      if (tec_to_UC4()) {
        // std::cout << *this;
        matrix.reset_switch();
        return true;
      }
    }
    matrix.reset_switch();
    return false;
  }

  bool verifyPropertyGadget(){
      for (int v1=0; v1 <n; v1++){
        if (matrix.degCol[v1] != 2){
          continue;
        }
        for (int v2=v1+1; v2 <n; v2++){
          if (matrix.degCol[v2] != 2){
            continue;
          }
          for (int v3=v2+1; v3 <n; v3++){
            if (matrix.degCol[v3] != 2){
              continue;
            }
            for (int v4=v3+1; v4 <n; v4++){
              if (matrix.degCol[v4] != 2){
                continue;
              }
              //std::cout << "Selected:" << v1 << " " << v2 << " " << v3 << " " << v4 << "\n";
              bool found = true;
              bool good = false;
              while (!matrix.next_switch(v1,v2,v3,v4) && found) {
                if (tec_to_UC4()) {
                  good = true;
                  // std::cout << *this;
                  BipGraph B(n);
                  id(B,v1,v2,v3,v4);
                  if (!B.tec_to_UC4()){
                    found = false;
                    continue;
                  }
                  if (weard_function(v1,v2) || weard_function(v2,v1)
                     || weard_function(v1,v3) || weard_function(v3,v1)
                     || weard_function(v1,v4) || weard_function(v4,v1)
                     || weard_function(v2,v3) || weard_function(v3,v2)
                     || weard_function(v2,v4) || weard_function(v4,v2)
                     || weard_function(v3,v4) || weard_function(v4,v3)
                   ){
                     found = false;
                   }
                }
              }
              matrix.reset_switch();
              if (good && found){
                return true;
              }
            }
          }
        }
      }
      return false;
  }

  bool weard_function(int c1,int c2) const {
    // Create a fake +- between two columns (- is located toward c2)
    // Then test if -> UC_4
    for (int i=0; i< n*n-1;i++){
      if (matrix.value(i) == 2 && i%n != c2){
        if (i%n == c1){
          return false;
        } else{
          if (matrix.value((i / n) * n + c2) == 1) {
            return false;
          }
        }
      }
    }
    return true;
  }

  bool id(BipGraph &B,int c1, int c2, int c3, int c4){
    int id_col = 1;
    for (int i=0; i<n;i++){
      if (i == c1 || i == c2 || i == c3 || i == c4){
        for (int j=0; j<n;j++){
          if (B.matrix.at(0,j) == 0){
            B.matrix.at(0,j) = matrix.at_val(i,j);
          }
          else if (B.matrix.at(0,j) != matrix.at_val(i,j) && matrix.at_val(i,j) != 0){
            return false;
          }
        }
      } else{
        for (int j=0; j<n;j++){
            B.matrix.at(id_col,j) = matrix.at_val(i,j);
        }
        id_col++;
      }
    }
  }

  void save(std::string filename) const {
    std::ofstream file(filename);
    file << "graph {" << '\n';
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix.at(i, j) == 1) {
          file << (char)('a' + i) << " -- " << (char)('a' + j) << ";\n";
        } else if (matrix.at(i, j) == 2) {
          file << (char)('a' + i) << " -- " << (char)('a' + j)
               << " [style=dotted];\n";
        }
      }
    }
    file << "}\n";
    file.close();
  }

  bool is_end() const { return end; }

  bool has_double_uc4(BipGraph &G, DoubleUC4 &U) {
    for (int i = 0; i < n * n; i++) {
      for (int j = 0; j < n * n; j++) {
        if (i / n == j / n || j % n == i % n) {
          continue;
        }
        int k = n * (i / n) + (j % n);
        int l = n * (j / n) + (i % n);
        if ((G.matrix[i] == G.matrix[j]) != (G.matrix[k] == G.matrix[l]) &&
            0 != G.matrix[i] && 0 != G.matrix[j] && 0 != G.matrix[k] &&
            0 != G.matrix[l]) { // UC4
          for (int p = 0; p < n * n; p++) {
            if (i / n == p / n || p % n == i % n) {
              continue;
            }
            if (j == p) {
              continue;
            }
            int q = n * (i / n) + (p % n);
            int r = n * (p / n) + (i % n);
            if ((G.matrix[i] == G.matrix[p]) != (G.matrix[q] == G.matrix[r]) &&
                0 != G.matrix[p] && 0 != G.matrix[q] &&
                0 != G.matrix[r]) { // UC4
              U = DoubleUC4(j, p);
              return true;
            }
          }
        }
      }
    }
    return false;
  }

  bool uc4_fold(BipGraph &G, BipGraph &G2, DoubleUC4 UC4) {
    int col1 = UC4.a % n;
    int col2 = UC4.b % n;
    int ligne1 = UC4.a / n;
    int ligne2 = UC4.b / n;
    // std::cout << col1 << " " << col2 << " " << ligne1 << " " << ligne2 << " "
    //          << UC4.a << " " << UC4.b << std::endl;
    bool eq1 = true;
    bool op1 = true;
    for (int i = 0; i < n; i++) {
      if (G.matrix.at(i, col1) > 0 && G.matrix.at(i, col2) > 0) {
        if (G.matrix.at(i, col1) == G.matrix.at(i, col2)) {
          op1 = false;
        } else {
          eq1 = false;
        }
      }
    }
    if (!eq1 && !op1) {
      return false;
    }

    bool eq2 = true;
    bool op2 = true;
    for (int i = 0; i < n; i++) {
      if (G.matrix.at(ligne1, i) > 0 && 0 < G.matrix.at(ligne2, i)) {
        if (G.matrix.at(ligne1, i) == G.matrix.at(ligne2, i)) {
          op2 = false;
        } else {
          eq2 = false;
        }
      }
    }
    if (!eq2 && !op2) {
      return false;
    }

    int current_col = 0;
    int current_line = 0;
    for (int i = 0; i < n; i++) {
      current_line = 0;
      if (i == col2) {
        for (int j = 0; j < n; j++) {
          if (j == ligne2) {
            G2.matrix.at(current_line, current_col) =
                (eq1 == eq2 ? G.matrix.at(j, i)
                            : (7 * G.matrix.at(j, i) -
                               3 * G.matrix.at(j, i) * G.matrix.at(j, i)) /
                                  2);
            if (G2.matrix.at(current_line, current_col) == 0) {
              G2.matrix.at(current_line, current_col) =
                  (eq2 ? G.matrix.at(j, col1)
                       : (7 * G.matrix.at(j, col1) -
                          3 * G.matrix.at(j, col1) * G.matrix.at(j, col1)) /
                             2);
            }
            if (G2.matrix.at(current_line, current_col) == 0) {
              G2.matrix.at(current_line, current_col) =
                  (eq1 ? G.matrix.at(ligne1, i)
                       : (7 * G.matrix.at(ligne1, i) -
                          3 * G.matrix.at(ligne1, i) * G.matrix.at(ligne1, i)) /
                             2);
            }
            if (G2.matrix.at(current_line, current_col) == 0) {
              G2.matrix.at(current_line, current_col) =
                  G.matrix.at(ligne1, col1);
            }
          } else if (j == ligne1) {
            continue;
          } else {
            G2.matrix.at(current_line, current_col) =
                (eq1 ? G.matrix.at(j, i)
                     : (7 * G.matrix.at(j, i) -
                        3 * G.matrix.at(j, i) * G.matrix.at(j, i)) /
                           2);
            if (G2.matrix.at(current_line, current_col) == 0) {
              G2.matrix.at(current_line, current_col) = G.matrix.at(j, col1);
            }
          }
          current_line++;
        }
      } else if (i == col1) {
        continue;
      } else {
        for (int j = 0; j < n; j++) {
          if (j == ligne2) {
            G2.matrix.at(current_line, current_col) =
                (eq2 ? G.matrix.at(j, i)
                     : (7 * G.matrix.at(j, i) -
                        3 * G.matrix.at(j, i) * G.matrix.at(j, i)) /
                           2);
            if (G2.matrix.at(current_line, current_col) == 0) {
              G2.matrix.at(current_line, current_col) = G.matrix.at(ligne1, i);
            }
          } else if (j == ligne1) {
            continue;
          } else {
            G2.matrix.at(current_line, current_col) = G.matrix.at(j, i);
          }

          current_line++;
        }
      }
      current_col++;
    }
    // std::cout << G2 << "\n";
    // std::cout << "bah" << std::endl;

    // std::cout << eq1 << op1 << eq2 << op2 << "\n";
    return true;
  }

  bool tentative_poly_test_helper(BipGraph G) {
    // std::cout << "booo" << std::endl;
    DoubleUC4 UC4(n * n, n * n);
    BipGraph G2(n);
    // G2.matrix.mat = std::vector<int>(n * n, -1);
    if (has_double_uc4(G, UC4)) {
      // std::cout << "booo" << std::endl;
      if (uc4_fold(G, G2, UC4)) {
        // std::cout << "booo2" << std::endl;
        // std::cout << G2 << "\n";
        return tentative_poly_test_helper(G2);
      } else {
        return false;
      }
    }
    return true;
  }
  bool tentative_poly_test() { return tentative_poly_test_helper(*this); }
};

std::ostream &operator<<(std::ostream &os, BipGraph &G) {
  for (int i = 0; i < G.n; i++) {
    for (int j = 0; j < G.n; j++) {
      os << G.matrix.at_val(i, j) << " ";
    }
    os << " | " << G.matrix.degLine[i] << " | " << G.matrix.strongLine[i]
       << " | " << G.matrix.compLine[i];
    os << "\n";
  }
  for (int j = 0; j < G.n; j++) {
    os << "_"
       << " ";
  }
  os << "\n";
  for (int j = 0; j < G.n; j++) {
    os << G.matrix.degCol[j] << " ";
  }
  os << '\n';
  for (int j = 0; j < G.n; j++) {
    os << "_"
       << " ";
  }
  os << "\n";
  for (int j = 0; j < G.n; j++) {
    os << G.matrix.strongCol[j] << " ";
  }
  os << '\n';
  for (int j = 0; j < G.n; j++) {
    os << "_"
       << " ";
  }
  os << "\n";
  for (int j = 0; j < G.n; j++) {
    os << G.matrix.compCol[j] << " ";
  }
  os << '\n';
  return os;
}
#endif
