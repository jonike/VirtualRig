#include <QtGui>
#include <QHBoxLayout>
#include <QFileSystemModel>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "skeleton.h"
#include "glarea.h"
#include "VirtualRig.h"
/* \brief:
QT GUI interface
*/
virtua::virtua(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	glArea = new GLArea(this);

	shinSlider = createSlider();
	shinSlider->setValue(0);

	connect(shinSlider, SIGNAL(valueChanged(int)),	glArea,		SLOT(setShininess(int)));
	connect(shinSlider, SIGNAL(valueChanged(int)),	glArea,		SLOT(SetMorph(int)));
	connect(shinSlider, SIGNAL(valueChanged(int)),	glArea,		SLOT(SetFrameNo(int)));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(glArea);
	//mainLayout->addWidget(shinSlider);
	ui.centralWidget->setLayout(mainLayout);

	//	QActionGroup * shadeGroup = new QActionGroup(this);
	//	shadeGroup->addAction(ui.actionFlatShading);
	//	shadeGroup->addAction(ui.actionShader);
	//	shadeGroup->setExclusive(true);
	//ui.actionShader->setChecked(true);
	//connect(shadeGroup, SIGNAL(triggered(QAction *)), this, SLOT(triggered(QAction *)));

	//	ui.actionSkeleton->setChecked(true);
	//	connect(ui.actionSkeleton, SIGNAL(toggled(bool)), this, SLOT(onSkeletonToggled(bool)));

	ui.bvhVisibleCheckBox->setChecked(true);
	connect(ui.bvhVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(onBvhVisibleToggled(bool)));
	ui.SkelVisibleCheckBox->setChecked(true);
	connect(ui.SkelVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(onSkeletonToggled(bool)));

	//connect(ui.xscaleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(xscaleChanged(double)));
	//connect(ui.yscaleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(yscaleChanged(double)));
	//connect(ui.zscaleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(zscaleChanged(double)));

	//connect(ui.xtransSpinBox, SIGNAL(valueChanged(double)), this, SLOT(xtransChanged(double)));
	//connect(ui.ytransSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ytransChanged(double)));
	//connect(ui.ztransSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ztransChanged(double)));
	// linear blend skinnning
//	connect(ui.LBSButton, SIGNAL(clicked(bool)), this, SLOT(LBSClick(bool)));
	
	// tool to play, pause, resart and stop
	connect(ui.buttonPlay, SIGNAL(clicked(bool)), this, SLOT(PlayClick(bool))); //  play animation 
	connect(ui.buttonPause, SIGNAL(clicked(bool)), this, SLOT(PauseClick(bool))); // pause animation 
	connect(ui.buttonOneStep, SIGNAL(clicked(bool)), glArea, SLOT(oneTimer())); // one step of animation 
	//connect(ui.buttonStop, SIGNAL(clicked(bool)), glArea, SLOT(resetTimer())); // pause animation 
     connect(ui.Tpose, SIGNAL(clicked(bool)), glArea, SLOT(Tpose())); // pause animation 
	connect(ui.Stiffness_str, SIGNAL(valueChanged(int)),	glArea,SLOT(SetStiffnessstr(int)));
	// save obj files
	connect(ui.ObjButton, SIGNAL(clicked(bool)), this, SLOT(SaveObjClick(bool)));

	//ui.xscaleSpinBox->setValue(glArea->bvh.scalex);
	//ui.yscaleSpinBox->setValue(glArea->bvh.scaley);
	//ui.zscaleSpinBox->setValue(glArea->bvh.scalez);

	//ui.xtransSpinBox->setValue(glArea->bvh.transx);
	//ui.ytransSpinBox->setValue(glArea->bvh.transy);
	//ui.ztransSpinBox->setValue(glArea->bvh.transz);
	// load bvh file
	InitBvhFileList();
	connect(ui.bvhListView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClicked(const QModelIndex &)));
	connect(glArea, SIGNAL(bvhLoaded()), this, SLOT(onBvhLoaded()));
	// load mesh 
	InitMeshFileList();
	// activate the loaded mesh
	connect(ui.modelComboBox, SIGNAL(activated(QString)), glArea, SLOT(loadMesh(QString)) );  // load mesh which load the teterhderal mesh
     connect(ui.modelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectModelFromCombobox(int)) );
	//BVHComboBox
	InitBVHFileCom();
	// activate the loaded bvh
	//connect(ui.BVHComboBox, SIGNAL(activated(QString)), glArea, SLOT(LoadBvh(QString)) );  // load mesh which load the teterhderal mesh
     //connect(ui.BVHComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectbvhFromCombobox(int)) );

	////// on Error
	ui.ErrorVisibleCheckBox->setChecked(false);
	connect(ui.ErrorVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(onErrorVisibleToggled(bool)));

	ui.WireVisibleCheckBox->setChecked(false);
	connect(ui.WireVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(onWireVisibleToggled(bool)));

	setWindowTitle(tr("Virtua"));

}

void virtua::heatWeightsClick(bool)
{
	//	glArea->model.HeatWeights();
}

void virtua::nearestBoneWeightsClick(bool)
{
	//glArea->model.NearestBoneWeights();
}

void virtua::smoothWeightsClick(bool)
{
	//glArea->model.SmoothWeights();
}
void virtua::LBSClick(bool)
{
	glArea->on_LBS= true;
	cout << "linear skin "; 
	glArea->PlayAnimation();
}
void virtua::onBvhVisibleToggled(bool b)
{
	glArea->show_bvh = b;
}
void virtua::onWireVisibleToggled(bool b)
{
	glArea->show_Wire = b;
}
void virtua::onErrorVisibleToggled(bool b)
{
	glArea->show_Error = b;
}
void virtua::PlayClick(bool)
{
	// here I should start play the motion but something going wrong
	glArea->model.skeleton->InitLocalChilds();
	glArea->playing=!glArea->playing;
	glArea->PlayAnimation();
	
}
void virtua::PauseClick(bool)
{
	//cout << " here stop playing ";
	glArea->model.skeleton->InitLocalChilds();
	glArea->playing=false ;
	glArea->on_animation=false ;
	//ui.PlayAnimationButton->setText("Play Animation");
	
}

// stop the animation 
/*void virtua::StopClick(bool)
{
	cout << " here stop playing ";
	glArea->playing=false ;
	glArea->on_animation=false ;
	//ui.PlayAnimationButton->setText("Play Animation");
	
}
void virtua::SaveObjClick(bool)]

void virtua::OneStepClick(bool)
{


}*/
// play and Stop button
// save obj file
void virtua::SaveObjClick(bool)
{
	glArea->model.Saveobj();
}
void virtua::PlayAnimationClick(bool)
{
/*	cout << " here start playing ";
	glArea->model.skeleton->InitLocalChilds();
	if(ui.PlayAnimationButton->text()=="Play Animation")
	{
		glArea->playing=!glArea->playing;
		glArea->PlayAnimation();
		ui.PlayAnimationButton->setText("Stop Animation");
	}
	else
	{

		glArea->playing=false ;
		glArea->on_animation=false ;
		ui.PlayAnimationButton->setText("Play Animation");
	}
	//glArea->model.skeleton->Assign(glArea->model.meshes[0]);*/
}

void virtua::xscaleChanged(double d)
{
	glArea->bvh.scalex = d;
}

void virtua::yscaleChanged(double d)
{
	glArea->bvh.scaley = d;
}

void virtua::zscaleChanged(double d)
{
	glArea->bvh.scalez = d;
}

void virtua::xtransChanged(double d)
{
	glArea->bvh.transx = d;
}

void virtua::ytransChanged(double d)
{
	glArea->bvh.transy = d;
}

void virtua::ztransChanged(double d)
{
	glArea->bvh.transz = d;
}

void virtua::onSkeletonToggled(bool b)
{
	glArea->model.skeleton->is_visible = b;
}

void virtua::triggered(QAction* action)
{
	if (action->text() == "Flat")
		glArea->selectDrawMode(GLArea::FLATWIRE);
	if (action->text() == "Shader")
		glArea->selectDrawMode(GLArea::SHADER);
	//	cout << action->text().toStdString() << endl;
}

QSlider* virtua::createSlider()
{
	QSlider *slider = new QSlider(Qt::Horizontal);
	//    slider->setRange(0, glArea->model.GetNbMorphs() - 1);
	slider->setRange(0, glArea->bvh.GetNumFrame() - 1);
	slider->setSingleStep(1);
	slider->setPageStep(10);
	slider->setTickInterval(10);
	slider->setTickPosition(QSlider::TicksAbove);
	return slider;
}

void virtua::onBvhLoaded()
{
	cout << " here wen";
	shinSlider->setRange(0, glArea->bvh.GetNumFrame() - 1);
}

void virtua::onMeshLoaded()
{
	//shinSlider->setRange(0, glArea->bvh.GetNumFrame() - 1);
}

void virtua::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
		close();
	else
		QWidget::keyPressEvent(e);
}
// load bvh
void virtua::InitBvhFileList()
{
	QFileSystemModel *model = new QFileSystemModel;
	model->setRootPath(QDir::currentPath() + "/bvh");
	ui.bvhListView->setModel(model);
	ui.bvhListView->setRootIndex(model->index(QDir::currentPath() + "/bvh"));
}
static int addItemsToComboBoxFromFiles(QComboBox *cb, QString folder, QString subfolder, QString initValue){
    QDir dir(folder+subfolder);
    dir.setNameFilters(QStringList( "*.mesh" ));

    int count = 0;

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QString val = list.at(i).baseName();
        cb->addItem( val , folder );
        if (QString::compare(val, initValue, Qt::CaseInsensitive)==0) {
            cb->setCurrentIndex( cb->count()-1 );
        }
        count++;
    }
    return count;
}
static int addItemsbvhToComboBoxFromFiles(QComboBox *cb, QString folder, QString subfolder, QString initValue){
    QDir dir(folder+subfolder);
    dir.setNameFilters(QStringList( "*.bvh" ));

    int count = 0;

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QString val = list.at(i).baseName();
        cb->addItem( val , folder );
        if (QString::compare(val, initValue, Qt::CaseInsensitive)==0) {
            cb->setCurrentIndex( cb->count()-1 );
        }
        count++;
    }
    return count;
}
// load bvh to com
void virtua ::InitBVHFileCom()
{
 QString folder="../../virtua/data/";
    QDir dir(folder);
    dir.setFilter( QDir::Dirs );
    dir.setNameFilters(QStringList( "*" ));
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
    QString val = list.at(i).baseName() + "/";
    int k = addItemsbvhToComboBoxFromFiles( ui.BVHComboBox, folder+val, "Animation/",  "LongWalk" );
 }
}
// load mesh 
void virtua::InitMeshFileList()
{
    
    QString folder="../../virtua/data/";
    QDir dir(folder);
    dir.setFilter( QDir::Dirs );
    dir.setNameFilters(QStringList( "*" ));
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
    QString val = list.at(i).baseName() + "/";
    int k = addItemsToComboBoxFromFiles( ui.modelComboBox, folder+val, "mesh/",  "Slim" );
     }
}
void virtua::doubleClicked(const QModelIndex & index)
{
	QFileSystemModel *model = (QFileSystemModel *)ui.bvhListView->model();
	//QFileInfo	fileInfo ( const QModelIndex & index ) const
	QString	fileName = model->fileName(index);
	//cout << "selected bvh: " << fileName.toStdString().c_str() << endl;
	QString	filePath = model->filePath(index);
	glArea->bvh.Clear();
	glArea->LoadBvh(filePath);
}
// when ever I change the mesh 
void virtua::selectModelFromCombobox(int newIndex)
{
    QString newDataPath = ui.modelComboBox->itemData(newIndex).toString();
    //glArea->loadMesh(newDataPath);
    //glArea->model.skeleton->InitLocalChilds();
    //onBvhLoaded();
    //glArea->LoadBvh("01_01.bvh");
    

}
virtua::~virtua()
{

}
