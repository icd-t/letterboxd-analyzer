#include "./include/csv.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
using namespace std;

// Check dataframe row for a specific genre. returns if true.
bool checkGenre(csv::DataFrame<int> &dataframe, int targetRow, string genre) {
  if (dataframe[targetRow]["genre1"].get<string>().compare(genre) == 0 ||
      dataframe[targetRow]["genre2"].get<string>().compare(genre) == 0) {
    return true;
  } else {
    return false;
  }
}

int main() {

  // Load in files
  ofstream f01("./output/by-year.csv");
  ofstream f02("./output/by-genre.csv");
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
  // First row is not weighted
  // Second row is weighted by popularity score
  double yearsRatings[200][2]{};
  // Popularity scores per year
  double yearsPopularity[200]{};
  // Loop over all rows
  for (int i = 1; i < df.n_rows(); i++) {
    // Increment count for whatever year the current row is
    years[(df[i]["year"].get<int>() - 1826)][0] += 1;
    // Increment count for excluding-docs years if neither genre of current row
    // is documentary
    if (!checkGenre(df, i, "Documentary")) {
      years[df[i]["year"].get<int>() - 1826][1] += 1;
    }
    // Increment average rating
    yearsRatings[(df[i]["year"].get<int>() - 1826)][0] +=
        df[i]["rating"].get<double>();
    // Increment average rating popularity weighted
    yearsRatings[(df[i]["year"].get<int>() - 1826)][1] +=
        (df[i]["rating"].get<double>() * df[i]["popularity"].get<double>());
    // Increment populatity
    yearsPopularity[(df[i]["year"].get<int>() - 1826)] +=
        df[i]["popularity"].get<double>();
  }
  cout << "Outputting years..." << endl;
  f01 << "year,count,count-exdocs,popularity,rating,rating-pop-weighted\n";
  for (int i = 0; i < size(years); i++) {
    // Average the ratings column
    if (yearsRatings[i][0] != 0) {
      yearsRatings[i][0] = (yearsRatings[i][0] / years[i][0]);
      yearsRatings[i][1] = (yearsRatings[i][1] / yearsPopularity[i]);
    } else {
      yearsRatings[i][0] = 0;
      yearsRatings[i][1] = 0;
    }
    // Output
    currentRow =
        (to_string((i + 1826)) + "," + to_string(years[i][0]) + "," +
         to_string(years[i][1]) + "," + to_string(yearsPopularity[i]) + "," +
         to_string(yearsRatings[i][0]) + "," + to_string(yearsRatings[i][1]));
    // cout << currentRow << endl;
    f01 << currentRow << "\n";
  }
  cout << "Finished outputting years..." << endl;

  // Count by genre
  cout << "Counting genres... " << endl;
  array<tuple<string, int>, 19> genres{{
      {"Action", 0},  {"Adventure", 0},   {"Animation", 0}, {"Comedy", 0},
      {"Crime", 0},   {"Documentary", 0}, {"Drama", 0},     {"Family", 0},
      {"Fantasy", 0}, {"History", 0},     {"Horror", 0},    {"Music", 0},
      {"Mystery", 0}, {"Romance", 0},     {"Thriller", 0},  {"TV Movie", 0},
      {"War", 0},     {"Western", 0},     {"Other", 0},
  }};
  for (int i = 1; i < df.n_rows(); i++) {
    for (int j = 0; j < size(genres); j++) {
      if (checkGenre(df, i, get<0>(genres[j]))) {
        get<1>(genres[j])++;
      }
    }
  }

  // By genre counts to file
  cout << "Finished counting genres...\nOutputing by genres..." << endl;
  f02 << "genre,count\n";
  for (int i = 0; i < size(genres); i++) {
    f02 << get<0>(genres[i]) << "," << get<1>(genres[i]) << "\n";
  }
  cout << "Finished outputting genres..." << endl;

  // Close files and exit
  f01.close();
  f02.close();
  return 0;
}
