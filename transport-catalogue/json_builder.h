#pragma once

#include "json.h"
#include <optional>

namespace json{

class Builder {
private:

	Node root_{nullptr};
	std::vector<Node*> stack_ = {&root_};
	std::optional<std::string> key_{ std::nullopt };

public:
	Builder() = default;

	class BaseContext;
	class KeyContext;
	class DictContext;
	class ArrayContext;

	ArrayContext StartArray();
	Builder& EndArray();

	DictContext StartDict();
	Builder& EndDict();

	KeyContext Key(std::string);
	Builder& Value(Node::Value);

	Node Build();

};

class Builder::BaseContext {
public:
	BaseContext(Builder& builder)
		:builder_(builder) {}

	KeyContext Key(std::string);
	DictContext StartDict();
	ArrayContext StartArray();
	Builder& EndDict();
	Builder& EndArray();
protected:
	Builder& builder_;
};


class Builder::DictContext : public BaseContext {
public:
	DictContext(Builder& builder)
		:BaseContext(builder) {}

	ArrayContext& StartArray() = delete;
	Builder& EndArray() = delete;
	DictContext& StartDict() = delete;
	Builder& Value(Node::Value) = delete;
	Node Build() = delete;
};

class Builder::KeyContext : public BaseContext {
public:
	KeyContext(Builder& builder)
		:BaseContext(builder) {}
	
	Builder& EndArray() = delete;
	Builder& EndDict() = delete;
	KeyContext& Key(std::string) = delete;
	Node Build() = delete;

	DictContext Value(Node::Value value);
};

class Builder::ArrayContext : public BaseContext {
public:
	ArrayContext(Builder& builder)
		:BaseContext(builder) {}

	Builder& EndDict() = delete;
	Builder::KeyContext& Key(std::string) = delete;
	Node Build() = delete;

	ArrayContext Value(Node::Value value);
};

}//json