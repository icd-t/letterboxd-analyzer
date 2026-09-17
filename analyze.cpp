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
  ofstream f03("./output/by-rating.csv");
  string currentRow{};

  // Load in csv fie
  cout << "Loading ./data/lbp.csv..." << endl;
  csv::DataFrame<int> df("./data/lbp.csv",
                         csv::DataFrameOptions().set_key_column("index"));
  cout << "Finished loading " << df.n_rows() << " rows..." << endl;

  // Store counts of genres
  array<tuple<string, int>, 19> genres{{
      {"Action", 0},  {"Adventure", 0},   {"Animation", 0}, {"Comedy", 0},
      {"Crime", 0},   {"Documentary", 0}, {"Drama", 0},     {"Family", 0},
      {"Fantasy", 0}, {"History", 0},     {"Horror", 0},    {"Music", 0},
      {"Mystery", 0}, {"Romance", 0},     {"Thriller", 0},  {"TV Movie", 0},
      {"War", 0},     {"Western", 0},     {"Other", 0},
  }};

  // Count volume by year
  cout << "Counting years..." << endl;
  // This array of tuples holds all the data that will be outputted to the csv
  // file. Variables from left to right: count of films in that year, count of
  // non-documentaries in that year, average rating, average rating (weighted by
  // popularity), total popularity score per year, an array storing counts for
  // each individual genre, number of films that year that have an average
  // rating above 4, percentage over 4 stars
  array<tuple<int, int, double, double, double, array<int, 19>, int, double>,
        200>
      byYear{};
  // Loop over all rows
  for (int i = 1; i < df.n_rows(); i++) {
    // Increment count for whatever year the current row is
    get<0>(byYear[df[i]["year"].get<int>() - 1826]) += 1;
    // Increment count for excluding-docs years if neither genre of current row
    // is documentary
    if (!checkGenre(df, i, "Documentary")) {
      get<1>(byYear[df[i]["year"].get<int>() - 1826]) += 1;
    }
    // Increment average rating
    get<2>(byYear[(df[i]["year"].get<int>() - 1826)]) +=
        df[i]["rating"].get<double>();
    // Increment average rating popularity weighted
    get<3>(byYear[(df[i]["year"].get<int>() - 1826)]) +=
        (df[i]["rating"].get<double>() * df[i]["popularity"].get<double>());
    // Increment populatity
    get<4>(byYear[(df[i]["year"].get<int>() - 1826)]) +=
        df[i]["popularity"].get<double>();
    // Increment counts for each genre
    for (int j = 0; j < size(get<5>(byYear[0])); j++) {
      if (checkGenre(df, i, get<0>(genres[j]))) {
        get<5>(byYear[(df[i]["year"].get<int>() - 1826)])[j] += 1;
      }
    }
    // Increment count if above 4.0 stars
    if (df[i]["rating"].get<double>() > 4.0) {
      get<6>(byYear[(df[i]["year"].get<int>() - 1826)]) += 1;
      get<7>(byYear[(df[i]["year"].get<int>() - 1826)]) += 1;
    }
  }
  cout << "Outputting years..." << endl;
  // Write header of csv file
  f01 << "year,count,count-exdocs,popularity,rating,rating-pop-weighted";
  for (int i = 0; i < size(genres); i++) {
    f01 << "," << get<0>(genres[i]);
  }
  f01 << ",count-over-4.0\n";
  // Write each row of csv file
  for (int i = 0; i < size(byYear); i++) {
    // Average the ratings column
    if (get<2>(byYear[i]) != 0) {
      get<2>(byYear[i]) = get<2>(byYear[i]) / get<0>(byYear[i]);
      get<3>(byYear[i]) = get<3>(byYear[i]) / get<4>(byYear[i]);
      get<7>(byYear[i]) = get<7>(byYear[i]) / get<0>(byYear[i]);
    } else {
      get<2>(byYear[i]) = 0;
      get<3>(byYear[i]) = 0;
      get<7>(byYear[i]) = 0;
    }
    // Output
    currentRow =
        (to_string((i + 1826)) + "," + to_string(get<0>(byYear[i])) + "," +
         to_string(get<1>(byYear[i])) + "," + to_string(get<2>(byYear[i])) +
         "," + to_string(get<3>(byYear[i])) + "," +
         to_string(get<4>(byYear[i])));
    for (int j = 0; j < size(get<5>(byYear[0])); j++) {
      currentRow += "," + to_string(get<5>(byYear[i])[j]);
    }
    currentRow +=
        "," + to_string(get<6>(byYear[i])) + "," + to_string(get<7>(byYear[i]));
    // cout << currentRow << endl;
    f01 << currentRow << "\n";
  }
  cout << "Finished outputting years..." << endl;

  // Count by genre
  cout << "Counting genres... " << endl;
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

  // Count by rating
  cout << "Counting by rating..." << endl;
  // Column 1 is a rating catagory (starts at 1 and increments by 0.05), column
  // 2 is the ammount of films that fall within that rating bucket
  array<tuple<double, int>, 90> ratings{};
  for (int i = 0; i < size(ratings); i++) {
    get<0>(ratings[i]) = 0.5 + (0.05 * i);
  }
  for (int i = 1; i < df.n_rows(); i++) {
    get<1>(ratings[static_cast<int>((df[i]["rating"].get<float>() - 0.5) /
                                    0.05)]) += 1;
  }
  cout << "Finished counting by rating..." << endl;

  // Output by rating
  cout << "Outputting by rating..." << endl;
  f03 << "rating,count\n";
  for (int i = 0; i < size(ratings); i++) {
    f03 << to_string(get<0>(ratings[i])) + "," + to_string(get<1>(ratings[i])) +
               "\n";
  }
  cout << "Finished outputting by rating..." << endl;

  // Close files and exit
  f01.close();
  f02.close();
  f03.close();
  cout << "Done!\n";
  return 0;
}
