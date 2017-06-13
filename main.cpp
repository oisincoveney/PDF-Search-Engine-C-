#include <QCoreApplication>
#include <QApplication>
#include <gui.h>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv)
{
    QApplication app(argc, argv); // just this line
    GUI display{};
    return 0;
}
