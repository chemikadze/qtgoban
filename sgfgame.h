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
	// all except EBadSyntax and WrongGM are not fatal
	enum Error { ENo, EBadAttrName, EBadSyntax, EWrongGM, EUnknownEncoding, EInvalidPoint };
	enum MoveError {MENo, MESuicide, MEKo};
	QVector <int> m_killed;
	QVector <int> m_square;

protected:
	QVector <QVector <StoneColor> > m_board;
	QVector <QVector <bool> > m_markup;
	SgfTree *m_tree;
	SgfTree *m_current;
	QFile *m_io;
	QByteArray m_buffer;
	QString m_encodedBuffer;
	QTextCodec *codec;
	bool writeNode(SgfTree *node);
	Error m_error;
	StoneColor m_turn;
	QString m_comment;

	static const QHash <Error,QString> m_errorStrings;
	static const QHash <QString, SgfVariant::Type> m_typeHash;
	static const QHash <MoveError, QString> m_moveErrorStrings;

	static QHash <QString, SgfVariant::Type> createSgfTypeHash();
	static QHash <Error, QString> createErrorStringsHash();
	static QHash <MoveError, QString> createMoveErrorHash();

	bool isRootAttr(const QString& s);
	QPair <QString,QString> splitCompose(const QString& s);
// root attrs
	QSize m_size;
	QString m_encoding;
	qint8 m_st;

	SgfTree* readNodeFromBuffer(SgfTree *parent=NULL);
	void writeNodeToBuffer(SgfTree *node);
	void emitError(Error errcode);
	void emitMoveError(MoveError errcode);
	QString ittegularMoveToString(MoveError errcode);

	bool isDead(qint8 col, qint8 row);
	void setKills(SgfTree* node);
	void validateAndAddKilled(SgfTree *node, qint8 col, qint8 row, const StoneColor color);

	void stepForward(SgfTree *next);
	void stepBackward();

	bool setStone(Point p, StoneColor color, bool force = false);
	bool setStone(qint8 col, qint8 row, StoneColor color, bool force = false);

	int fillGroup(qint8 col, qint8 row, StoneColor color);

signals:
	void wrongValue(QString attrName, QString dataString);
	void errorOccured(Error errorcode);
	void errorOccured(QString s);
	void newNode(SgfTree* newCurr);
	void currentNodeChanged(SgfTree* current);
	//void message(QString);
	void moveErrorOccured(MoveError errcode);
	void moveErrorOccured(QString s);

public:

	bool setCurrentMove(SgfTree* newCurr);

	inline Error error() { return m_error; }
	inline QString errorToString(Error e) { return m_errorStrings.value(e); }
	inline QString moveErrorToString(MoveError e) { return m_moveErrorStrings.value(e); }

	inline StoneColor stone(char col, char row) { return m_board[row][col]; }
	inline const QVector < QVector<StoneColor> >& board() { return m_board; }

	inline SgfTree *tree() { return m_tree; }
	inline SgfTree *currentMove() { return m_current; }

	inline const QString& encoding()const { return m_encoding; }

	inline QSize size()const { return m_size; }

	inline StoneColor turn() { return m_turn; }

	bool validatePoint(qint8 col, qint8 row);
	bool validatePoint(Point point);

	void setMarkup(qint8 col, qint8 row, Markup m);

	void resize(QSize s);
	void resize(qint8 col, qint8 row = -1);
	void setEncoding(QString encoding);
	bool makeMove(qint8 col, qint8 row	);
	bool canMove(qint8 col, qint8 row);

	inline const QString& comment() { return m_comment; }
	void setComment(const QString& comment);

	QFile::FileError loadBufferFromFile(const QString& filename);
	QString readEncodingFromBuffer();
	void encodeBuffer();
	bool readGameFromBuffer();
	QFile::FileError saveToFile(const QString& filename);
	SgfVariant strToAttrValue(const QString& attr, const QString& data);
	void setRootAttr(const QString& attr, const SgfVariant& data);

	SgfGame(QSize size = QSize(19, 19));
	~SgfGame();
};

#endif // SGFGAME_H
