#pragma once
#include <unordered_map>
#include<map>
#include <utility>
#include "xml/Writer.h"
#include "cnt/PushBackVector.h"
#include "td/String.h"
#include <vector>
#include <cnt/StringBuilder.h>
#include <regex>
#include <unordered_set>
#include <td/StringConverter.h>
#include <array>

#include <array>
#include <algorithm>
#include <cstddef>



class baseNode {
public:




	template <std::size_t N>
	struct ConstExprString {
		char data[N];
		constexpr ConstExprString(const char(&str)[N]) {
			for (std::size_t i = 0; i < N; ++i)
				data[i] = str[i];
		}

		constexpr const char*get() const {
			return data;
		}
	};

private:

	baseNode* parent = nullptr;
	baseNode* lastChlid = this;

	static std::cmatch match;
	const static std::regex _lineExtract;
	static std::cmatch match2;
	static const std::regex _attribsExtract;

protected:

	bool _done = false;

	static unsigned int _processingLine;
	std::vector<baseNode*> nodes;
	td::String _comment;
	virtual bool nodeAction(const char* cmndStart, const char* cmndEnd, baseNode*& newChild) = 0;
	/*
	
	nodeAction prestavlja glavnu funkciju koju node koristi da procesira svoje komande.
	return false -> node ne moze da procesira ovu komandu, prosljedi je parrent-u
	return true -> node je procesirao datu komandu

	_done = true oznacava da je node zatvoren, nece vise primati komandi i ako je vratio true. Naredni komentari se nece ispisivati u ovom elementu

	ukoliko node stvori novo dijete onda njegov pointer stavlja u newChild argument.
	Time se komande nastavljaju slati njemu.

	*/
	
	void addLine(const char* start, const char *end, const char*comment, int commentLen);
	virtual bool prettyPrint(cnt::StringBuilder<>& str, td::String &indent) const;
	virtual void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const;
	template<ConstExprString... excludeAttribs>
	void prettyPrintAttribs(cnt::StringBuilder<>& str) const;
	void clear();
	baseNode(const baseNode& node, const td::String &alias);

public:
	
	static const std::unordered_set<td::String> attributeKeywords, functionKeywords;
	std::map<td::String, td::String> _attribs;
	static const std::regex varPatten;
	const std::vector<baseNode*> &getNodes();
	baseNode *getParent() const;
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
		return _attribs[attrib];
	}
	void addComment(const td::String& comment, bool forceNewline = false, bool exitComment = false);
	void addChild(baseNode *childNode);
	void setAsDone();

	void processCommands(const td::String& text);
	virtual inline const char* getName() const = 0;

	virtual void setAttrib(const td::String& name, const td::String& val) {
		_attribs[name] = val;
	}

	virtual ~baseNode() {
		for (baseNode * var : nodes) {
			delete var;
		}
	}





};


class modelNode : public baseNode {
	bool done = false;
	modelNode(const modelNode& model, const td::String &alias);
public:

	struct exceptionInvalidBlockName { td::String message; int line; };
	struct exceptionInvalidCommand { td::String message; int line;};
	struct exceptionInvalidAttribute { td::String message; int line; };
	modelNode() {};
	modelNode(const modelNode& model) = default;
	modelNode &operator =(const modelNode &model);
	modelNode(td::String command);
	bool nodeAction(const char* cmndStart, const char* cmndEnd, baseNode*& newChild) override;
	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const;
	modelNode& addWtih(const modelNode &model, const td::String &alias);
	void clear();
	bool readFromFile(const td::String &path);
	baseNode* createCopy(const td::String& alias) override;
	inline const char* getName() const override {
		return "Model";
	}

};

