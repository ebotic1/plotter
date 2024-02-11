#include"gui/Application.h"
#include"mainWindow.h"
#include <gui/WinMain.h>

class Application : public gui::Application
{


protected:

    gui::Window* createInitialWindow() override
    {
        return new MainWindow();
    }

public:
    Application(int argc, const char** argv)
        : gui::Application(argc, argv)
    {
    }
};





int main(int argc, const char** argv) {

    std::string file_path = "./IDEMode.txt";
    if (std::filesystem::exists(file_path)) {
        std::cout << "Pokrenuti u IDE modu";
    }
    else {
        std::cout << "Pokrenuti u instalacijskom modu";
        std::ofstream new_file(file_path);
        new_file << "daw\n";
        new_file.close();
    }

    Application app(argc, argv);
    app.init("BA");
    return app.run();

}