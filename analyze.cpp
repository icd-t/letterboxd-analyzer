#include "./include/csv.hpp"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main() {

  std::ofstream f01("./output/volumeByYear.csv");
  std::string currentRow{};

  // Load in csv fie
  cout << "Loading ./data/lbp.csv ..." << endl;
  csv::DataFrame<int> df("./data/lbp.csv",
                         csv::DataFrameOptions().set_key_column("index"));
  cout << "Finished loading " << df.n_rows() << " rows..." << endl;

  // Count volume by year
  cout << "Counting years..." << endl;
  int years[200]{};
  for (int i = 1; i < df.n_rows(); i++) {
    years[(df[i]["year"].get<int>() - 1826)] += 1;
  }
  cout << "Outputting years..." << endl;
  for (int i = 0; i < std::size(years); i++) {
    currentRow = (std::to_string((i + 1826)) + "," + std::to_string(years[i]));
    cout << currentRow << endl;
    f01 << currentRow << "\n";
  }

  f01.close();
  return 0;
}
