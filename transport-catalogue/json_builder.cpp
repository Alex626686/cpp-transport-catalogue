#include "json_builder.h"

namespace json{

    Node GetNode(Node::Value value) {
        if (std::holds_alternative<int>(value)) return Node(std::get<int>(value));
        if (std::holds_alternative<double>(value)) return Node(std::get<double>(value));
        if (std::holds_alternative<std::string>(value)) return Node(std::get<std::string>(value));
        if (std::holds_alternative<std::nullptr_t>(value)) return Node(std::get<std::nullptr_t>(value));
        if (std::holds_alternative<bool>(value)) return Node(std::get<bool>(value));
        if (std::holds_alternative<Dict>(value)) return Node(std::get<Dict>(value));
        if (std::holds_alternative<Array>(value)) return Node(std::get<Array>(value));
        return {};
    }

    Builder::ArrayContext Builder::StartArray(){
        if (stack_.empty()) throw std::logic_error("StartArray(...) error");

        if (stack_.back()->IsDict() && key_) {
            auto& dict = std::get<Dict>(stack_.back()->GetValue());
            auto [pos, _] = dict.emplace(std::move(key_.value()), Array());
            key_ = std::nullopt;
            stack_.emplace_back(&pos->second);

            return *this;
        }
        else if (stack_.back()->IsArray()) {
            auto& array = std::get<Array>(stack_.back()->GetValue());
            array.emplace_back(Array());
            stack_.emplace_back(&array.back());

            return *this;
        }
        else if (stack_.back()->IsNull()) {
            stack_.back()->GetValue() = Array();

            return *this;
        }

        throw std::logic_error("StartArray(...) error");
    }

    Builder& Builder::EndArray(){
        if (stack_.empty()) throw std::logic_error("EndArray(...) error");
        if (stack_.back()->IsArray()) {
            stack_.pop_back();
        }
        else {
            throw std::logic_error{ "EndArray(...) error" };
        }
        return *this;
    }

    Builder::DictContext Builder::StartDict() {
        if (stack_.empty()) throw std::logic_error("StartDict(...) error");

        if (stack_.back()->IsDict() && key_) {
            Dict& dict = std::get<Dict>(stack_.back()->GetValue());
            auto [pos, _] = dict.emplace(std::move(key_.value()), Dict());
            key_ = std::nullopt;
            stack_.emplace_back(&pos->second);

            return *this;
        }
        else if (stack_.back()->IsArray()) {
            auto& array = std::get<Array>(stack_.back()->GetValue());
            array.emplace_back(Dict());
            stack_.emplace_back(&array.back());

            return *this;
        }
        else if (stack_.back()->IsNull()) {
            stack_.back()->GetValue() = Dict();

            return *this;
        }
        throw std::logic_error("StartDict(...) error");
    }

    Builder& Builder::EndDict()    {
        if (stack_.empty()) throw std::logic_error("EndDict(...) error");
        if (stack_.back()->IsDict()) {
            stack_.pop_back();
            return *this;
        }
        throw std::logic_error{ "EndDict(...) error" };        
    }

    Builder::KeyContext Builder::Key(std::string key){
        if (!stack_.empty() && stack_.back()->IsDict() && !key_) {
            key_ = std::move(key);
            return *this;
        }
            throw std::logic_error{ "Key(...) error" };
    }

    Builder& Builder::Value(Node::Value value){
        if (stack_.empty()) throw std::logic_error("Value(...) error");

        if (stack_.back()->IsDict() && key_) {
            Dict& dict = std::get<Dict>(stack_.back()->GetValue());
            dict.emplace(key_.value(), GetNode(value));
            key_ = std::nullopt;

            return *this;
        }
        else if (stack_.back()->IsArray()) {
            Array& array = std::get<Array>(stack_.back()->GetValue());
            array.push_back(GetNode(value));

            return *this;
        }
        else if (root_.IsNull()) {
            root_.GetValue() = std::move(value);
            stack_.clear();
            return *this;
        }
        throw std::logic_error("Value(...) error");
    }

    Node Builder::Build(){
        if (stack_.empty()) {
            return std::move(root_);
        }
        throw std::logic_error("Builder is invalid");
    }



    Builder::KeyContext Builder::BaseContext::Key(std::string key){
        return builder_.Key(std::move(key));
    }

    Builder::DictContext Builder::BaseContext::StartDict(){
        return builder_.StartDict();
    }

    Builder::ArrayContext Builder::BaseContext::StartArray(){
        return builder_.StartArray();
    }

    Builder& Builder::BaseContext::EndDict(){
        return builder_.EndDict();
    }

    Builder& Builder::BaseContext::EndArray()    {
        return builder_.EndArray();
    }

    Builder::DictContext Builder::KeyContext::Value(Node::Value value){
        return builder_.Value(value);
    }

    Builder::ArrayContext Builder::ArrayContext::Value(Node::Value value){
        return builder_.Value(value);
    }

}//json

