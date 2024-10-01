#include <gui/View.h>
#include <gui/TextEdit.h>
#include <gui/VerticalLayout.h>
#include <td/Time.h>
#include <cnt/StringBuilder.h>

class LogView : public gui::View {

	gui::TextEdit textMain;
	gui::VerticalLayout _vl;

public:

	enum class LogType{info, warning, error};

	LogView() :
		_vl(1)
	{
		textMain.setAsReadOnly();
		_vl << textMain;
		setLayout(&_vl);
	}

	void appendLog(const td::String text, LogType type){
		td::Time t(true);
		cnt::StringBuilderSmall str;
		td::ColorID color;

		if(!textMain.isEmpty())
			str << "\n";

		gui::Range range(textMain.getText().glyphLength() , 0);

		str << "[ " << ((t.getHour() < 10) ? "0" : "\0") << t.getHour() << ':'; 
		str << ((t.getMinute() < 10) ? "0" : "\0") << t.getMinute() << ':' ;
		str << ((t.getSecond() < 10) ? "0" : "\0")  << t.getSecond() << " ] ";

		switch (type)
		{
		case LogType::error:
			str << tr("error");
			color = td::ColorID::Crimson;
			break;
		case LogType::info:
			str << tr("info");
			color = td::ColorID::RoyalBlue;
			break;
		case LogType::warning:
			color = td::ColorID::Gold;
			str << tr("warning");
		}

		str << ": " << text;
		const auto &strFromBuilder = str.toString();
		range.length = strFromBuilder.glyphLength();
		textMain.appendString(str.toString());

	
		textMain.setColor(range, color);
	}

};