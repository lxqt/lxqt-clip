/*
lxqt-clip - clipboard history manager
Copyright (C) 2012 Petr Vanek <petr@yarpen.cz>
              2026~ LXQt team

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#pragma once

#include "ui_preferencesdialog.h"

namespace LXQt {

class PreferencesDialog : public QDialog, private Ui::PreferencesDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);

private slots:
    void accept();

    void selectIconFromFile();
    void useDefaultIcon();

    void stickyAddButton_clicked();
    void stickyRemoveButton_clicked();
    void stickyUpButton_clicked();
    void stickyDownButton_clicked();
    void listWidget_currentRowChanged(int);

  private:
    void temporarilyRembemberNewTrayIcon(const QString &path);
    QString getNewTrayIcon() const;
};

} // namespace
