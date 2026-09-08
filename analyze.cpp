#include "./include/csv.hpp"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main() {

  ofstream f01("./output/volumeByYear.csv");
  string currentRow{};

  // Load in csv fie
  cout << "Loading ./data/lbp.csv..." << endl;
  csv::DataFrame<int> df("./data/lbp.csv",
                         csv::DataFrameOptions().set_key_column("index"));
  cout << "Finished loading " << df.n_rows() << " rows..." << endl;

  // Count volume by year
  cout << "Counting years..." << endl;
  // This array will store data for all years
  // It is indexed such that 0 is 1826 and 199 is 2025
  // Column 0 is just a count of movies for that year
  // Column 1 is a count of non-documentaries for that year
  int years[200][2]{};
  // Stores average rating per year
  double yearsRatings[200]{};
  // Loop over all rows
  for (int i = 1; i < df.n_rows(); i++) {
    // Increment count for whatever year the current row is
    years[(df[i]["year"].get<int>() - 1826)][0] += 1;
    // Increment count for excluding-docs years if neither genre of current row
    // is documentary
    if ((df[i]["genre1"].get<string>().compare("Documentary") != 0) &&
        (df[i]["genre2"].get<string>().compare("Documentary") != 0)) {
      years[df[i]["year"].get<int>() - 1826][1] += 1;
    }
    // Increment average rating
    yearsRatings[(df[i]["year"].get<int>() - 1826)] +=
        df[i]["rating"].get<double>();
  }
  cout << "Outputting years..." << endl;
  f01 << "year,count,count-exdocs,average-rating\n";
  for (int i = 0; i < size(years); i++) {
    // Average the ratings column
    if (yearsRatings[i] != 0) {
      yearsRatings[i] = (yearsRatings[i] / years[i][0]);
    }
    // Output
    currentRow = (to_string((i + 1826)) + "," + to_string(years[i][0]) + "," +
                  to_string(years[i][1]) + "," + to_string(yearsRatings[i]));
    // cout << currentRow << endl;
    f01 << currentRow << "\n";
  }
  cout << "Finished outputting years..." << endl;

  f01.close();
  return 0;
}
