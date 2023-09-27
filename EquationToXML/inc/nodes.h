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

public:
	std::vector<baseNode*> nodes;
	std::map<td::String, td::String> attribs;

	virtual void printNode(xml::Writer& w);
	inline td::String& operator[](td::String attrib) {
		return attribs[attrib];
	}
	void addComment(const td::String& comment);
	void addComment(td::String&& comment);

	void processCommands(const td::String& text);
	virtual inline const char* getName() = 0;

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
	modelNode(td::String command);
	bool nodeAction(const td::String& command, baseNode*& newChild) override;
	inline const char* getName() override {
		return "Model";
	}

};


