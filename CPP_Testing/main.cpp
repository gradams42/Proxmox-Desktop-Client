#include <QApplication>
#include "ProxmoxClientWindow.h"
#include <curl/curl.h> // Include curl initialization

int main(int argc, char *argv[])
{
    // Initialize libcurl global environment (your existing CLI code)
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Initialize the Qt Application
    QApplication a(argc, argv);
    
    // Must register Vm struct for use in signals/slots across threads (if threading is used)
    qRegisterMetaType<Vm>("Vm");
    qRegisterMetaType<QVector<Vm>>("QVector<Vm>");

    // Create and show the main window
    ProxmoxClientWindow w;
    w.show();

    // Start the application event loop
    int result = a.exec();
    
    // Cleanup libcurl
    curl_global_cleanup();
    
    return result;
}