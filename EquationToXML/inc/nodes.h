#pragma once
#include<unordered_map>
#include<map>
#include <utility>
#include "xml/Writer.h"
#include "cnt/PushBackVector.h"
#include "td/String.h"
#include <vector>




class baseNode {

	static const td::String attributeKeywords[];
	baseNode* parent = nullptr;
	baseNode* lastChlid = this;

protected:
	
	virtual bool nodeAction(const td::String& command, baseNode*& newChild) = 0;
	td::String comment;
	size_t addLine(std::vector<std::pair<td::String, td::String>>& lines, size_t startLine = 0);
	void clear();

public:
	std::vector<baseNode*> nodes;
	std::map<td::String, td::String> attribs;

	baseNode();
	baseNode(baseNode&&) = delete;
	baseNode(const baseNode& node);
	virtual baseNode* createCopy() = 0;
	baseNode &operator =(baseNode&) = delete;

	void printNodeToString(td::String& string) const;
	void printNode(const td::String &path) const;
	virtual void printNode(xml::Writer& w) const;
	inline td::String& operator[](td::String attrib) {
		return attribs[attrib];
	}
	void addComment(const td::String& comment);
	void addComment(td::String&& comment);

	void processCommands(const td::String& text);
	virtual inline const char* getName() const = 0;

	virtual void setAttrib(const td::String& name, const td::String& val) {
		attribs[name] = val;
	}

	~baseNode() {
		for (baseNode * var : nodes) {
			delete var;
		}
	}

};


class modelNode : public baseNode {
	bool done = false;

public:
	static const td::String attributeKeywords[];
	modelNode() {};
	modelNode(const modelNode& model);
	modelNode(td::String command);
	bool nodeAction(const td::String& command, baseNode*& newChild) override;
	modelNode& operator+(const modelNode& node);
	void clear();
	baseNode* createCopy() override;
	inline const char* getName() const override {
		return "Model";
	}

};


