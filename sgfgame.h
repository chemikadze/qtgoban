/*

  Game structure, SGF-based, provides game in tree.
  To read from file:
	loadBufferFromFile()
	readEncodingFromBuffer() or setEncoding()
	readGameFromBuffer()

*/

#ifndef SGFGAME_H
#define SGFGAME_H

#include <QtCore/QVector>
#include <QtCore/QFile>
#include <QtCore/QPair>
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QSize>
#include <QtCore/QObject>
#include "common.h"
#include "sgftree.h"
#include "sgfvariant.h"

class SgfGame : public QObject
{
	Q_OBJECT

public:
	enum Error { ENo, EBadAttrName, EBadSyntax, EWrongGM, EUnknownEncoding };


protected:
	QVector <QVector <StoneColor> > m_board;
	SgfTree *m_tree;
	SgfTree *m_current;
	QFile *m_io;
	QByteArray m_buffer;
	QString m_encodedBuffer;
	QTextCodec *codec;
	bool writeNode(SgfTree *node);
	Error m_error;
	Turn m_turn;
	static const QMap <Error,QString> m_errorStrings;
	static const QMap <QString, SgfVariant::Type> m_typeMap;
	static QMap <QString, SgfVariant::Type> createSgfTypeMap();
	static QMap <Error, QString> createErrorStringsMap();
	bool isRootAttr(const QString& s);
	QPair <QString,QString> splitCompose(const QString& s);
// root attrs
	QSize m_size;
	QString m_encoding;
	qint8 m_st;

	SgfTree* readNodeFromBuffer(SgfTree *parent=NULL);
	void writeNodeToBuffer(SgfTree *node);
	void emitError(Error errcode);
	QString errorToString(Error errcode);

signals:
	void wrongValue(QString attrName, QString dataString);
	void errorOccured(Error errorcode);

public:
	inline StoneColor stone(char i, char j);
  /*inline*/ SgfTree *tree();
	inline SgfTree *currentMove();
	inline const QString& encoding()const;
	inline QSize size()const;
	void resize(QSize s);
	void resize(qint8 col, qint8 row = -1);
	void setEncoding(QString encoding);
	bool makeMove(qint8 col, qint8 row); // realization
	bool moveIsCorrect(qint8 col, qint8 row);
	inline Turn turn();

	QFile::FileError loadBufferFromFile(const QString& filename);
	QString readEncodingFromBuffer();
	void encodeBuffer();
	bool readGameFromBuffer();
	QFile::FileError saveToFile(const QString& filename);
	SgfVariant strToAttrValue(const QString& attr, const QString& data);
	void setRootAttr(const QString& attr, const SgfVariant& data);

	SgfGame(int size = 19);
	~SgfGame();
};

#endif // SGFGAME_H