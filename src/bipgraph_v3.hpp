#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <fstream>
#include "stream.hpp"
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
using uchar = unsigned char;


struct DoubleUC4 {
  uchar a, b;

  DoubleUC4(uchar _a, uchar _b) : a(_a), b(_b) {}
};

struct Matrix {
  uchar size;
  std::vector<uchar> mat;
  std::vector<uchar> degCol;
  std::vector<uchar> degLine;
  std::vector<uchar> strongCol;
  std::vector<uchar> strongLine;
  std::vector<uchar> compCol;
  std::vector<uchar> compLine;
  std::vector<bool> switchCol;
  std::vector<bool> switchLine;
	uchar nb_col_leq_2;

  Matrix(const uchar n)
      : size(n), mat(std::vector<uchar>(n * n, 0)),
        degCol(std::vector<uchar>(n, 0)), degLine(std::vector<uchar>(n, 0)),
        strongCol(std::vector<uchar>(n, n * n)),
        strongLine(std::vector<uchar>(n, n * n)),
        compCol(std::vector<uchar>(n, n * n)),
        compLine(std::vector<uchar>(n, n * n)),
        switchCol(std::vector<bool>(n, false)),
        switchLine(std::vector<bool>(n, false)),
				nb_col_leq_2(size) {}

  uchar &at(const uchar a,const  uchar b) { return mat[a * size + b]; }
  uchar at(const uchar a,const  uchar b) const{ return mat[a * size + b]; }
  uchar at_val(const uchar a,const  uchar b) const { return value(a * size + b); }
  uchar value(const uchar i) const {
    if (switchCol[i % size] == switchLine[i / size]) {
      return mat[i];
    } else {
      return (7 * mat[i] - 3 * mat[i] * mat[i]) / 2;
    }
  }

  uchar &operator[](uchar i) { return mat[i]; }

  uchar get_col(const uchar i) const { return i % size; }
  uchar get_line(const uchar i) const { return i / size; }

  uchar &degreCol(const uchar i) { return degCol[i % size]; };
  uchar &degreLine(const uchar i) { return degLine[i / size]; };
  uchar degreCol(const uchar i) const { return degCol[i % size]; };
  uchar degreLine(const uchar i) const { return degLine[i / size]; };

  uchar &get_strongCol(const uchar i) { return strongCol[i % size]; }
  uchar &get_strongLine(const uchar i) { return strongLine[i / size]; }
  uchar get_strongCol(const uchar i) const { return strongCol[i % size]; }
  uchar get_strongLine(const uchar i) const { return strongLine[i / size]; }

  uchar &get_compCol(const uchar i) { return compCol[i % size]; }
  uchar &get_compLine(const uchar i) { return compLine[i / size]; }
  uchar get_compCol(const uchar i) const { return compCol[i % size]; }
  uchar get_compLine(const uchar i) const { return compLine[i / size]; }

  bool is_strong(const uchar i) const{
    return (get_strongCol(i) == i) || (get_strongLine(i) == i);
  }

  void reset_switch() {
    for (uchar i = 0; i < size; i++) {
      switchCol[i] = false;
      switchLine[i] = false;
    }
  }

  bool next_switch(const uchar c1 = -1,const uchar c2= -1, const uchar c3 = -1, const uchar c4 = -1) {
    for (uchar i = 0; i < size; i++) {
      if (i == c1 || i == c2 || i == c3 || i == c4){
        continue;
      }
      switchCol[i] = !switchCol[i];
      if (switchCol[i] == true) {
        return false;
      }
    }
    for (uchar i = 0; i < size; i++) {
      switchLine[i] = !switchLine[i];
      if (switchLine[i] == true) {
        return false;
      }
    }
    return true;
  }

  uchar next_bit_on_line(const uchar i) const {
    uchar j = i + 1;
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

  void zero(const uchar i) {
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
		for (uchar i=0;i<size;i++){
			for (uchar j=0;j<size;j++){
				if (at(i,j)){
					boost::add_edge(i,size+j,g);
				}
			}
		}
	}

	bool as_at_least_4_deg_2(){
		uchar nb = 0;
		for (uchar i =0; i<size;i++){
			if (degCol[i] == 2) nb++;
		}
		return nb >= 4;
	}
};

struct BipGraph {
  Matrix matrix;
  uchar n;
  bool end = false;

  BipGraph(uchar _n) : matrix(_n), n(_n) {}

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
    for (uchar i = 0; i < n * n; i++) {
      if (matrix.value(i) != 2) {
        continue;
      }
      for (uchar j = i + 1; j < n * n; j++) {
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
        // ms << *this;
        matrix.reset_switch();
        return true;
      }
    }
    matrix.reset_switch();
    return false;
  }

  bool verifyPropertyGadget(){
      for (uchar v1=0; v1 <n; v1++){
        if (matrix.degCol[v1] != 2){
          continue;
        }
        for (uchar v2=v1+1; v2 <n; v2++){
          if (matrix.degCol[v2] != 2){
            continue;
          }
          for (uchar v3=v2+1; v3 <n; v3++){
            if (matrix.degCol[v3] != 2){
              continue;
            }
            for (uchar v4=v3+1; v4 <n; v4++){
              if (matrix.degCol[v4] != 2){
                continue;
              }
              //ms << "Selected:" << v1 << " " << v2 << " " << v3 << " " << v4 << "\n";
              bool found = true;
              bool good = false;
              while (!matrix.next_switch(v1,v2,v3,v4) && found) {
                if (tec_to_UC4()) {
                  good = true;
                  // ms << *this;
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
ms << "Very good" <<(int) v1 << " " << (int)v2 << " " << (int)v3 << " " <<(int) v4 << '\n';
                return true;
              }
            }
          }
        }
      }
      return false;
  }

  bool weard_function(uchar c1,uchar c2) const {
    // Create a fake +- between two columns (- is located toward c2)
    // Then test if -> UC_4
    for (uchar i=0; i< n*n-1;i++){
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

  bool id(BipGraph &B,uchar c1, uchar c2, uchar c3, uchar c4){
    uchar id_col = 1;
    for (uchar i=0; i<n;i++){
      if (i == c1 || i == c2 || i == c3 || i == c4){
        for (uchar j=0; j<n;j++){
          if (B.matrix.at(0,j) == 0){
            B.matrix.at(0,j) = matrix.at_val(i,j);
          }
          else if (B.matrix.at(0,j) != matrix.at_val(i,j) && matrix.at_val(i,j) != 0){
            return false;
          }
        }
      } else{
        for (uchar j=0; j<n;j++){
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
    for (uchar i = 0; i < n * n; i++) {
      for (uchar j = 0; j < n * n; j++) {
        if (i / n == j / n || j % n == i % n) {
          continue;
        }
        uchar k = n * (i / n) + (j % n);
        uchar l = n * (j / n) + (i % n);
        if ((G.matrix[i] == G.matrix[j]) != (G.matrix[k] == G.matrix[l]) &&
            0 != G.matrix[i] && 0 != G.matrix[j] && 0 != G.matrix[k] &&
            0 != G.matrix[l]) { // UC4
          for (uchar p = 0; p < n * n; p++) {
            if (i / n == p / n || p % n == i % n) {
              continue;
            }
            if (j == p) {
              continue;
            }
            uchar q = n * (i / n) + (p % n);
            uchar r = n * (p / n) + (i % n);
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
    uchar col1 = UC4.a % n;
    uchar col2 = UC4.b % n;
    uchar ligne1 = UC4.a / n;
    uchar ligne2 = UC4.b / n;
    // ms << col1 << " " << col2 << " " << ligne1 << " " << ligne2 << " "
    //          << UC4.a << " " << UC4.b << '\n';
    bool eq1 = true;
    bool op1 = true;
    for (uchar i = 0; i < n; i++) {
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
    for (uchar i = 0; i < n; i++) {
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

    uchar current_col = 0;
    uchar current_line = 0;
    for (uchar i = 0; i < n; i++) {
      current_line = 0;
      if (i == col2) {
        for (uchar j = 0; j < n; j++) {
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
        for (uchar j = 0; j < n; j++) {
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
    // ms << G2 << "\n";
    // ms << "bah" << '\n';

    // ms << eq1 << op1 << eq2 << op2 << "\n";
    return true;
  }

  bool tentative_poly_test_helper(BipGraph G) {
    // ms << "booo" << '\n';
    DoubleUC4 UC4(n * n, n * n);
    BipGraph G2(n);
    // G2.matrix.mat = std::vector<int>(n * n, -1);
    if (has_double_uc4(G, UC4)) {
      // ms << "booo" << '\n';
      if (uc4_fold(G, G2, UC4)) {
        // ms << "booo2" << '\n';
        // ms << G2 << "\n";
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
      os << (int)G.matrix.at_val(i, j) << " ";
    }
    os << " | " << (int)G.matrix.degLine[i] << " | " << (int)G.matrix.strongLine[i]
       << " | " << (int)G.matrix.compLine[i];
    os << "\n";
  }
  for (int j = 0; j < G.n; j++) {
    os << "_"
       << " ";
  }
  os << "\n";
  for (int j = 0; j < G.n; j++) {
    os << (int)G.matrix.degCol[j] << " ";
  }
  os << '\n';
  for (int j = 0; j < G.n; j++) {
    os << "_"
       << " ";
  }
  os << "\n";
  for (int j = 0; j < G.n; j++) {
    os << (int)G.matrix.strongCol[j] << " ";
  }
  os << '\n';
  for (int j = 0; j < G.n; j++) {
    os << "_"
       << " ";
  }
  os << "\n";
  for (int j = 0; j < G.n; j++) {
    os << (int)G.matrix.compCol[j] << " ";
  }
  os << '\n';
  return os;
}
#endif
