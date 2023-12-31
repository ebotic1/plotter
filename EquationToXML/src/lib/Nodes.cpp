#include "./../../inc/generateXML.h"
#include "./../../inc/nodes.h"
#include <vector>
#include <cnt/PushBackVector.h>
#include "td/String.h"
#include "xml/Writer.h"
#include<map>
#include <utility>


class nameNode : public baseNode {
	td::String name;
public:
	nameNode(const td::String &name) : name(name) {}
	nameNode(td::String &&name) : name(name) {}

	bool nodeAction(const td::String& command, baseNode*& newChild) override {
		return false;
	}

	inline const char* getName() override{
		return name.c_str();
	}
};


class conditionNode : public baseNode {
public:
	enum class type { thenn, elsee };
private:
	type tip;
public:
	conditionNode(type);
	virtual bool nodeAction(const td::String& command, baseNode*& newChild) override;
	inline const char* getName() override {
		return (tip == type::thenn) ? "Then" : "Else";
	}
};

conditionNode::conditionNode(type t) : tip(t) {}

class singleEquation : public baseNode {
	bool added = false;
	bool consumeEnd = false;
public:
	virtual bool nodeAction(const td::String& command, baseNode*& newChild) override {
		if (command.beginsWithCI("then"))
			return true;


		if (command.beginsWithCI("end")) {
			if (consumeEnd) {
				added = true;
				consumeEnd = false;
				return true;
			}
			return false;
		}

		if (added)
			return false;

		if (command.beginsWithCI("else")) {
			nodes.push_back(new conditionNode(conditionNode::type::elsee));
			newChild = nodes.back();
			added = true;
			consumeEnd = true;
			return true;
		}

		if (command.beginsWithCI("if")) {
			setAttrib("cond", command.subStr(2, -1).trimLeft());
			nodes.push_back(new conditionNode(conditionNode::type::thenn));
			newChild = nodes.back();
			consumeEnd = true;
			return true;
		}

		added = true;
		int wPoz = command.find("[");

		if (wPoz != -1 && command.getLastChar() == ']') {
			setAttrib("fx", command.subStr(0, wPoz - 1).trimRight());
			setAttrib("w", command.subStr(wPoz + 1, command.length() - 2));
		}
		else
			setAttrib("fx", command);



		return true;
	}
	inline const char* getName() override {
		return "Eq";
	}
};


struct GroupName { static const char* val; };
const char* GroupName::val = "Group";

template<typename containerName, typename nodeType>
class containerNode : public baseNode {
	bool done = false;
public:
	bool nodeAction(const td::String& command, baseNode*& newChild) override{
		if (done) return false;

		if (command.cCompareNoCase("Group:") == 0) {
			typedef containerNode<GroupName, singleEquation> GroupNode;
			nodes.push_back(new GroupNode);
			newChild = nodes.back();
			return true;
		}
		if (command.getLastChar() == ':') return false;
		if (command.beginsWithCI("end")) {
			//done = true;
			return false;
		}



		newChild = new nodeType;
		nodes.push_back(newChild);

		return false;

	}
	
	inline const char* getName() override {
		return containerName::val;
	}

};

template<typename variableName>
class variableNode : public baseNode {
	bool added = false;
public:
	bool nodeAction(const td::String& command, baseNode*& newChild) override {
		if (added) return false;
		added = true;

		int pozEq = command.find("=");
		if (pozEq == -1) {
			setAttrib("name", command);
			return true;
		}

		setAttrib("name", command.subStr(0, pozEq - 1).trimRight());
		setAttrib("val", command.subStr(pozEq + 1, -1).trimLeft());
		
		return true;
	}

	inline const char* getName() override {
		return variableName::val;
	}
};







bool conditionNode::nodeAction(const td::String& command, baseNode*& newChild){

	if (command.beginsWithCI("end"))
		return false;
	
	
	if (command.findCI("else") != -1)
		return false;
	
	nodes.push_back(new singleEquation());
	newChild = nodes.back();

	return false;
}



struct varsName { static const char* val; };
const char* varsName::val = "Vars";

struct varName { static const char* val; };
const char* varName::val = "Var";

struct paramsName { static const char* val; };
const char* paramsName::val = "Params";

struct paramName { static const char* val; };
const char* paramName::val = "Param";

struct NLEName { static const char* val; };
const char* NLEName::val = "NLEqs";

struct ODEName { static const char* val; };
const char* ODEName::val = "ODEqs";

struct postProcName { static const char* val; };
const char* postProcName::val = "PostProc";

struct MeasEqName { static const char* val; };
const char* MeasEqName::val = "MeasEqs";

struct LimitName { static const char* val; };
const char* LimitName::val = "Limits";

struct ECsName { static const char* val; };
const char* ECsName::val = "ECs";

struct TfName { static const char* val; };
const char* TfName::val = "TFs";

typedef containerNode<varsName, variableNode<varName>> varsNode;
typedef containerNode<paramsName, variableNode<paramName>> paramsNode;
typedef containerNode<NLEName, singleEquation> NLEquationsNode;
typedef containerNode<ODEName, singleEquation> ODEquationsNode;
typedef containerNode<postProcName, singleEquation> postProcNode;
typedef containerNode<MeasEqName, singleEquation> MeasEqNode;

typedef containerNode<LimitName, singleEquation> LimitNode;
typedef containerNode<ECsName, singleEquation> ECsNode;
typedef containerNode<TfName, singleEquation> TFsNode;


class initNode : public baseNode {
public:
	virtual bool nodeAction(const td::String& command, baseNode*& newChild) override {

		if (command.cCompareNoCase("model:") == 0) {
			nodes.push_back(new modelNode());
			newChild = nodes.back();
			return true;
		}
		
		return false;
	}
	inline const char* getName() override {
		return "Init";
	}
};


modelNode::modelNode(td::String text){
	processCommands(text);
}

bool modelNode::nodeAction(const td::String& command, baseNode*& newChild){
	if (done) return false;

	if (command.beginsWithCI("end")) {
		done = true;
		return true;
	}


	if (command.getLastChar() == ':') { //radi se o novom elementu
		if (command.cCompareNoCase("Vars:") == 0)
			nodes.push_back(new varsNode());
		else if (command.cCompareNoCase("Params:") == 0)
			nodes.push_back(new paramsNode());
		else if (command.cCompareNoCase("model:") == 0)
			return true;
		else if (command.cCompareNoCase("NLEqs:") == 0 || command.cCompareNoCase("NL:") == 0)
			nodes.push_back(new NLEquationsNode());
		else if (command.cCompareNoCase("ODEqs:") == 0 || command.cCompareNoCase("ODE:") == 0)
			nodes.push_back(new ODEquationsNode());
		else if (command.cCompareNoCase("Init:") == 0)
			nodes.push_back(new initNode);
		else if (command.cCompareNoCase("PostProc:") == 0)
			nodes.push_back(new postProcNode);
		else if (command.cCompareNoCase("MeasEqs:") == 0 || command.cCompareNoCase("Meas:") == 0)
			nodes.push_back(new MeasEqNode);
		else if (command.cCompareNoCase("Limits:") == 0)
			nodes.push_back(new LimitNode);
		else if (command.cCompareNoCase("ECs:") == 0 || command.cCompareNoCase("EC:") == 0)
			nodes.push_back(new ECsNode);
		else if (command.cCompareNoCase("TFs:") == 0 || command.cCompareNoCase("TF:") == 0)
			nodes.push_back(new TFsNode);
		else
			nodes.push_back(new nameNode(command.subStr(0, command.length() - 2)));

		newChild = nodes.back();
		return true;
	}

	
}



void generateXML(const td::String& equations, const td::String& output_path) {
	modelNode m(equations);

	xml::Writer w(output_path);
	w.startDocument();

	m.printNode(w);

	w.endDocument();
	w.close();
}

