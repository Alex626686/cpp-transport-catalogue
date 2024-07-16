#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);


        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
                };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
                };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        std::string LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }

        Node LoadArray(istream& input) {
            Array result;
            char c;
            for (; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (c != ']') {
                throw ParsingError("");
            }


            return Node(move(result));
        }



        Node LoadDict(istream& input) {
            Dict result;
            char c;
            for (; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }
                string key = LoadString(input);
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }
            if (c != '}') {
                throw ParsingError("");
            }

            return Node(move(result));
        }

        Node LoadBool(istream& input) {
            string s;
            while (isalpha(input.peek())) {
                s.push_back(input.get());
            }
            if (s != "true" && s != "false") {
                throw ParsingError("");
            }

            return Node(s == "true");
        }

        Node LoadNull(istream& input) {
            string s;
            while (isalpha(input.peek())) {
                s.push_back(input.get());
            }
            if (s != "ull") {
                throw ParsingError("");
            }
            return Node{};
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 'n') {
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace


    Node::Node(std::nullptr_t) {}

    Node::Node(int value)
        : value_(std::move(value)) {}

    Node::Node(double value)
        : value_(std::move(value)) {}

    Node::Node(bool value)
        : value_(std::move(value)) {}

    Node::Node(std::string value)
        :value_(std::move(value)) {}

    Node::Node(Dict value)
        : value_(std::move(value)) {}

    Node::Node(Array value)
        : value_(std::move(value)) {}



    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }
    bool Node::IsDouble() const {
        return std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_);
    }
    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }
    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }
    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }
    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(value_);
    }
    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }
    bool Node::IsDict() const {
        return std::holds_alternative<Dict>(value_);
    }

    const Node::Value& Node::GetValue() const {
        return value_;
    }

    int Node::AsInt() const {
        if (IsInt()) {
            return std::get<int>(value_);
        }
        throw std::logic_error("");
    }

    bool Node::AsBool() const {
        if (IsBool()) {
            return std::get<bool>(value_);
        }
        throw std::logic_error("");
    }

    double Node::AsDouble() const {
        if (IsPureDouble()) {
            return std::get<double>(value_);
        }
        if (IsInt()) {
            return std::get<int>(value_);
        }
        throw std::logic_error("");
    }

    const std::string& Node::AsString() const {
        if (IsString()) {
            return std::get<std::string>(value_);
        }
        throw std::logic_error("");
    }

    const Array& Node::AsArray() const {
        if (IsArray()) {
            return std::get<Array>(value_);
        }
        throw std::logic_error("");
    }

    const Dict& Node::AsDict() const {
        if (IsDict()) {
            return std::get<Dict>(value_);
        }
        throw std::logic_error("");
    }

    bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.GetValue() == rhs.GetValue();
    }

    bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    Document::Document(Node root)
        : root_(root) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs.GetRoot() == rhs.GetRoot());
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }



    // Перегрузка функции PrintValue для вывода значений null
    void PrintValue(const std::nullptr_t, std::ostream& out) {
        out << "null"sv;
    }

    void PrintValue(const int i, std::ostream& out) {
        out << i;
    }

    void PrintValue(const double d, std::ostream& out) {
        out << d;
    }

    void PrintValue(const bool b, std::ostream& out) {
        if (b == 1) {
            out << "true"sv;
            return;
        }
        out << "false"sv;
    }

    void PrintValue(const std::string& str, std::ostream& out) {
        out << "\"";
        for (const char c : str) {
            if (c == '\"') {
                out << "\\\"";
            }
            else if (c == '\\') {
                out << "\\\\";
            }
            else if (c == '\r') {
                out << "\\r";
            }
            else if (c == '\n') {
                out << "\\n";
            }
            else if (c == '\t') {
                out << "\\t";
            }
            else {
                out << c;
            }
        }
        out << "\"";
    }

    void PrintNode(const Node& node, std::ostream& out);

    void PrintValue(const Array& arr, std::ostream& out) {
        out << '[';
        bool is_first = true;
        for (const auto& elem : arr) {
            if (!is_first) {
                out << ", ";
            }
            is_first = false;
            PrintNode(elem, out);
        }
        out << ']';
    }

    void PrintValue(const Dict& dict, std::ostream& out) {
        out << '{';
        bool is_first = true;
        for (const auto& elem : dict) {
            if (!is_first) {
                out << ", ";
            }
            is_first = false;
            PrintNode(elem.first, out);
            out << ": ";
            PrintNode(elem.second, out);
        }
        out << '}';

    }

    void PrintNode(const Node& node, std::ostream& out) {
        std::visit(
            [&out](const auto& value) { PrintValue(value, out); },
            node.GetValue());
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), output);
    }

}  // namespace json