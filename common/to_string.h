#include "td/String.h"
#include "td/LinePattern.h"

td::String to_string(td::ColorID) {

	return "color";
}


td::String to_string(td::LinePattern) {

	return "pattern";

}