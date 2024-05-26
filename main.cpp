#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <regex>

// Function to strip comments from the code
std::string stripComments(const std::string& code) {
    std::string noComments;
    bool inBlockComment = false;
    bool inLineComment = false;

    for (size_t i = 0; i < code.length(); ++i) {
        if (inBlockComment) {
            if (code[i] == '*' && i + 1 < code.length() && code[i + 1] == '/') {
                inBlockComment = false;
                ++i; // Skip the '/'
            }
        } else if (inLineComment) {
            if (code[i] == '\n') {
                inLineComment = false;
                noComments += code[i];
            }
        } else {
            if (code[i] == '/' && i + 1 < code.length() && code[i + 1] == '*') {
                inBlockComment = true;
                ++i; // Skip the '*'
            } else if (code[i] == '/' && i + 1 < code.length() && code[i + 1] == '/') {
                inLineComment = true;
                ++i; // Skip the second '/'
            } else {
                noComments += code[i];
            }
        }
    }
    return noComments;
}

// Function to obfuscate variable names
std::string obfuscateVariables(const std::string& code) {
    std::unordered_map<std::string, std::string> varMap;
    std::string obfuscatedCode;
    std::istringstream stream(code);
    std::string line;
    int varCount = 0;

    std::regex varPattern(R"(\b(int|float|double|char|bool|string)\s+([a-zA-Z_]\w*)\b)");
    std::smatch match;

    while (std::getline(stream, line)) {
        std::string processedLine = line;
        while (std::regex_search(processedLine, match, varPattern)) {
            std::string varType = match[1];
            std::string varName = match[2];

            if (varMap.find(varName) == varMap.end()) {
                varMap[varName] = "var" + std::to_string(varCount++);
            }

            processedLine = match.suffix().str();
        }

        for (const auto& pair : varMap) {
            std::regex namePattern("\\b" + pair.first + "\\b");
            line = std::regex_replace(line, namePattern, pair.second);
        }

        obfuscatedCode += line + "\n";
    }

    return obfuscatedCode;
}

// Main function to handle file input/output
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile) {
        std::cerr << "Error opening input file\n";
        return 1;
    }

    std::ostringstream buffer;
    buffer << inputFile.rdbuf();
    std::string code = buffer.str();
    inputFile.close();

    // Strip comments
    std::string noCommentsCode = stripComments(code);

    // Obfuscate variables
    std::string obfuscatedCode = obfuscateVariables(noCommentsCode);

    std::ofstream outputFile(argv[2]);
    if (!outputFile) {
        std::cerr << "Error opening output file\n";
        return 1;
    }

    outputFile << obfuscatedCode;
    outputFile.close();

    std::cout << "Obfuscation complete. Output written to " << argv[2] << "\n";

    return 0;
}

