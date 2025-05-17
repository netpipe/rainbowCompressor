#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QByteArray>
#include <QDebug>
#include <QCommandLineParser>
#include <QCommandLineOption>

QMap<QByteArray, QByteArray> buildTrigramDictionary(const QByteArray &input, int maxTokens = 255) {
    QMap<QByteArray, int> freq;
    for (int i = 0; i < input.size() - 2; ++i) {
        QByteArray trigram = input.mid(i, 3);
        freq[trigram]++;
    }

    // Sort by frequency
    QList<QPair<QByteArray, int>> freqList = freq.toStdMap().toStdVector().toList();
    std::sort(freqList.begin(), freqList.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });

    // Build dictionary: trigram → byte
    QMap<QByteArray, QByteArray> dict;
    for (int i = 0; i < qMin(maxTokens, freqList.size()); ++i) {
        QByteArray token(1, static_cast<char>(i + 1)); // avoid null byte
        dict[freqList[i].first] = token;
    }
    return dict;
}

QByteArray compress(const QByteArray &input, const QMap<QByteArray, QByteArray> &dict) {
    QByteArray output;
    int i = 0;
    while (i < input.size()) {
        if (i + 3 <= input.size()) {
            QByteArray trigram = input.mid(i, 3);
            if (dict.contains(trigram)) {
                output.append(dict[trigram]);
                i += 3;
                continue;
            }
        }
        output.append(input[i]);
        i++;
    }
    return output;
}

QByteArray decompress(const QByteArray &compressed, const QMap<QByteArray, QByteArray> &dict) {
    // Reverse dictionary
    QMap<QByteArray, QByteArray> reverseDict;
    for (auto it = dict.begin(); it != dict.end(); ++it)
        reverseDict[it.value()] = it.key();

    QByteArray output;
    for (int i = 0; i < compressed.size(); ++i) {
        QByteArray token(1, compressed[i]);
        if (reverseDict.contains(token)) {
            output.append(reverseDict[token]);
        } else {
            output.append(compressed[i]);
        }
    }
    return output;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("RainbowCompressor");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Rainbow Table Compressor (Trigram-Based)");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("input", "Input file to compress.");
    parser.addPositionalArgument("output", "Output file to write.");

    QCommandLineOption decompressOpt(QStringList() << "d" << "decompress", "Decompress instead of compress.");
    parser.addOption(decompressOpt);

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 2) {
        qCritical() << "Usage: rainbowcompress <input> <output> [-d]";
        return 1;
    }

    QFile inputFile(args[0]);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open input file.";
        return 1;
    }

    QByteArray inputData = inputFile.readAll();
    inputFile.close();

    QFile outputFile(args[1]);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        qCritical() << "Failed to open output file.";
        return 1;
    }

    bool decompressMode = parser.isSet(decompressOpt);
    if (decompressMode) {
        // Load dictionary from sidecar file
        QFile dictFile(args[1] + ".dict");
        if (!dictFile.open(QIODevice::ReadOnly)) {
            qCritical() << "Missing dictionary file for decompression.";
            return 1;
        }

        QDataStream ds(&dictFile);
        QMap<QByteArray, QByteArray> dict;
        ds >> dict;
        dictFile.close();

        QByteArray outputData = decompress(inputData, dict);
        outputFile.write(outputData);
    } else {
        QMap<QByteArray, QByteArray> dict = buildTrigramDictionary(inputData);
        QByteArray compressed = compress(inputData, dict);
        outputFile.write(compressed);

        // Save dictionary for decompression
        QFile dictFile(args[1] + ".dict");
        if (dictFile.open(QIODevice::WriteOnly)) {
            QDataStream ds(&dictFile);
            ds << dict;
            dictFile.close();
        }
    }

    outputFile.close();
    qDebug() << (decompressMode ? "Decompression" : "Compression") << "done.";
    return 0;
}
