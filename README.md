Simple project that cleans and analyzes a Letterboxd dataset I found from Hugging Face.

# Prereqs.

Download the [Dataset](https://huggingface.co/datasets/pkchwy/letterboxd-all-movie-data) from Hugging Face, and rename it as `lb.jsonl`. Then place it inside of a directory called `data/` inside of the project's root directory.

# Compilation

Run `g++ main.cpp -o lb-analyzer && ./lb-analyzer` to compile and run the program. This will output a cleaned and formatted CSV file to `data/`.
