#include "Application.h"

Application *Application::m_self = Q_NULLPTR;

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    init();
}

Application::~Application()
{
}

void Application::init()
{
    m_self = this;
}

void Application::handleQuitAction()
{
    DApplication::handleQuitAction();
}
