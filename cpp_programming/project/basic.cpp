#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// Bi-grams
#define n 2

string concatenate_words(string words[])
{
    string key;
    for (int i = 0; i < n; i++)
    {
        if (i > 0)
            key += " ";
        key += words[i];
    }
    return key;
}

vector<string> tokenize(string text)
{
    vector<string> words;
    stringstream ss;

    for (char c : text)
    {
        if (isalnum(c))
        {
            ss << (char)tolower(c);
        }
        else if (c == ' ' || c == '\t' || c == '\n')
        {
            ss << ' ';
        }
    }

    string word;
    while (ss >> word)
    {
        words.push_back(word);
    }

    return words;
}

class NGramModel
{
private:
    // Maps bigram context to possible next words and their counts
    unordered_map<string, unordered_map<string, int>> ngram_counts;

    // Maps bigram context to total count (for probability calculation)
    unordered_map<string, int> context_counts;

    mt19937 rng;

public:
    NGramModel(int n_size = 3) : rng(random_device{}()) {}

    void train(string text)
    {
        auto words = tokenize(text);

        if (words.size() < n)
        {
            cout << "Warning: Text too short for bigram\n";
            return;
        }

        // Count bigram + next word occurrences
        for (int i = 0; i <= words.size() - n; i++)
        {
            string context[n] = {words[i], words[i + 1]};
            string context_key = concatenate_words(context);
            string next_word = words[i + n];

            ngram_counts[context_key][next_word]++;
            context_counts[context_key]++;
        }

        cout << "Trained on " << words.size() << " words\n";
        cout << "Found " << context_counts.size() << " unique bigrams\n";
    }

    // Get probability distribution for next word given context
    vector<pair<string, double>> get_next_word_probabilities(string context[])
    {

        vector<pair<string, double>> probabilities;
        string context_key = concatenate_words(context);

        if (ngram_counts.find(context_key) == ngram_counts.end())
        {
            cout << "Context not found in training data\n";
            return probabilities;
        }

        int total_count = context_counts[context_key];

        for (const auto &pair : ngram_counts[context_key])
        {
            string word = pair.first;
            int count = pair.second;
            double probability = (double)count / total_count;
            probabilities.push_back({word, probability});
        }

        sort(probabilities.begin(), probabilities.end(),
             [](const auto &a, const auto &b)
             { return a.second > b.second; });

        return probabilities;
    }

    // Predict single most likely next word
    string predict_next_word(string context[])
    {
        auto probabilities = get_next_word_probabilities(context);

        if (probabilities.empty())
        {
            return "Hah!";
        }

        return probabilities[0].first;
    }

    // Sample next word based on probability distribution
    string sample_next_word(string context[])
    {
        auto probabilities = get_next_word_probabilities(context);

        if (probabilities.empty())
        {
            return "Hah!";
        }

        // Create cumulative distribution
        vector<double> cumulative_probs;
        double sum = 0.0;
        for (const auto &pair : probabilities)
        {
            sum += pair.second;
            cumulative_probs.push_back(sum);
        }

        // Sample from distribution
        uniform_real_distribution<double> dist(0.0, 1.0);
        double random_val = dist(rng);

        for (int i = 0; i < cumulative_probs.size(); i++)
        {
            if (random_val <= cumulative_probs[i])
            {
                return probabilities[i].first;
            }
        }

        return probabilities[0].first; // Fallback
    }

    // Generate text starting with given context
    string generate_text(const vector<string> &initial_context,
                         int max_words = 32)
    {
        string current_context[] = initial_context;
        string result = concatenate_words(current_context);

        for (int i = 0; i < max_words; i++)
        {
            string next_word = sample_next_word(current_context);

            if (next_word == "Hah!")
            {
                break;
            }

            result += " " + next_word;

            current_context.erase(current_context.begin());
            current_context.push_back(next_word);
        }

        return result;
    }

    // Print model statistics
    void print_stats()
    {
        cout << "\n=== Model Statistics ===\n";
        cout << "N-gram size: " << n << "\n";
        cout << "Unique contexts: " << context_counts.size() << "\n";

        int total_ngrams = 0;
        for (const auto &pair : context_counts)
        {
            total_ngrams += pair.second;
        }
        cout << "Total n-grams: " << total_ngrams << "\n";

        // Find most common contexts
        vector<pair<string, int>> contexts(context_counts.begin(),
                                           context_counts.end());
        sort(contexts.begin(), contexts.end(),
             [](const auto &a, const auto &b)
             { return a.second > b.second; });

        cout << "\nMost common contexts:\n";
        for (int i = 0; i < min(5, (int)contexts.size()); i++)
        {
            cout << "  \"" << contexts[i].first << "\": " << contexts[i].second
                 << " times\n";
        }
    }
};

// Function to load text from file
string load_text_file(string filename)
{
    string content;
    ifstream file(filename);
    if (file.is_open())
    {
        while (getline(file, content))
        {
            cout << content << '\n';
        }
        file.close();
    }
    else
        cout << "Error: Could not open file " << filename << "\n";
    return content;
}

int main()
{
    cout << "=== N-Gram Next Word Predictor ===\n";

    // Create a trigram model (uses 2 words context to predict 3rd word)
    NGramModel model(3);

    string training_text;
    training_text = load_text_file("corpus/train-text.txt");

    if (training_text.empty())
    {
        cerr << "No training data available!\n";
        return 1;
    }

    // Train the model
    cout << "Training model...\n";
    model.train(training_text);
    model.print_stats();

    // Interactive prediction loop
    cout << "\n=== Interactive Prediction ===\n";
    cout << "Enter two words to predict the next word (or 'quit' to exit):\n";

    string input;
    while (true)
    {
        cout << "\nEnter context (2 words): ";
        getline(cin, input);

        if (input == "quit" || input == "q")
        {
            break;
        }

        // Parse input
        stringstream ss(input);
        vector<string> context;
        string word;

        while (ss >> word)
        {
            // Convert to lowercase
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            context.push_back(word);
        }

        if (context.size() != 2)
        {
            cout << "Please enter exactly 2 words.\n";
            continue;
        }

        // Get predictions
        auto probabilities = model.get_next_word_probabilities(context);

        if (probabilities.empty())
        {
            cout << "Context not found in training data.\n";
            continue;
        }

        // Show top predictions
        cout << "\nTop predictions:\n";
        for (int i = 0; i < min(5, (int)probabilities.size()); i++)
        {
            cout << fixed << setprecision(3);
            cout << "  " << probabilities[i].first
                 << " (probability: " << probabilities[i].second << ")\n";
        }

        // Generate some text
        cout << "\nGenerated text: ";
        cout << model.generate_text(context, 10) << "\n";
    }

    cout << "Goodbye!\n";
    return 0;
}
