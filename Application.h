/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     tangguang <tangguang@uniontech.com>
*
* Maintainer: tangguang <tangguang@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// 管理程序，用于实现程序特定的退出

#ifndef APPLICATION_H
#define APPLICATION_H

#include <DApplication>

DWIDGET_USE_NAMESPACE

class Application : public DApplication
{
    Q_OBJECT

public:
    explicit Application(int &argc, char **argv);
    ~Application() Q_DECL_OVERRIDE;

    static Application* getInstance(int &argc, char **argv){
        static Application instance(argc, argv);
        return &instance;
    }

    static Application *instance() { return m_self; }
    int showQuitWindow();

protected:
    void handleQuitAction() Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(Application)

    void init();

private:
    static Application *m_self;
};

#endif // APPLICATION_H
