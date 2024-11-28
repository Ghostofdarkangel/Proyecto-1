#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cctype>
#include <vector>
#include <algorithm>
#include <cstring> // Para usar funciones de C como strcpy

// Función para convertir una cadena a minúsculas
std::string toLower(const std::string& str) {
    std::string lowerStr;
    for (size_t i = 0; i < str.length(); ++i) {
        lowerStr += std::tolower(static_cast<unsigned char>(str[i]));
    }
    return lowerStr;
}

// Función para eliminar signos de puntuación de una cadena
std::string removePunctuation(const std::string& str) {
    std::string cleanStr;
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::ispunct(static_cast<unsigned char>(str[i]))) {
            cleanStr += str[i];
        }
    }
    return cleanStr;
}

// Función para calcular la distancia de Levenshtein entre dos cadenas
int levenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t m = s1.size();
    const size_t n = s2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    for (size_t i = 0; i <= m; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= n; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
        }
    }

    return dp[m][n];
}

// Función para sugerir correcciones basadas en la distancia de Levenshtein
std::string suggestCorrection(const std::string& word, const std::map<std::string, int>& diccionario) {
    int minDistance = INT_MAX;
    std::string suggestion;

    for (const auto& entry : diccionario) {
        int distance = levenshteinDistance(word, entry.first);
        if (distance < minDistance) {
            minDistance = distance;
            suggestion = entry.first;
        }
    }

    return suggestion;
}

// Función para cargar el diccionario base
void loadDictionary(std::map<std::string, int>& diccionario) {
    std::ifstream archivo("diccionario.txt");
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo del diccionario" << std::endl;
        return;
    }

    std::string palabra;
    while (archivo >> palabra) {
        diccionario[palabra] = 0;
    }
    archivo.close();
}

// Función para corregir palabras y actualizar estadísticas
void processFile(std::map<std::string, int>& diccionario, std::map<std::string, int>& topWords) {
    std::ifstream archivo("archivo.txt");
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo de entrada" << std::endl;
        return;
    }

    std::string palabra;
    while (archivo >> palabra) {
        // Eliminar signos de puntuación y convertir a minúsculas
        palabra = removePunctuation(palabra);
        palabra = toLower(palabra);

        // Verificar si la palabra está en el diccionario
        if (diccionario.find(palabra) != diccionario.end()) {
            diccionario[palabra]++;
        }
        else {
            std::string suggestion = suggestCorrection(palabra, diccionario);
            std::cout << "Palabra no encontrada: " << palabra << ". Quizás quiso decir: " << suggestion << "?" << std::endl;
        }

        // Actualizar el conteo de las palabras más usadas
        topWords[palabra]++;
    }
    archivo.close();
}

// Función para guardar las palabras más usadas
void saveTopWords(const std::map<std::string, int>& topWords) {
    std::ofstream archivo("top_words.txt");
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo para guardar las palabras más usadas" << std::endl;
        return;
    }

    for (std::map<std::string, int>::const_iterator it = topWords.begin(); it != topWords.end(); ++it) {
        archivo << it->first << ": " << it->second << std::endl;
    }
    archivo.close();
}

int main() {
    // Diccionario base y mapa para palabras más usadas
    std::map<std::string, int> diccionario;
    std::map<std::string, int> topWords;

    // Cargar el diccionario base
    loadDictionary(diccionario);

    // Procesar el archivo de entrada
    processFile(diccionario, topWords);

    // Guardar las palabras más usadas
    saveTopWords(topWords);

    return 0;
}
