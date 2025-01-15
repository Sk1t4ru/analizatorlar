#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <msclr/marshal_cppstd.h>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::IO;

// Структура для хранения лексем
struct Lexeme {
    int id;
    std::string type;
    std::string value;
};

// Класс формы
public ref class MainForm : public Form {
public:
    MainForm();

private:
    TabControl^ tabControl;
    TabPage^ tabInput;
    TabPage^ tabLexemes;
    TextBox^ inputPathBox;
    Button^ browseButton;
    TextBox^ inputFileBox;
    DataGridView^ lexemeTable;

    // Обработчики событий
    void OnBrowseClick(Object^ sender, EventArgs^ e);

    // Лексический анализ
    void LexicalAnalysis(const std::string& input, std::vector<Lexeme>& lexemes);
    std::string getNextToken(const std::string& input, size_t& pos);
};

