
         #include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

using namespace std;

class TrieNode {
public:
    bool isWord;
    unordered_map<char, TrieNode*> children;

    TrieNode() {
        isWord = false;
    }
};

class SmartDictionary {
private:
    TrieNode* root;

    void collectWords(TrieNode* node, string current,
                      vector<string>& results) {
        if (node == nullptr || results.size() >= 10)
            return;

        if (node->isWord)
            results.push_back(current);

        for (auto& pair : node->children) {
            collectWords(node->children[pair.first],
                         current + pair.first,
                         results);
        }
    }

    int editDistance(const string& a, const string& b) {
        int n = a.size();
        int m = b.size();

        vector<vector<int>> dp(n + 1,
                               vector<int>(m + 1));

        for (int i = 0; i <= n; i++)
            dp[i][0] = i;

        for (int j = 0; j <= m; j++)
            dp[0][j] = j;

        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {

                if (a[i - 1] == b[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1];
                } else {
                    dp[i][j] = 1 + min({
                        dp[i - 1][j],
                        dp[i][j - 1],
                        dp[i - 1][j - 1]
                    });
                }
            }
        }

        return dp[n][m];
    }

    void collectAllWords(TrieNode* node,
                         string current,
                         vector<string>& words) {
        if (node == nullptr)
            return;

        if (node->isWord)
            words.push_back(current);

        for (auto& pair : node->children) {
            collectAllWords(node->children[pair.first],
                            current + pair.first,
                            words);
        }
    }

public:

    SmartDictionary() {
        root = new TrieNode();
    }

    void insertWord(string word) {
        transform(word.begin(), word.end(),
                  word.begin(), ::tolower);

        TrieNode* current = root;

        for (char c : word) {
            if (current->children.find(c) ==
                current->children.end()) {

                current->children[c] = new TrieNode();
            }

            current = current->children[c];
        }

        current->isWord = true;
    }

    bool searchWord(string word) {
        transform(word.begin(), word.end(),
                  word.begin(), ::tolower);

        TrieNode* current = root;

        for (char c : word) {
            if (current->children.find(c) ==
                current->children.end()) {
                return false;
            }

            current = current->children[c];
        }

        return current->isWord;
    }

    vector<string> autocomplete(string prefix) {
        transform(prefix.begin(), prefix.end(),
                  prefix.begin(), ::tolower);

        TrieNode* current = root;

        for (char c : prefix) {
            if (current->children.find(c) ==
                current->children.end()) {
                return {};
            }

            current = current->children[c];
        }

        vector<string> suggestions;
        collectWords(current, prefix, suggestions);

        sort(suggestions.begin(), suggestions.end());

        return suggestions;
    }

    vector<string> spellingSuggestions(string word) {
        transform(word.begin(), word.end(),
                  word.begin(), ::tolower);

        vector<string> allWords;
        collectAllWords(root, "", allWords);

        vector<pair<int, string>> candidates;

        for (const string& candidate : allWords) {
            int distance = editDistance(word, candidate);

            if (distance <= 2) {
                candidates.push_back(
                    {distance, candidate}
                );
            }
        }

        sort(candidates.begin(), candidates.end());

        vector<string> result;

        for (int i = 0;
             i < candidates.size() && i < 5;
             i++) {

            result.push_back(candidates[i].second);
        }

        return result;
    }
};

void loadDictionary(SmartDictionary& dictionary,
                    const string& filename) {

    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Could not open dictionary file.\n";
        return;
    }

    string word;

    while (file >> word) {
        dictionary.insertWord(word);
    }

    file.close();
}

int main() {

    SmartDictionary dictionary;

    loadDictionary(dictionary, "words.txt");

    int choice;

    while (true) {

        cout << "\n==============================\n";
        cout << " Smart Dictionary System\n";
        cout << "==============================\n";

        cout << "1. Search word\n";
        cout << "2. Autocomplete\n";
        cout << "3. Spelling suggestion\n";
        cout << "4. Add new word\n";
        cout << "5. Exit\n";

        cout << "\nEnter choice: ";
        cin >> choice;

        if (choice == 1) {

            string word;
            cout << "Enter word: ";
            cin >> word;

            if (dictionary.searchWord(word))
                cout << "Word found!\n";
            else
                cout << "Word not found.\n";
        }

        else if (choice == 2) {

            string prefix;
            cout << "Enter prefix: ";
            cin >> prefix;

            vector<string> suggestions =
                dictionary.autocomplete(prefix);

            if (suggestions.empty()) {
                cout << "No autocomplete suggestions.\n";
            } else {
                cout << "\nSuggestions:\n";

                for (const string& word : suggestions)
                    cout << "- " << word << "\n";
            }
        }

        else if (choice == 3) {

            string word;
            cout << "Enter possibly misspelled word: ";
            cin >> word;

            vector<string> suggestions =
                dictionary.spellingSuggestions(word);

            if (suggestions.empty()) {
                cout << "No close spelling suggestions found.\n";
            } else {
                cout << "\nDid you mean:\n";

                for (const string& suggestion : suggestions)
                    cout << "- " << suggestion << "\n";
            }
        }

        else if (choice == 4) {

            string word;
            cout << "Enter new word: ";
            cin >> word;

            dictionary.insertWord(word);

            cout << "Word added successfully!\n";
        }

        else if (choice == 5) {

            cout << "Goodbye!\n";
            break;
        }

        else {
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}
