#include "inputmanager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <csignal>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

InputManager* g_pManager;
bool g_continueRunning;

void signalHandler(int)
{
    qInfo() << "Signal received, exiting...";

    if (g_pManager != NULL)
    {
        g_pManager->stopMonitoring();
        delete g_pManager;
    }

    g_continueRunning = false;

    exit(0);
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    g_continueRunning = true;
    g_pManager = new InputManager();

    // Register for signals
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Check that we are root
    if ((getuid ()) != 0)
    {
        printf ("This application must be run as root...\n");
        //return 1;
    }

    g_pManager->startMonitoring();

    while (g_continueRunning)
    {
        QThread::usleep(HALF_SECOND_IN_NANOS);
    }
}
