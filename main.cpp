// Clean the data of a database I found of 800k movies on Letterboxd
// Written by icd-t

#include "./include/json.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
using json = nlohmann::json;

int main() {
  std::ifstream f("./data/lb.jsonl");  // Input file
  std::ofstream lbp("./data/lbp.csv"); // Output file
  std::string line;                    // Input line
  std::string parsedLine{""};          // Output line

  int rows{837217};
  int usableRows{0};
  float currentPercentage{0};

  float avgRating{0.0};
  float rating{0.0};
  float popularity{0.0};
  int year{0};
  std::string name{""};
  std::string director1{""};
  std::string director2{""};
  std::string genre1{""};
  std::string genre2{""};
  std::string lbURL{""};
  std::string posterURL{""};

  // Loop over all films in dataset
  for (int i = 0; i < rows; i++) {
    std::getline(f, line);
    json data = json::parse(line);

    // Only export data if the film has a rating associated with it
    if (data["rating"] != nullptr) {
      usableRows++;
      // Get film title
      name = data["title"].get<std::string>();
      std::replace(name.begin(), name.end(), ',', ' ');
      // Get film director
      if (data["directors"][0] != nullptr) {
        director1 = data["directors"][0].get<std::string>();
      } else {
        director1 = "-";
      }
      if (data["directors"][1] != nullptr) {
        director2 = data["directors"][1].get<std::string>();
      } else {
        director2 = "-";
      }
      // Get film genres
      if (data["genres"][0] != nullptr) {
        genre1 = data["genres"][0].get<std::string>();
      } else {
        genre1 = "-";
      }
      if (data["genres"][1] != nullptr) {
        genre2 = data["genres"][1].get<std::string>();
      } else {
        genre2 = "-";
      }
      // Get film rating
      rating = std::stof(data["rating"].get<std::string>().substr(0, 4));
      // Get film release year
      year = std::stoi(data["year"].get<std::string>());
      // Calculate film popularity
      if (data["reviews"] != nullptr) {

        for (int j = 0; j < data["reviews"].size(); j++) {
          popularity +=
              std::stof(data["reviews"][j]["likes"].get<std::string>());
        }
        popularity = popularity / data["reviews"].size();
      } else {
        popularity = 0.0;
      }
      // Get film URL
      lbURL = data["url"].get<std::string>();
      // Get poster URL
      posterURL = data["poster_url"].get<std::string>();

      // Export current film data to csv file
      parsedLine +=
          (std::to_string(usableRows) + "," + name + "," + director1 + "," +
           director2 + "," + genre1 + "," + genre2 + "," +
           std::to_string(year) + "," + std::to_string(rating) + "," +
           std::to_string(popularity) + "," + lbURL + "," + posterURL);
      lbp << parsedLine << "\n";

      avgRating += rating;
    }
    parsedLine = "";

    // Status update
    currentPercentage = (float(i) / rows) * 100;
    if ((i % 20000) == 0) {
      std::cout << "Film " << i << " of " << rows << " (" << currentPercentage
                << "%)" << std::endl;
    }
  }
  avgRating = avgRating / usableRows;
  std::cout << "Collected data from " << usableRows
            << " unique films with an average rating of " << avgRating << "."
            << std::endl;

  lbp.close();
  f.close();
  return 0;
}
