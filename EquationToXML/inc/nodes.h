#pragma once
#include<unordered_map>
#include<map>
#include <utility>
#include "xml/Writer.h"
#include "cnt/PushBackVector.h"
#include "td/String.h"
#include <vector>
#include <cnt/StringBuilder.h>
#include <regex>




class baseNode {

	static const td::String attributeKeywords[];
	baseNode* parent = nullptr;
	baseNode* lastChlid = this;

protected:
	
	virtual bool nodeAction(const td::String& command, baseNode*& newChild) = 0;
	td::String comment;
	size_t addLine(std::vector<std::pair<td::String, td::String>>& lines, size_t startLine = 0);
	virtual void prettyPrint(cnt::StringBuilder<>& str, const td::String &indent) const;
	void clear();

public:
	std::vector<baseNode*> nodes;
	std::map<td::String, td::String> attribs;
	static const std::regex varPatten;

	baseNode();
	baseNode(baseNode&&) = delete;
	baseNode(const baseNode& node);
	virtual baseNode* createCopy(const td::String &alias) = 0;
	baseNode &operator =(baseNode&) = delete;

	void printNodeToString(td::String& string) const;
	bool printNode(const td::String &path) const;
	virtual void printNode(xml::Writer& w) const;

	void prettyPrint(td::String& text) const;
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

	virtual ~baseNode() {
		for (baseNode * var : nodes) {
			delete var;
		}
	}

};


class modelNode : public baseNode {
	bool done = false;

public:

	static const td::String attributeKeywords[];
	struct exceptionInvalidBlockName { td::String message; };
	modelNode() {};
	modelNode(const modelNode& model);
	modelNode &operator =(const modelNode &model);
	modelNode(td::String command);
	bool nodeAction(const td::String& command, baseNode*& newChild) override;
	modelNode& addWtih(const modelNode &model, const td::String &alias);
	void clear();
	bool readFromFile(const td::String &path);
	baseNode* createCopy(const td::String& alias) override;
	inline const char* getName() const override {
		return "Model";
	}

};


