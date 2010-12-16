#include <WidgetSettings.h>
#include <ui_WidgetSettings.h>
using namespace GUI;

#include <QFileDialog>
#include <QMessageBox>

#include <Common/ProtoHelper.h>
#include <Common/Settings.h>

#include <Protos/gui_settings.pb.h>

void DirListModel::setDirs(const QStringList& dirs)
{
   QStringList dirsToRemove = this->dirs;

   for (QStringListIterator i(dirs); i.hasNext();)
   {
      QString dir = i.next();
      if (this->dirs.contains(dir))
      {
         dirsToRemove.removeOne(dir);
      }
      else
      {
         this->beginInsertRows(QModelIndex(), this->dirs.size(), this->dirs.size());
         this->dirs << dir;
         this->endInsertRows();
      }
   }

   for (QStringListIterator i(dirsToRemove); i.hasNext();)
   {
      int j = this->dirs.indexOf(i.next());
      if (j != -1)
      {
         this->beginRemoveRows(QModelIndex(), j, j);
         this->dirs.removeAt(j);
         this->endRemoveRows();
      }
   }
}

void DirListModel::addDir(const QString& dir)
{
   if (this->dirs.contains(dir))
      return;

   this->beginInsertRows(QModelIndex(), this->dirs.size(), this->dirs.size());
   this->dirs << dir;
   this->endInsertRows();
}

void DirListModel::addDirs(const QStringList& dirs)
{
   foreach (QString dir, dirs)
      this->addDir(dir);
}

void DirListModel::rmDir(int row)
{
   if (row >= this->dirs.size())
      return;

   this->beginRemoveRows(QModelIndex(), row, row);
   this->dirs.removeAt(row);
   this->endRemoveRows();
}

const QStringList& DirListModel::getDirs() const
{
   return this->dirs;
}

int DirListModel::rowCount(const QModelIndex& parent) const
{
   return this->dirs.size();
}

QVariant DirListModel::data(const QModelIndex& index, int role) const
{
   if (role != Qt::DisplayRole || index.row() >= this->dirs.size())
      return QVariant();
   return this->dirs[index.row()];
}

//////

WidgetSettings::WidgetSettings(CoreConnection& coreConnection, QWidget *parent)
   : QWidget(parent), ui(new Ui::WidgetSettings), coreConnection(coreConnection), initialState(true)
{
   this->ui->setupUi(this);

   this->ui->lstIncoming->setModel(&this->incomingDirsModel);
   this->ui->lstShared->setModel(&this->sharedDirsModel);

   this->ui->txtCoreAddress->setText(SETTINGS.get<QString>("core_address"));

   connect(&this->coreConnection, SIGNAL(newState(Protos::GUI::State)), this, SLOT(newState(Protos::GUI::State)));

   connect(this->ui->txtNick, SIGNAL(editingFinished()), this, SLOT(saveCoreSettings()));

   connect(this->ui->butAddIncoming, SIGNAL(clicked()), this, SLOT(addIncoming()));
   connect(this->ui->butAddShared, SIGNAL(clicked()), this, SLOT(addShared()));
   connect(this->ui->butRemoveIncoming, SIGNAL(clicked()), this, SLOT(removeIncoming()));
   connect(this->ui->butRemoveShared, SIGNAL(clicked()), this, SLOT(removeShared()));

   connect(this->ui->txtCoreAddress, SIGNAL(editingFinished()), this, SLOT(saveGUISettings()));
   connect(this->ui->txtPassword, SIGNAL(editingFinished()), this, SLOT(saveGUISettings()));
   connect(this->ui->butResetCoreAddress, SIGNAL(clicked()), this, SLOT(resetCoreAddress()));
}

WidgetSettings::~WidgetSettings()
{
   delete this->ui;
}

void WidgetSettings::coreConnected()
{
   this->ui->txtPassword->clear();
   this->ui->tabWidget->setTabEnabled(0, true);
}

void WidgetSettings::coreDisconnected()
{
   this->initialState = true;
   this->ui->tabWidget->setTabEnabled(0, false);
}

void WidgetSettings::newState(const Protos::GUI::State& state)
{
   const Protos::GUI::CoreSettings& settings = state.settings();
   if (!this->ui->txtNick->hasFocus())
      this->ui->txtNick->setText(Common::ProtoHelper::getStr(settings.myself(), &Protos::GUI::Peer::nick));

   QStringList incomingDirs;
   for (int i = 0; i < settings.destination_directory_size(); i++)
      incomingDirs << Common::ProtoHelper::getRepeatedStr(settings, &Protos::GUI::CoreSettings::destination_directory, i);
   this->incomingDirsModel.setDirs(incomingDirs);

   QStringList sharedDirs;
   for (int i = 0; i < settings.shared_directory_size(); i++)
      sharedDirs << Common::ProtoHelper::getRepeatedStr(settings, &Protos::GUI::CoreSettings::shared_directory, i);
   this->sharedDirsModel.setDirs(sharedDirs);

   // If this is the first message state received and there is no incoming folder defined we ask the user to choose one.
   if (this->initialState)
   {
      this->initialState = false;
      if (this->incomingDirsModel.rowCount() == 0)
      {
         if (QMessageBox::question(
               this,
               "No incoming folder",
               "You don't have any incoming folder, would you like to choose one?\n All downloaded files will be put in this folder",
               QMessageBox::Yes,
               QMessageBox::No
            ) == QMessageBox::Yes)
         {
            this->addIncoming();
         }
      }
   }
}

void WidgetSettings::saveCoreSettings()
{
   Protos::GUI::CoreSettings settings;
   Common::ProtoHelper::setStr(*settings.mutable_myself(), &Protos::GUI::Peer::set_nick, this->ui->txtNick->text());

   for (QStringListIterator i(this->incomingDirsModel.getDirs()); i.hasNext();)
      Common::ProtoHelper::addRepeatedStr(settings, &Protos::GUI::CoreSettings::add_destination_directory, i.next());

   for (QStringListIterator i(this->sharedDirsModel.getDirs()); i.hasNext();)
      Common::ProtoHelper::addRepeatedStr(settings, &Protos::GUI::CoreSettings::add_shared_directory, i.next());

   Common::ProtoHelper::setStr(*settings.mutable_myself(), &Protos::GUI::Peer::set_nick, this->ui->txtNick->text());

   this->coreConnection.setCoreSettings(settings);
}

void WidgetSettings::saveGUISettings()
{
   this->ui->txtCoreAddress->setText(this->ui->txtCoreAddress->text().trimmed());

   QString previousAddress = SETTINGS.get<QString>("core_address");
   SETTINGS.set("core_address", this->ui->txtCoreAddress->text());

   SETTINGS.set("password", Common::Hasher::hashWithSalt(this->ui->txtPassword->text()));

   SETTINGS.save();

   if (previousAddress != SETTINGS.get<QString>("core_address") || !this->coreConnection.isConnected())
   {
      this->coreConnection.connectToCore();
   }
}

void WidgetSettings::addIncoming()
{
   QStringList dirs = askForDirectories();
   if (!dirs.isEmpty())
   {
      this->incomingDirsModel.addDirs(dirs);
      this->saveCoreSettings();
   }
}

void WidgetSettings::addShared()
{
   QStringList dirs = askForDirectories();
   if (!dirs.isEmpty())
   {
      this->sharedDirsModel.addDirs(dirs);
      this->saveCoreSettings();
   }
}

void WidgetSettings::removeIncoming()
{
   QModelIndex index = this->ui->lstIncoming->selectionModel()->currentIndex();
   if (index.isValid())
   {
      this->incomingDirsModel.rmDir(index.row());
      this->saveCoreSettings();
   }
}

void WidgetSettings::removeShared()
{
   QModelIndex index = this->ui->lstShared->selectionModel()->currentIndex();
   if (index.isValid())
   {
      this->sharedDirsModel.rmDir(index.row());
      this->saveCoreSettings();
   }
}

void WidgetSettings::resetCoreAddress()
{
   this->ui->txtCoreAddress->setText("localhost");
   this->saveGUISettings();
}

/**
  * TODO : browse the remotes directories (Core) not the local ones.
  */
QStringList WidgetSettings::askForDirectories()
{
   QFileDialog fileDialog(this);
   fileDialog.setOption(QFileDialog::DontUseNativeDialog,true);
   //fileDialog.setOptions(QFileDialog::ShowDirsOnly);
   fileDialog.setFileMode(QFileDialog::Directory);

   QListView* l = fileDialog.findChild<QListView*>("listView");
   if (l)
      l->setSelectionMode(QAbstractItemView::ExtendedSelection);

   if (fileDialog.exec())
   {
      return fileDialog.selectedFiles();
   }
   return QStringList();
}

void WidgetSettings::showEvent(QShowEvent* event)
{
   if (this->ui->tabWidget->isTabEnabled(0))
      this->ui->tabWidget->setCurrentIndex(0);

   QWidget::showEvent(event);
}
