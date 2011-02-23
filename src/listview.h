#include <QtGui> 
 
class ListView : public QListView
{
public:    
	ListView() : QListView() { mySize = QSize(42,42); }      
 
	QSize sizeHint() const { return mySize; }
        void setSizeHint( QSize s  ) { mySize = s; } 

private:
	QSize mySize;
};
