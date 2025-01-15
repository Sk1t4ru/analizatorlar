#include "MainForm.h"

using namespace msclr::interop;
// ��������� ���������� ������
std::string MainForm::getNextToken(const std::string& input, size_t& pos) {
    std::string token;
    // ���� ������� ������ �� ������, �� ����� ������ � �� ����������� ';'
    while (pos < input.size() && !std::isspace(static_cast<unsigned char>(input[pos])) && input[pos] != ';') {
        token += input[pos];
        pos++;
    }
    // ���������� ������� � ����������� ����� ������
    while (pos < input.size() && (std::isspace(static_cast<unsigned char>(input[pos])) || input[pos] == ';')) {
        pos++;
    }
    return token;
}

// ����������� ������
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
            continue; // ���������� ������ ������
        }

        std::smatch match;

        // �������� �����
        if (std::regex_match(token, match, keywordRegex)) {
            lexemes.push_back({ idCounter++, "�������� �����", token });
        }
        // ��������������
        else if (std::regex_match(token, match, identifierRegex)) {
            lexemes.push_back({ idCounter++, "�������������", token });
        }
        // ������� �����
        else if (std::regex_match(token, match, numberRegex)) {
            std::cout << "������� ����� �������: " << token << ", �����: " << token.size() << std::endl;
            lexemes.push_back({ idCounter++, "������� �����", token });
        }
        // ��������� ������������
        else if (std::regex_match(token, match, assignRegex)) {
            lexemes.push_back({ idCounter++, "������������", token });
        }
        // ����� ���������
        else if (std::regex_match(token, match, compareRegex)) {
            lexemes.push_back({ idCounter++, "���������", token });
        }
        // �����������
        else if (std::regex_match(token, match, commentRegex)) {
            size_t newlinePos = input.find('\n', pos); // ���� ����� ������
            if (newlinePos == std::string::npos) {
                pos = input.size(); // ���� ����� ������ �� ������, ������������ � ����� �����
            }
            else {
                pos = newlinePos + 1; // ���������� ��������� �� ��������� ������ ����� '\n'
            }
        }
        // ����������� ������
        else {
            std::cerr << "������ ������������ �������: ����������� ������ '" << token << "'" << std::endl;
        }
    }
}

// ����������� �����
MainForm::MainForm() {
    this->Text = "����������� ����������";
    this->Size = System::Drawing::Size(800, 600);

    tabControl = gcnew TabControl();
    tabControl->Dock = DockStyle::Fill;

    tabInput = gcnew TabPage("������� ����");
    tabLexemes = gcnew TabPage("������� ������");

    tabControl->TabPages->Add(tabInput);
    tabControl->TabPages->Add(tabLexemes);

    inputPathBox = gcnew TextBox();
    inputPathBox->Location = Point(10, 10);
    inputPathBox->Size = System::Drawing::Size(600, 20);

    browseButton = gcnew Button();
    browseButton->Text = "�����...";
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
    lexemeTable->Columns[0]->Name = "���������� �����";
    lexemeTable->Columns[1]->Name = "�������";
    lexemeTable->Columns[2]->Name = "��������";

    tabLexemes->Controls->Add(lexemeTable);

    this->Controls->Add(tabControl);
}

// ���������� ������� ������ "�����"
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

