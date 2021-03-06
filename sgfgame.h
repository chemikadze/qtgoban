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
	enum Error { ENo, EBadAttrName, EBadSyntax, EWrongGM, EUnknownEncoding, EInvalidPoint, EBadAttrValue };
	enum MoveError {MENo, MESuicide, MEKo};
	enum CellMark { CMNone = 0x0, CMDimm = 0x1, CMInvisible = 0x2 };
	enum Rules { JapaneseRules, ChineseRules };

protected:
	QVector <int> m_killed; // killed (dead) stones count
	QVector <int> m_square;	// square allocated
	QVector <QVector <Color> > m_board;
	QVector <QVector <qint8> > m_cellVisible; // CellMark, but need bit operations
	QVector <QList <SgfVariant> > m_viewStack;
	QVector <QPair <qint16, QSet<Stone> > > m_rewriteStack;
	QVector <QPair <qint16, QSet<Stone> > > m_killStack;
	QVector <QVector <Color> > m_territory;
	QList <Mark> m_marks;
	SgfTree *m_tree;
	SgfTree *m_current;
	QFile *m_io;
	QByteArray m_buffer;
	QString m_encodedBuffer;
	QTextCodec *codec;
	bool writeNode(SgfTree *node);
	Error m_error;
	Color m_turn;
	Rules m_rules;

	// TODO: will be deleted when will be used fill MVC
	QVector <Line> m_lines;
	QVector <Label> m_labels;

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

//	IO
	SgfTree* readNodeFromBuffer(SgfTree *parent=NULL);
	void writeNodeToBuffer(SgfTree *node);

	void emitError(Error errcode);
	void emitMoveError(MoveError errcode);

//	Moving through tree
	void stepForward(SgfTree *next);
	void stepBackward();

	void setLabels(SgfTree* node);
	void setMarks(SgfTree* node);
	void setLines(SgfTree* node);

// Board manipulating
	int fillGroup(qint8 col, qint8 row, Color color);
	bool isDead(qint8 col, qint8 row);
	void setKills(SgfTree* node);
	void validateAndAddKilled(SgfTree *node, qint8 col, qint8 row, const Color color);
	void clearState();
	bool setStone(Point p, Color color, bool force = false);
	bool setStone(qint8 col, qint8 row, Color color, bool force = false);

//	Viewport changing, VW property
	void setView(QList <SgfVariant> regionList);

signals:
	void wrongValue(QString attrName, QString dataString);
	void errorOccured(Error errorcode);
	void errorOccured(QString s);
	void nodeRemoved(SgfTree* newCurr);
	void currentNodeChanged(SgfTree* current);
	//void message(QString);
	void boardChanged();
	void moveErrorOccured(MoveError errcode);
	void moveErrorOccured(QString s);
	void gameTreeChanged(SgfTree* root);
	void turnChanged(Color turn);
	void nodeAttributesChanged();

public slots:
	void setTurn(Color turn);

public:
//	Board API

// stones
	bool rules();
	bool makeMove(qint8 col, qint8 row);
	bool canMove(qint8 col, qint8 row);
	bool addStone(qint8 col, qint8 row, Color color);
	bool addStone(Stone stone);
	inline Color stone(char col, char row) { return m_board[row][col]; }
	inline const QVector < QVector<Color> >& board() { return m_board; }

// markup
	void addMark(qint8 col, qint8 row, Markup m);
	//inline Markup markup(qint8 col, qint8 row) { return m_markup[row][col]; }
	QList <Mark> marks()const { return m_marks; }
	inline QVector<QVector<qint8> > cellVisibleStates() { return m_cellVisible; }
	void addLine(Line ln);
	void removeLine(Point p);
	inline QVector<Line> lines() { return m_lines; };
	void addLabel(Label lbl);
	inline QVector<Label> labels() { return m_labels; }
	QString labelAt(Point p);
	void removeLabel(Point p);

// scoring
	void markTerritory();
	void markTerritory(Point p);
	void unmarkTerritory(Point p);
	int score(Color c) { return killed(c)+square(c); }
	int killed(Color c) { return m_killed.at(c); }
	int square(Color c) { return m_square.at(c); }
	void setTerritory(Stone s);
	QList <Point> terrBlack() { return m_current->terrBlack(); };
	QList <Point> terrWhite() { return m_current->terrWhite(); };
	QVector <QVector <Color> > territory() { return m_territory; }
	Color borderColors(Point pnt);
	void setTerritories(SgfTree *node);

// setticg current position
	bool setCurrentMove(SgfTree* newCurr);

// error handling
	inline Error error() { return m_error; }
	inline QString errorToString(Error e) { return m_errorStrings.value(e); }
	inline QString moveErrorToString(MoveError e) { return m_moveErrorStrings.value(e); }

	inline SgfTree *tree() { return m_tree; }
	inline SgfTree *currentMove() { return m_current; }

	inline const QString& encoding()const { return m_encoding; }

	inline QSize size()const { return m_size; }

	inline Color turn() { return m_turn; }

	bool validatePoint(qint8 col, qint8 row);
	bool validatePoint(Point point);

// board manpulating
	void resize(QSize s);
	void resize(qint8 col, qint8 row = -1);
	void setEncoding(QString encoding);
	void removeNode(SgfTree *node);

//	node comments
	void setRootAttr(const QString& attr, const SgfVariant& data);
	inline QString comment() { return m_current->attrValue("C").toString(); }
	inline void setComment(const QString& comment) { m_current->setAttribute("C", comment); }
	inline QString nodeName() { return m_current->attrValue("N").toString(); }
	inline void setNodeName(const QString& name) { m_current->setAttribute("N", name); }

	QFile::FileError readBufferFromFile(const QString& filename);
	QString readEncodingFromBuffer();
	inline void clearBuffer() { m_buffer.clear(); }
	void encodeBuffer();
	bool readGameFromBuffer();
	QFile::FileError saveToFile(const QString& filename);
	SgfVariant strToAttrValue(const QString& attr, const QString& data);

	SgfGame(QObject *p = 0, QSize size = QSize(19, 19), Rules rls = JapaneseRules);
	~SgfGame();
};


#endif // SGFGAME_H
