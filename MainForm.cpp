#include "MainForm.h"

using namespace msclr::interop;
// Получение следующего токена
std::string MainForm::getNextToken(const std::string& input, size_t& pos) {
    std::string token;
    // Пока текущий символ не пробел, не конец строки и не разделитель ';'
    while (pos < input.size() && !std::isspace(static_cast<unsigned char>(input[pos])) && input[pos] != ';') {
        token += input[pos];
        pos++;
    }
    // Пропускаем пробелы и разделители после токена
    while (pos < input.size() && (std::isspace(static_cast<unsigned char>(input[pos])) || input[pos] == ';')) {
        pos++;
    }
    return token;
}

// Лексический анализ
void MainForm::LexicalAnalysis(const std::string& input, std::vector<Lexeme>& lexemes) {
    int idCounter = 1;
    size_t pos = 0;
    std::regex identifierRegex("[a-z_][a-z0-9_]{0,31}");
    std::regex numberRegex("M{0,3}(CM|CD|D?C{0,3})(XC|XL|L?X{0,3})(IX|IV|V?I{0,3})");
    std::regex assignRegex(":=");
    std::regex compareRegex("<|>|=");
    std::regex keywordRegex("if|then|else");
    std::regex separatorRegex(";");
    std::regex commentRegex("//.*");

    while (pos < input.size()) {
        std::string token = getNextToken(input, pos);
        if (token.empty()) {
            continue; // Пропускаем пустые токены
        }

        std::smatch match;

        // Ключевые слова
        if (std::regex_match(token, match, keywordRegex)) {
            lexemes.push_back({ idCounter++, "Ключевое слово", token });
        }
        // Идентификаторы
        else if (std::regex_match(token, match, identifierRegex)) {
            lexemes.push_back({ idCounter++, "Идентификатор", token });
        }
        // Римские числа
        else if (std::regex_match(token, match, numberRegex)) {
            std::cout << "Римское число найдено: " << token << ", длина: " << token.size() << std::endl;
            lexemes.push_back({ idCounter++, "Римское число", token });
        }
        // Операторы присваивания
        else if (std::regex_match(token, match, assignRegex)) {
            lexemes.push_back({ idCounter++, "Присваивание", token });
        }
        // Знаки сравнения
        else if (std::regex_match(token, match, compareRegex)) {
            lexemes.push_back({ idCounter++, "Сравнение", token });
        }
        // Комментарии
        else if (std::regex_match(token, match, commentRegex)) {
            size_t newlinePos = input.find('\n', pos); // Ищем конец строки
            if (newlinePos == std::string::npos) {
                pos = input.size(); // Если конец строки не найден, перемещаемся в конец файла
            }
            else {
                pos = newlinePos + 1; // Перемещаем указатель на следующий символ после '\n'
            }
        }
        // Неизвестный символ
        else {
            std::cerr << "Ошибка лексического анализа: неизвестный символ '" << token << "'" << std::endl;
        }
    }
}

// Конструктор формы
MainForm::MainForm() {
    this->Text = "Лексический анализатор";
    this->Size = System::Drawing::Size(800, 600);

    tabControl = gcnew TabControl();
    tabControl->Dock = DockStyle::Fill;

    tabInput = gcnew TabPage("Входной файл");
    tabLexemes = gcnew TabPage("Таблица лексем");

    tabControl->TabPages->Add(tabInput);
    tabControl->TabPages->Add(tabLexemes);

    inputPathBox = gcnew TextBox();
    inputPathBox->Location = Point(10, 10);
    inputPathBox->Size = System::Drawing::Size(600, 20);

    browseButton = gcnew Button();
    browseButton->Text = "Обзор...";
    browseButton->Location = Point(620, 10);
    browseButton->Click += gcnew EventHandler(this, &MainForm::OnBrowseClick);

    inputFileBox = gcnew TextBox();
    inputFileBox->Location = Point(10, 40);
    inputFileBox->Size = System::Drawing::Size(760, 450);
    inputFileBox->Multiline = true;
    inputFileBox->ScrollBars = ScrollBars::Vertical;

    tabInput->Controls->Add(inputPathBox);
    tabInput->Controls->Add(browseButton);
    tabInput->Controls->Add(inputFileBox);

    lexemeTable = gcnew DataGridView();
    lexemeTable->Dock = DockStyle::Fill;
    lexemeTable->ColumnCount = 3;
    lexemeTable->Columns[0]->Name = "Порядковый номер";
    lexemeTable->Columns[1]->Name = "Лексема";
    lexemeTable->Columns[2]->Name = "Значение";

    tabLexemes->Controls->Add(lexemeTable);

    this->Controls->Add(tabControl);
}

// Обработчик события кнопки "Обзор"
void MainForm::OnBrowseClick(Object^ sender, EventArgs^ e) {
    OpenFileDialog^ openFileDialog = gcnew OpenFileDialog();
    openFileDialog->Filter = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*";

    if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        inputPathBox->Text = openFileDialog->FileName;
        inputFileBox->Text = File::ReadAllText(openFileDialog->FileName);

        lexemeTable->Rows->Clear();

        std::vector<Lexeme> lexemes;
        LexicalAnalysis(marshal_as<std::string>(inputFileBox->Text), lexemes);

        for (const auto& lexeme : lexemes) {
            lexemeTable->Rows->Add(lexeme.id, gcnew String(lexeme.type.c_str()), gcnew String(lexeme.value.c_str()));
        }
    }
}

