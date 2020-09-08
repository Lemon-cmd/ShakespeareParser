#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <regex> 
#include <fstream>
#include <sstream>
#include <boost/algorithm/string/replace.hpp>

class tokenizer
{
    private:

    struct item
    {
        std::map<std::string, int> lexicon_id;
        std::map<int, std::string> id_lexicon;
        std::vector<std::vector<std::string>> corpus;

        item(std::map<std::string, int> &lex_id, std::map<int, std::string> &id_lex, std::vector<std::vector<std::string>> &corp)
        {
            lexicon_id = lex_id; id_lexicon = id_lex; corpus = corp;
        }

        item(item const&)=delete;
        item& operator=(item const&)=delete;
    };

    std::map<std::string, std::string> symbols 
    { 
        {":", ":"}, {",", ","}, {".", "."}, {"!", "!"}, {"@", "@"}, {";", ";"}, {"\"", "\""}, {"?", "?"},
        {"'m", "am"}, {"\'ll", "will"}, {"n't", "not"}, {"'ve", "have"}, {"'d", "do"}, {"'re", "are"}, {"o'er", "over"},
        {"'D", "DO"}, {"'M", "AM"}, {"'LL", "WILL"}, {"'RE", "ARE"}, {"N'T", "NOT"}, {"'S", "'S"}, {"'s", "'s"}, {"O'er", "Over"}, 
        {"O'VER", "O'VER"}, {"'t", "not"}, {".'", ". '"}
    };

    int id = 0;
    std::map<std::string, int> lexicon_id {{"-OOV-", 0}};
    std::map<int, std::string> id_lexicon {{0, "-OOV-"}};

    void encode(const std::string word)
    {
        if (lexicon_id.find(word) == lexicon_id.end())
        {
            lexicon_id[word] = id;
            id_lexicon[id] = word;
            id ++;
        }
    }

    std::vector <std::string> clean(std::string &sentence)
    {
        std::vector <std::string> words;
        std::stringstream input(sentence);

        for (std::string word; input >> word;)
        {
            words.push_back(word);
        }

        return words;
    }

    std::vector <std::string> tag(std::vector <std::string> &sentence) 
    {
        std::vector <std::string> tokens;

        std::for_each(sentence.begin(), sentence.end(), [&tokens, this](std::string word)
        {
            if (word.size() > 1)
            {
                if (std::regex_match(word, std::regex("[A-Za-z0-9]+")) 
                && !std::regex_match(word, std::regex("[\\?!()\";/\\|',\\.\\:@#$%^&*`~\\_\\=\\+[]<>\\{\\}]")) 
                && !std::regex_match(word, std::regex("(|'t|'T|O'ver|o'ver|:|-|'S|'D|'M|'LL|'RE|'VE|N'T|'s|'d|'m|'ll|'re|'ve|n't)+"))) //it's a word or number
                {
                    encode(word);
                    tokens.push_back(word);
                }
                else 
                {   
                    std::for_each(symbols.begin(), symbols.end(), [&word](auto item)
                    {
                        std::size_t x = word.find(item.first);
                        if (x != -1)
                        {   
                                if (x == 0)
                                {
                                    boost::replace_all(word, item.first, item.second + " ");
                                }
                                else
                                {
                                    boost::replace_all(word, item.first, " " + item.second);
                                }
                        }
                    });   
                    
                    auto cleaned = clean(word);
                    std::for_each(cleaned.begin(), cleaned.end(), [&tokens, this](std::string w) {encode(w); tokens.push_back(w);});   
                }
            }
            else 
            {
                encode(word);
                tokens.push_back(word);
            }
        }
        );
        
        return tokens;
    } 

    public:
        tokenizer()
        {
            std::cout << "\nA SIMPLE TOKENIER FOR THE ENGLISH LANGUAGE." << "\n\n";
        }

        item* parse(const std::string filename)
        {
            std::vector<std::vector<std::string>> corpus;
            std::ifstream file(filename);
            std::string line;

            while(std::getline(file, line))
            {
                if (!line.empty())
                {
                    auto sentence = clean(line);
                    auto tokens = tag(sentence);
                    corpus.push_back(tokens);
                }
            }
            item* out = new item(lexicon_id, id_lexicon, corpus);
            return out;
        }   
};


int main()
{
    std::string sentence = "Today, I'll be happy again. He said: \"You're crazy!\"";
    tokenizer tk = tokenizer();
    auto data = tk.parse("shakespeare.txt");
    
    
    for (auto &row: data->corpus)
    {
        std::for_each(row.begin(), row.end(), [](std::string x) {std::cout << x << " ";});
        std::cout << "\n";
    }
    
    /*
    for (auto &item: data->lexicon_id)
    {
       std::cout << "Word: " << item.first << " ID: " << item.second << "\n";
    }    
    */
}
