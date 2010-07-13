#include "newgamedialog.h"

NewGameDialog::NewGameDialog(QWidget *parent) :
    QDialog(parent)
{
	QStringList list;
	list << "9" << "13" << "19";
	m_width = new QComboBox(this);
	m_height = new QComboBox(this);
	m_width->addItems(list);
	m_height->addItems(list);
	m_width->setEditable(true);
	m_height->setEditable(true);
	m_width->lineEdit()->setValidator(new QIntValidator(1, 52, m_width->lineEdit()));
	m_height->lineEdit()->setValidator(m_width->validator());

	m_bbox = new QDialogButtonBox(Qt::Horizontal, this);
	connect(m_bbox->addButton(QDialogButtonBox::Ok),
			SIGNAL(clicked()),
			this,
			SLOT(accept()));
	connect(m_bbox->addButton(QDialogButtonBox::Cancel),
			SIGNAL(clicked()),
			this,
			SLOT(reject()));

	QFrame *frame = new QFrame(this);
	QGridLayout *fla = new QGridLayout(frame);
	fla->addWidget(new QLabel(tr("Width"), this), 0, 0);
	fla->addWidget(new QLabel(tr("Height"), this), 0, 1);
	fla->addWidget(m_width, 1, 0);
	fla->addWidget(m_height, 1, 1);
	frame->setLayout(fla);

	QVBoxLayout *la = new QVBoxLayout(this);
	la->addWidget(frame);
	la->addWidget(m_bbox);
}

QSize NewGameDialog::boardSize()
{
	return QSize(m_width->currentText().toInt(),
				 m_height->currentText().toInt());
}


