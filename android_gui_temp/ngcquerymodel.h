#ifndef NGCQUERYMODEL_H
#define NGCQUERYMODEL_H

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QDateTime>
#include <QSqlQuery>
#include <QDirIterator>

struct Ngc {
    QString name;
    QString rawName;
    QDateTime lastModified;
    bool bottomHood;
    bool topHood;
    int G1Speed;
    int acceleration;
    bool sizeSet;
    int width;
    int height;
    int minPower; // x100
    int maxPower; // x100
    QString mapType;
    QString material;
    QString action;
    bool withNotches;
    bool requireCreating;
};


class NgcQueryModel : public QSqlTableModel
{
    Q_OBJECT
public:
    enum Fields {
        name,
        lastModified,
        bottomHood,
        topHood,
        G1Speed,
        acceleration,
        sizeSet,
        width,
        height,
        minPower,
        maxPower,
        mapType,
        material,
        action
    };

    explicit NgcQueryModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

    Q_INVOKABLE bool updateNgc();
    Q_INVOKABLE bool fetchNgc();

    bool updateRecord(int row, const QVariant &value, const QString &name);
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void ngcUpdated();
    void messageChanged(QString msg);

private:
    bool updateLocalNgcFileProperties(QString &fileName, int G1Speed, int acceleration, bool makeJumpers);

    QVector<Ngc> m_ngc;

    QString m_message;
    bool m_connectionState;
    QHash<int, QByteArray> m_roleNames;
};

#endif // NGCQUERYMODEL_H
