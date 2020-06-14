#include "ngcquerymodel.h"
#include <QDebug>

#define GCODE_REPO_PATH "C:\\Users\\Кирилл\\Desktop\\WoodenDocs\\Drawings\\woodenGcode"

NgcQueryModel::NgcQueryModel(QObject *parent, QSqlDatabase db) : QSqlTableModel (parent, db)
{m_connectionState = false;}

bool NgcQueryModel::updateNgc()
{
    m_ngc.clear();
    m_ngc.squeeze();

    // acquire local files and modify if necessary
    QDirIterator it( GCODE_REPO_PATH , QStringList() << "*.ngc", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        Ngc tempNgc;

        QFile f(it.next());
        tempNgc.rawName = f.fileName();

        QFileInfo fileInfo(f);
        fileInfo.setCaching(false);
        QString fileName;
        fileName = fileInfo.completeBaseName();


        QStringList fields;
        fields = fileName.split(QRegExp("-"));


        bool configuredSuccesfuly = false;

        tempNgc.sizeSet = false;
        tempNgc.width = 0;
        tempNgc.height = 0;
        tempNgc.requireCreating = true;

        if (fields.size() < 4){
            m_message = "Проблемы в названии файла - слишком короткое " + fileName;
            emit messageChanged(m_message);
            qDebug() << "File name too short " << fileName;
            return 0;
        }
        tempNgc.name = fields.at(0);
        tempNgc.material = fields.at(1);
        tempNgc.action = fields.at(2);
        tempNgc.mapType = fields.at(3);

        if(tempNgc.action == "cut"){
            if (tempNgc.material == "ply"){
                tempNgc.minPower = 230;
                tempNgc.maxPower = 260;
                tempNgc.G1Speed = 1650;
                tempNgc.acceleration = 300;
                tempNgc.withNotches = true;
                configuredSuccesfuly = true;
            }
            if (tempNgc.material == "glass"){
                tempNgc.minPower = 330;
                tempNgc.maxPower = 360;
                tempNgc.G1Speed = 1500;
                tempNgc.acceleration = 250;
                tempNgc.withNotches = true;
                configuredSuccesfuly = true;
            }
            if (tempNgc.material == "card"){
                tempNgc.minPower = 350;
                tempNgc.maxPower = 360;
                tempNgc.G1Speed = 3000;
                tempNgc.acceleration = 600;
                tempNgc.withNotches = true;
                configuredSuccesfuly = true;
            }
            tempNgc.topHood =true;
            tempNgc.bottomHood =true;
        }
        if(tempNgc.action == "botEngrLine"){
            if (tempNgc.material == "ply"){
                tempNgc.minPower = 47;
                tempNgc.maxPower = 48;
                tempNgc.G1Speed = 4000;
                tempNgc.acceleration = 500;
                tempNgc.topHood =true;
                tempNgc.bottomHood =false;
                tempNgc.withNotches = false;
                configuredSuccesfuly = true;
            }
        }
        if(tempNgc.action == "topEngrLine"){
            if (tempNgc.material == "ply"){
                tempNgc.minPower = 47;
                tempNgc.maxPower = 48;
                tempNgc.G1Speed = 4000;
                tempNgc.acceleration = 500;
                tempNgc.topHood =true;
                tempNgc.bottomHood =false;
                tempNgc.withNotches = false;
                configuredSuccesfuly = true;
            }
            if (tempNgc.material == "glass"){
                tempNgc.minPower = 90;
                tempNgc.maxPower = 100;
                tempNgc.G1Speed = 4000;
                tempNgc.acceleration = 500;
                tempNgc.topHood =true;
                tempNgc.bottomHood =true;
                tempNgc.withNotches = false;
                configuredSuccesfuly = true;
            }
        }
        if(tempNgc.action == "topEngr"){
            if (tempNgc.material == "ply"){
                tempNgc.minPower = 47;
                tempNgc.maxPower = 70;
                tempNgc.G1Speed = 10000;
                tempNgc.acceleration = 1000;
                tempNgc.topHood =false;
                tempNgc.bottomHood =false;
                tempNgc.withNotches = false;
                configuredSuccesfuly = true;
            }
        }
        if(tempNgc.action == "mill"){
            if (tempNgc.material == "poly"){
                tempNgc.minPower = 0;
                tempNgc.maxPower = 1;
                tempNgc.topHood =false;
                tempNgc.bottomHood =false;
                tempNgc.withNotches = false;
                configuredSuccesfuly = true;
            }
        }

        if (fields.size() > 4){
            if (fields.size() == 6){
                tempNgc.sizeSet = true;
                tempNgc.width = fields.at(4).toInt();
                tempNgc.height = fields.at(5).toInt();
            } else {
                m_message = "Неправильное количество полей в названии файла " + fileName;
                emit messageChanged(m_message);
                qDebug() << "Wrong fields count in filename " << fileName;
                return 0;
            }
        }

        tempNgc.lastModified = fileInfo.lastModified();

        if (configuredSuccesfuly) {
            m_ngc.append(tempNgc);
        } else {
            m_message = "Неизвестные переменные в имени: " + fileName;
            emit messageChanged(m_message);
            qDebug() << "Wrong fields in filename " << fileName;
            return 0;
        }
    }

    //delete old remote files and merge changes in present files

    for (int i = 0; i < rowCount(); i++){
        bool requireRemoving = true;
        Ngc *localNgc;
        for (int j = 0; j<m_ngc.size();j++){
            localNgc = &m_ngc[j];
            if (record(i).value("name").toString() == localNgc->name &&
                record(i).value("mapType").toString() == localNgc->mapType &&
                record(i).value("material").toString() == localNgc->material &&
                record(i).value("action").toString() == localNgc->action){
                requireRemoving = false;
                localNgc->requireCreating = false;
                bool requireLocalUpdating = false;

                if(record(i).value("lastModified").toDateTime() < localNgc->lastModified){
                    requireLocalUpdating = true;
                }
                if(!(record(i).value("bottomHood").toBool() == localNgc->bottomHood)){
                    if (!updateRecord(i,localNgc->bottomHood,"bottomHood")) return 0;
                }
                if(!(record(i).value("topHood").toBool() == localNgc->topHood)){
                    if (!updateRecord(i,localNgc->topHood,"topHood")) return 0;
                }
                if(!(record(i).value("G1Speed").toInt() == localNgc->G1Speed)){
                    if (!updateRecord(i,localNgc->G1Speed,"G1Speed")) return 0;
                    requireLocalUpdating = true;
                }
                if(!(record(i).value("acceleration").toInt() == localNgc->acceleration)){
                    if (!updateRecord(i,localNgc->acceleration,"acceleration")) return 0;
                    requireLocalUpdating = true;
                }
                if(!(record(i).value("sizeSet").toBool() == localNgc->sizeSet)){
                    if (!updateRecord(i,localNgc->sizeSet,"sizeSet")) return 0;
                }
                if(!(record(i).value("width").toInt() == localNgc->width)){
                    if (!updateRecord(i,localNgc->width,"width")) return 0;
                }
                if(!(record(i).value("height").toInt() == localNgc->height)){
                    if (!updateRecord(i,localNgc->height,"height")) return 0;
                }
                if(!(record(i).value("minPower").toInt() == localNgc->minPower)){
                    if (!updateRecord(i,localNgc->minPower,"minPower")) return 0;
                }
                if(!(record(i).value("maxPower").toInt() == localNgc->maxPower)){
                    if (!updateRecord(i,localNgc->maxPower,"maxPower")) return 0;
                }

                if (requireLocalUpdating) {
                    if(!updateLocalNgcFileProperties(localNgc->rawName,localNgc->G1Speed,
                                                 localNgc->acceleration,localNgc->withNotches))
                        return 0;

                    //QProcess -> to controller if no - return 0

                    QFile f(localNgc->rawName);
                    QFileInfo fileInfo(f);
                    fileInfo.setCaching(false);
                    if (!updateRecord(i,fileInfo.lastModified(),"lastModified")) return 0;
                }
            }
        }
        if (requireRemoving) {
            removeRow(i);
            if (!submitAll()) {
                m_message = "Проблемы с подключением, попробуйте переподключить";
                emit messageChanged(m_message);
                qDebug() << "Row cannot be deleted";
                m_connectionState = false;
                return 0;
            }
        }
    }

    //Create new remote if present
    Ngc *localNgc;
    for (int i = 0; i<m_ngc.size();i++){
        localNgc = &m_ngc[i];
        if (localNgc->requireCreating){
            if (!updateLocalNgcFileProperties(localNgc->rawName,localNgc->G1Speed,
                                         localNgc->acceleration,localNgc->withNotches))
                return 0;

            //QProcess -> to controller if no - return 0

            QFile f(localNgc->rawName);
            QFileInfo fileInfo(f);
            fileInfo.setCaching(false);

            QSqlRecord tempRecord = this->record();
            tempRecord.setValue("name", localNgc->name);
            tempRecord.setValue("lastModified", fileInfo.lastModified());
            tempRecord.setValue("bottomHood", localNgc->bottomHood);
            tempRecord.setValue("topHood", localNgc->topHood);
            tempRecord.setValue("G1Speed", localNgc->G1Speed);
            tempRecord.setValue("acceleration", localNgc->acceleration);
            tempRecord.setValue("sizeSet", localNgc->sizeSet);
            tempRecord.setValue("width", localNgc->width);
            tempRecord.setValue("height", localNgc->height);
            tempRecord.setValue("minPower", localNgc->minPower);
            tempRecord.setValue("maxPower", localNgc->maxPower);
            tempRecord.setValue("mapType", localNgc->mapType);
            tempRecord.setValue("material", localNgc->material);
            tempRecord.setValue("action", localNgc->action);

            insertRecord(-1, tempRecord);
            if (!submitAll()) {
                m_message = "Проблемы с подключением, попробуйте переподключить";
                emit messageChanged(m_message);
                qDebug() << "Row cannot be inserted";
                m_connectionState = false;
                return 0;
            }
        }
    }

    m_message = "Программы обновлены";
    emit messageChanged(m_message);
    emit ngcUpdated();
    return 1;
}

bool NgcQueryModel::updateRecord(int row, const QVariant &value, const QString &name)
{
    QSqlRecord record = this->record(row);
    record.setValue(name, value);
    setRecord(row, record);
    if (!submitAll()) {
        m_message = "Проблемы с подключением, попробуйте переподключить";
        emit messageChanged(m_message);
        qDebug() << "Row cannot be updated";
        m_connectionState = false;
        return 0;
    }
    return 1;
}

bool NgcQueryModel::fetchNgc()
{
    if (!m_connectionState) {
        database().close();
        if (!database().open()) {
            m_message = "Проблемы с подключением, попробуйте переподключить";
            emit messageChanged(m_message);
            qDebug() << "Database cannot open";
            return 0;
        } else {
            m_connectionState = true;
            setTable("active_ngc");
            setEditStrategy(QSqlTableModel::OnManualSubmit);
        }
    }

    if (this->select()) {
        m_message = "";
        emit messageChanged(m_message);
        return 1;
    } else {
        m_message = "Проблемы с подключением, попробуйте переподключить";
        emit messageChanged(m_message);
        qDebug() << "Select query failed";
        m_connectionState = false;
        return 0;
    }
}

QVariant NgcQueryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QVariant value;

    if(role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    }
    else {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    return value;
}

QHash<int, QByteArray> NgcQueryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[name] = "name";
    roles[lastModified] = "lastModified";
    roles[bottomHood] = "bottomHood";
    roles[topHood] = "topHood";
    roles[G1Speed] = "G1Speed";
    roles[acceleration] = "acceleration";
    roles[sizeSet] = "sizeSet";
    roles[width] = "width";
    roles[height] = "height";
    roles[minPower] = "minPower";
    roles[maxPower] = "maxPower";
    roles[mapType] = "mapType";
    roles[material] = "material";
    roles[action] = "action";
    return roles;
}

bool NgcQueryModel::updateLocalNgcFileProperties(QString &fileName, int G1Speed, int acceleration, bool makeJumpers)
{
    QFile f(fileName);
    if(!f.open(QFile::ReadOnly | QFile::Text))
        return 0;
    QStringList fileStrings;

    fileStrings.append(f.readLine());
    fileStrings.append(f.readLine());
    if (fileStrings.first().contains(QString("M204 S%1").arg(acceleration)) &&
        fileStrings.at(1).contains(QString("G1 F%1").arg(G1Speed)))
        return 1;

    if (fileStrings.first().contains("M204") && fileStrings.at(1).contains("G1"))
        fileStrings.clear();

    while(!f.atEnd())
        fileStrings.append(f.readLine());
    f.close();

    fileStrings.prepend(QString("M204 S%1\nG1 F%2\n").arg(acceleration).arg(G1Speed));

    if(!f.open(QFile::WriteOnly | QFile::Text))
        return 0;
    QTextStream out(&f);

    for ( QStringList::Iterator it = fileStrings.begin(); it != fileStrings.end(); ++it )
                    out << *it;
    return 1;
}
