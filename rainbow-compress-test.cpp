#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QStringList>
#include <QDebug>

QMap<QString, QString> buildDictionary() {
    QMap<QString, QString> dict;
    dict["the"] = "A";
    dict["and"] = "B";
    dict["compression"] = "C";
    dict["algorithm"] = "D";
    dict["data"] = "E";
    dict["rainbow"] = "F";
    dict["table"] = "G";
    // Add more entries here...
    return dict;
}

QString compressText(const QString &input, const QMap<QString, QString> &dict) {
    QString output = input;
    for (auto it = dict.begin(); it != dict.end(); ++it) {
        output.replace(it.key(), it.value());
    }
    return output;
}

QString decompressText(const QString &input, const QMap<QString, QString> &dict) {
    QString output = input;
    for (auto it = dict.begin(); it != dict.end(); ++it) {
        output.replace(it.value(), it.key());
    }
    return output;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QMap<QString, QString> dict = buildDictionary();

    // Sample input (you can load from file)
    QString original = "the compression algorithm and the rainbow table work on the data";

    qDebug() << "Original Text:\n" << original;

    QString compressed = compressText(original, dict);
    qDebug() << "\nCompressed:\n" << compressed;

    QString decompressed = decompressText(compressed, dict);
    qDebug() << "\nDecompressed:\n" << decompressed;

    // Optional: Save compressed data to file
    QFile file("compressed.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << compressed;
        file.close();
        qDebug() << "\nCompressed data written to 'compressed.txt'";
    }

    return 0;
}
