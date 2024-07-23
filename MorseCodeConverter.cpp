#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>
#include <vector>
#include <exception>
#include <ctime>

// Define the Morse code dictionary
const std::map<char, std::string> MORSE_CODE_DICT = {
    {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."}, {'E', "."},
    {'F', "..-."}, {'G', "--."}, {'H', "...."}, {'I', ".."}, {'J', ".---"},
    {'K', "-.-"}, {'L', ".-.."}, {'M', "--"}, {'N', "-."}, {'O', "---"},
    {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."}, {'S', "..."}, {'T', "-"},
    {'U', "..-"}, {'V', "...-"}, {'W', ".--"}, {'X', "-..-"}, {'Y', "-.--"},
    {'Z', "--.."}, {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
    {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."}, {'8', "---.."},
    {'9', "----."}, {'.', ".-.-.-"}, {',', "--..--"}, {'?', "..--.."}, {'\'', ".----."},
    {'!', "-.-.--"}, {'/', "-..-."}, {'(', "-.--."}, {')', "-.--.-"}, {'&', ".-..."},
    {':', "---..."}, {';', "-.-.-."}, {'=', "-...-"}, {'+', ".-.-."}, {'-', "-....-"},
    {'_', "..--.-"}, {'"', ".-..-."}, {'$', "...-..-"}, {'@', ".--.-."}, {' ', "/"}
};

class MorseCodeConverter {
public:
    void convertTextToMorseAndSave(const std::string& inputFilePath, const std::string& outputFilePath) {
        try {
            std::string text = readFile(inputFilePath);
            std::string morseCode = textToMorseCode(text);
            generateWavFile(morseCode, outputFilePath);
            logConversion(inputFilePath, outputFilePath);
        } catch (const std::exception& e) {
            logError(e.what());
        }
    }

private:
    std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open input file: " + filePath);
        }
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    std::string textToMorseCode(const std::string& text) {
        std::string morseCode;
        for (char ch : text) {
            ch = toupper(ch);
            if (MORSE_CODE_DICT.count(ch)) {
                morseCode += MORSE_CODE_DICT.at(ch) + " ";
            } else {
                morseCode += ch; // Append character as is if not found in dictionary
            }
        }
        return morseCode;
    }

    void generateWavFile(const std::string& morseCode, const std::string& outputFilePath) {
        std::ofstream outFile(outputFilePath, std::ios::binary);
        if (!outFile.is_open()) {
            throw std::runtime_error("Failed to open output WAV file: " + outputFilePath);
        }

        // Write WAV header
        const int sampleRate = 44100;
        const short channels = 1;
        const short bitsPerSample = 16;
        const int byteRate = sampleRate * channels * bitsPerSample / 8;
        const int blockAlign = channels * bitsPerSample / 8;

        outFile.write("RIFF", 4);
        int chunkSize = 36; // Placeholder for chunk size
        outFile.write(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
        outFile.write("WAVE", 4);
        outFile.write("fmt ", 4);
        int subchunk1Size = 16;
        outFile.write(reinterpret_cast<char*>(&subchunk1Size), sizeof(subchunk1Size));
        short audioFormat = 1;
        outFile.write(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat));
        outFile.write(reinterpret_cast<char*>(&channels), sizeof(channels));
        outFile.write(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate));
        outFile.write(reinterpret_cast<char*>(&byteRate), sizeof(byteRate));
        outFile.write(reinterpret_cast<char*>(&blockAlign), sizeof(blockAlign));
        outFile.write(reinterpret_cast<char*>(&bitsPerSample), sizeof(bitsPerSample));
        outFile.write("data", 4);
        
        // Placeholder for data chunk size
        int dataChunkSizePos = outFile.tellp();
        int dataChunkSize = 0;
        outFile.write(reinterpret_cast<char*>(&dataChunkSize), sizeof(dataChunkSize));

        // Generate wave data for morse code
        std::vector<char> soundData;

        for (char symbol : morseCode) {
            if (symbol == '.') {
                // Short beep
                addTone(soundData, sampleRate, 0.1); // 100 ms for short beep
            } else if (symbol == '-') {
                // Long beep
                addTone(soundData, sampleRate, 0.3); // 300 ms for long beep
            } else if (symbol == ' ') {
                // Letter space (short silence)
                addSilence(soundData, sampleRate, 0.1); // 100 ms
            }
        }

        // Write the generated sound data
        outFile.write(soundData.data(), soundData.size());

        // Update the size of the data chunk
        dataChunkSize = static_cast<int>(soundData.size());
        outFile.seekp(dataChunkSizePos);
        outFile.write(reinterpret_cast<char*>(&dataChunkSize), sizeof(dataChunkSize));

        // Update the size of the RIFF chunk
        chunkSize += dataChunkSize + 36;
        outFile.seekp(4);
        outFile.write(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));

        outFile.close();
    }

    void addTone(std::vector<char>& soundData, int sampleRate, double duration) {
        int numSamples = static_cast<int>(sampleRate * duration);
        for (int i = 0; i < numSamples; ++i) {
            short value = static_cast<short>(32767 * 0.5 * sin((2 * M_PI * 440 * i) / sampleRate)); // 440 Hz
            soundData.push_back(value & 0xff);
            soundData.push_back((value >> 8) & 0xff);
        }
    }

    void addSilence(std::vector<char>& soundData, int sampleRate, double duration) {
        int numSamples = static_cast<int>(sampleRate * duration);
        for (int i = 0; i < numSamples; ++i) {
            soundData.push_back(0);
            soundData.push_back(0);
        }
    }

    void logConversion(const std::string& inputFile, const std::string& outputFile) {
        std::ofstream logFile("conversion.log", std::ios::app);
        logFile << "Converted " << inputFile << " to " << outputFile << " at " << getCurrentTime() << "\n";
    }

    void logError(const std::string& errorMessage) {
        std::ofstream logFile("error.log", std::ios::app);
        logFile << "Error: " << errorMessage << " at " << getCurrentTime() << "\n";
    }

    std::string getCurrentTime() {
        time_t now = time(0);
        char buffer[80];
        struct tm timeInfo;
        localtime_r(&now, &timeInfo);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
        return std::string(buffer);
    }
};

int main() {
    MorseCodeConverter converter;
    std::string inputDirectory = "input_texts"; // Set your input directory
    std::string outputDirectory = "output_audio"; // Set your output directory

    // Loop through text files in the input directory
    try {
        for (const auto& entry : std::filesystem::directory_iterator(inputDirectory)) {
            if (entry.path().extension() == ".txt") {
                std::string inputFilePath = entry.path().string();
                std::string outputFilePath = outputDirectory + "/" + entry.path().stem().string() + ".wav";
                converter.convertTextToMorseAndSave(inputFilePath, outputFilePath);
            }
        }
    } catch (const std::exception& e) {
        converter.logError(e.what());
    }

    return 0;
}
