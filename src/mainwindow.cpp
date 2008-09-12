#include <QtGui>

#include <iostream>
#include <iomanip>
#include "mainwindow.h"
#include "drawingdisplay.h"
#include "drawingcanvas.h"
#include "defines.h"


MainWindow::MainWindow(FileParser *parser_in):
	parser(parser_in)
{
    drawingInfo = new DrawingInfo();
    canvas = new DrawingCanvas(itemMenu, drawingInfo, parser);
	// Selecting items causes an update of the menus to reflect the current
	// selected items' settings
    connect(canvas, SIGNAL(selectionChanged()), this, SLOT(updateMenus()));

    createActions();
    createToolBox();
    createMenus();
    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    view = new DrawingDisplay(canvas, drawingInfo);
    view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    view->setGeometry(0, 0, DEFAULT_SCENE_SIZE_X, DEFAULT_SCENE_SIZE_Y);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    toolBox->setGeometry(0, 0, DEFAULT_TOOLBOX_WIDTH, DEFAULT_SCENE_SIZE_Y);
    toolBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	drawingInfo->setHeight(view->sceneRect().height());
	drawingInfo->setWidth(view->sceneRect().width());
	drawingInfo->determineScaleFactor();
	canvas->setSceneRect(view->sceneRect());
	canvas->refresh();
	
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(view);
    splitter->addWidget(toolBox);
    layout->addWidget(splitter);
    
    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    loadFile();
    
}


//void MainWindow::periodicTableButtonGroupClicked(QAbstractButton *button)
//{
//    canvas->setElementToAdd(button->text());
//    canvas->setMode(DrawingCanvas::InsertAtom);
//    canvas->update();
//    view->update();
//}

void MainWindow::insertTextAtCursor(QAction *action)
{
	foreach(QGraphicsItem* item,canvas->items()){
		if(ITEM_IS_LABEL){
			Label *label = dynamic_cast<Label*>(item);
			if(label->textInteractionFlags() & Qt::TextEditorInteraction){
				QTextCursor cursor = label->textCursor();
				cursor.insertText(action->iconText());
			}
		}
	}	
	update();
}


void MainWindow::setTextBoxFonts()
{
	// Start by looking to see if one of the labels is being edited
	bool hasLabel = false;
	Label *labelBeingEdited = 0;
	foreach(QGraphicsItem* item,canvas->items()){
		if(ITEM_IS_LABEL){
			hasLabel = true;
			Label *label = dynamic_cast<Label*>(item);
			if(label->textInteractionFlags() & Qt::TextEditorInteraction){
				labelBeingEdited = label;
				break;
			}
		}
	}
	
	// Nothing to edit - bail now
	if(!hasLabel) return;
	
	if(labelBeingEdited!=0){
		// One of the labels is being edited - operate only on this selection
		QTextCursor cursor = labelBeingEdited->textCursor();
		QTextCharFormat format(cursor.blockCharFormat());
		format.setFontWeight((boldTextButton->isChecked() ? QFont::Bold : QFont::Normal));
		format.setFontItalic(italicTextButton->isChecked());
		format.setFontUnderline((underlineTextButton->isChecked() ?
				                 QTextCharFormat::SingleUnderline : QTextCharFormat::NoUnderline));
		format.setFontFamily(textFontCombo->currentFont().family());
	    cursor.mergeCharFormat(format);
	}else{
		// No editor - operate on selected text boxes
		foreach(QGraphicsItem* item,canvas->items()){
			if(ITEM_IS_LABEL){
				Label *label = dynamic_cast<Label*>(item);	
				if(label->isSelected()){
					QFont myFont(label->font());
					myFont.setUnderline(underlineTextButton->isChecked());
					myFont.setItalic(italicTextButton->isChecked());
					myFont.setWeight((boldTextButton->isChecked() ? QFont::Bold : QFont::Normal));
					myFont.setFamily(textFontCombo->currentFont().family());
					label->setFontSize(textFontSizeCombo->currentText().toInt());
					label->setFont(myFont);
				}
			}
		}		
	}
	update();
}


void MainWindow::openFile()
{
     QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
   	 parser->setFileName(fileName);
     loadFile();
}


void MainWindow::loadFile()
{
	
	if (!parser->fileName().isEmpty()) {
	    parser->readFile();
	    canvas->clearAll();
	    canvas->loadFromParser();
	    setWindowTitle(tr("%1 - cheMVP").arg(parser->fileName()));
	}
}


void MainWindow::deleteItem()
{
    foreach (QGraphicsItem *item, canvas->selectedItems()) {
        canvas->removeItem(item);
    }
}


void MainWindow::mouseModeButtonGroupClicked(int buttonID)
{
	// This is just in case the mode was changed automatically by the canvas
	foreach(QAbstractButton *button, mouseModeButtonGroup->buttons()){
		if(mouseModeButtonGroup->id(button) == buttonID){
			button->setChecked(true);
		}
	}
	
    canvas->setMode(DrawingCanvas::Mode(mouseModeButtonGroup->checkedId()));
    if(mouseModeButtonGroup->checkedId() == DrawingCanvas::Rotate){
    	canvas->setAcceptsHovers(false);
    }else{
    	canvas->setAcceptsHovers(true);
    }
}


void MainWindow::createToolBox()
{
//    QWidget *builderWidget 		= createBuilderWidget();
    QWidget *appearanceWidget 	= createAppearanceWidget();
    QWidget *bondsWidget        = createBondsWidget();
    QWidget *anglesWidget       = createAnglesWidget();
    QWidget *atomsWidget        = createAtomsWidget();
    QWidget *annotationWidget   = createAnnotationWidget();

    toolBox = new QToolBox;
//    toolBox->addItem(builderWidget, tr("Builder"));
    toolBox->addItem(anglesWidget, tr("Angles"));
    toolBox->addItem(annotationWidget, tr("Annotation"));
    toolBox->addItem(appearanceWidget, tr("Appearance"));
    toolBox->addItem(atomsWidget, tr("Atoms"));
    toolBox->addItem(bondsWidget, tr("Bonds"));
}


//QWidget *MainWindow::createBuilderWidget()
//{
//    QGridLayout *layout = new QGridLayout;
//
//    periodicTableButtonGroup = new QButtonGroup;
//    connect(periodicTableButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)),
//                this, SLOT(periodicTableButtonGroupClicked(QAbstractButton *)));
//    layout->addWidget(createPeriodicTable(), 0, 0);
//    layout->setRowStretch(3, 10);
//    layout->setColumnStretch(2, 10);
//
//    
//    QWidget *widget = new QWidget;
//    widget->setLayout(layout);
//
//    return widget;	
//}


void MainWindow::setXRotation(int phi)
{
	xLabel->setText(QString::number(phi));
	xSlider->setValue(phi);
	drawingInfo->setXRot(phi);
	canvas->refresh();
}


void MainWindow::setYRotation(int phi)
{
	yLabel->setText(QString::number(phi));	
	ySlider->setValue(phi);
	drawingInfo->setYRot(phi);
	canvas->refresh();
}


void MainWindow::setZRotation(int phi)
{
	zLabel->setText(QString::number(phi));	
	zSlider->setValue(phi);
	drawingInfo->setZRot(phi);
	canvas->refresh();
}


QWidget *MainWindow::createAppearanceWidget()
{
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    
    // The Orientation Sliders
    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();

    QLabel *xTitle = new QLabel("x");
    QLabel *yTitle = new QLabel("y");
    QLabel *zTitle = new QLabel("z");
    xLabel = new QLabel();
    yLabel = new QLabel();
    zLabel = new QLabel();
    setXRotation(0);
    setYRotation(0);
    setZRotation(0);

    connect(xSlider, SIGNAL(valueChanged(int)), this, SLOT(setXRotation(int)));
    connect(ySlider, SIGNAL(valueChanged(int)), this, SLOT(setYRotation(int)));
    connect(zSlider, SIGNAL(valueChanged(int)), this, SLOT(setZRotation(int)));
    connect(canvas,   SIGNAL(xRotChanged(int)), this, SLOT(setXRotation(int)));
    connect(canvas,   SIGNAL(yRotChanged(int)), this, SLOT(setYRotation(int)));
    connect(canvas,   SIGNAL(zRotChanged(int)), this, SLOT(setZRotation(int)));
   
    QGroupBox *rotationGroupBox = new QGroupBox(tr("Rotation"));
    QGridLayout *sliderLayout = new QGridLayout;
    sliderLayout->addWidget(xTitle,  0, 0);
    sliderLayout->addWidget(yTitle,  1, 0);
    sliderLayout->addWidget(zTitle,  2, 0);
    sliderLayout->addWidget(xSlider, 0, 1);
    sliderLayout->addWidget(ySlider, 1, 1);
    sliderLayout->addWidget(zSlider, 2, 1);
    sliderLayout->addWidget(xLabel,  0, 2);
    sliderLayout->addWidget(yLabel,  1, 2);
    sliderLayout->addWidget(zLabel,  2, 2);
    sliderLayout->setColumnMinimumWidth(2,32);
    rotationGroupBox->setLayout(sliderLayout);
    layout->addWidget(rotationGroupBox);

    QGroupBox *backgroundColorGroupBox = new QGroupBox(tr("Background"));
    QGridLayout *backgroundColorLayout = new QGridLayout;
    backgroundColorButton = new QPushButton(tr("Background Color"));
    backgroundColorButton->setToolTip(tr("Change the background color"));
    backgroundColorLayout->addWidget(backgroundColorButton, 0, 0, 1, 2);
    connect(backgroundColorButton, SIGNAL(clicked()), canvas, SLOT(setBackgroundColor()));
    backgroundColorLayout->addWidget(new QLabel(tr("Background Opacity")), 1, 0);
    backgroundOpacitySpinBox = new QSpinBox;
    backgroundOpacitySpinBox->setToolTip(tr("Changes how opaque the background is"));
    backgroundOpacitySpinBox->setSuffix("%");
    backgroundOpacitySpinBox->setValue(0);
    backgroundOpacitySpinBox->setRange(0,100);
    connect(backgroundOpacitySpinBox, SIGNAL(valueChanged(int)), canvas, SLOT(setBackgroundOpacity(int)));
    backgroundColorLayout->addWidget(backgroundOpacitySpinBox, 1, 1);
    backgroundColorGroupBox->setLayout(backgroundColorLayout);
    layout->addWidget(backgroundColorGroupBox);

    QGroupBox *zoomGroupBox = new QGroupBox(tr("Drawing Size"));
    QGridLayout *zoomLayout = new QGridLayout;
    QLabel *zoomTitle = new QLabel(tr("Zoom:"));
    zoomSpinBox = new QSpinBox();
    zoomSpinBox->setRange(0, 500);
    zoomSpinBox->setSuffix("%");
    zoomSpinBox->setAccelerated(true);
    zoomSpinBox->setValue(100);
    connect(zoomSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
    zoomLayout->addWidget(zoomTitle, 0, 0);
    zoomLayout->addWidget(zoomSpinBox, 0, 1);
    zoomGroupBox->setLayout(zoomLayout);
    layout->addWidget(zoomGroupBox);
    
    widget->setLayout(layout);
    return widget;	
}


QWidget *MainWindow::createAnnotationWidget()
{
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    
    QGroupBox *insertSymbolBox 	    = new QGroupBox(tr("Insert Symbol"));    
//    QGridLayout *insertSymbolLayout = new QGridLayout;
//    insertSymbolLayout->addWidget(insertAngstromAction, 0, 1);
//    insertSymbolLayout->addWidget(insertDegreeAction, 0, 1);
//    insertSymbolLayout->addWidget(insertPlusMinusAction, 0, 1);
//    insertSymbolBox->setLayout(insertSymbolLayout);
    insertSymbolBox->addAction(insertAngstromAction);
    insertSymbolBox->addAction(insertDegreeAction);
    insertSymbolBox->addAction(insertPlusMinusAction);
    layout->addWidget(insertSymbolBox);
    widget->addAction(insertPlusMinusAction);

    widget->setLayout(layout);
    return widget;	
}


QWidget *MainWindow::createBondsWidget()
{
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;

    // The bond labels    
    QGroupBox *bondLabelsGroupBox = new QGroupBox(tr("Labels"));
    QGridLayout *bondLabelsLayout = new QGridLayout;
    toggleBondLabelsButton = new QPushButton(tr("Toggle Bond Labels"));
    toggleBondLabelsButton->setToolTip(tr("Toggle the bond length labels of the selected bonds"));
    bondLabelsLayout->addWidget(toggleBondLabelsButton);
    bondLabelsGroupBox->setLayout(bondLabelsLayout);
    layout->addWidget(bondLabelsGroupBox);
    connect(toggleBondLabelsButton, SIGNAL(pressed()),
                canvas, SLOT(toggleBondLabels()));


    // The bond appearance
    QGroupBox *bondSizeWidget   = new QGroupBox(tr("Bond Appearance"));
    QGridLayout *bondSizeLayout = new QGridLayout;

    // The bond dashing
    toggleBondDashingButton = new QPushButton(tr("Toggle Bond Dashing"));
    toggleBondDashingButton->setToolTip(tr("Toggle dashed / solid bonds for the selected bonds"));
    bondSizeLayout->addWidget(toggleBondDashingButton, 0, 0, 1, 2);
    connect(toggleBondDashingButton, SIGNAL(pressed()),
                canvas, SLOT(toggleBondDashing()));
    // The bond thickness
    QLabel *bondSizeLabel       = new QLabel("Bond thickness = ");
    bondSizeSpinBox             = new QDoubleSpinBox();
    bondSizeSpinBox->setSuffix(QString(" Angstrom"));
    bondSizeSpinBox->setDecimals(3);
    bondSizeSpinBox->setSingleStep(0.005);
    bondSizeSpinBox->setSpecialValueText(tr("Select Bonds"));
    bondSizeSpinBox->setValue(bondSizeSpinBox->minimum());
    bondSizeSpinBox->setAccelerated(true);
    bondSizeLayout->addWidget(bondSizeLabel, 1, 0);
    bondSizeLayout->addWidget(bondSizeSpinBox, 1, 1);
    connect(bondSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeBondSize()));
    bondSizeWidget->setLayout(bondSizeLayout);
    layout->addWidget(bondSizeWidget);

    widget->setLayout(layout);
    return widget;	
}


QWidget *MainWindow::createAtomsWidget()
{
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    
    QGroupBox *atomSizeGroupBox = new QGroupBox(tr("Size"));
    QGridLayout *atomSizeLayout = new QGridLayout;
    QLabel *atomSizeLabel       = new QLabel("Atom size = ");
    atomSizeSpinBox             = new QDoubleSpinBox();
    atomSizeSpinBox->setSuffix(QString(" vdW radii"));
    atomSizeSpinBox->setDecimals(2);
    atomSizeSpinBox->setSingleStep(0.02);
    atomSizeSpinBox->setSpecialValueText(tr("Select Atoms"));
    atomSizeSpinBox->setValue(atomSizeSpinBox->minimum());
    atomSizeSpinBox->setAccelerated(true);
    atomSizeLayout->addWidget(atomSizeLabel, 0, 0);
    atomSizeLayout->addWidget(atomSizeSpinBox, 0, 1);
    connect(atomSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeAtomSize()));
    atomSizeGroupBox->setLayout(atomSizeLayout);
    layout->addWidget(atomSizeGroupBox);


    QGroupBox *drawingStyleBox 	    = new QGroupBox(tr("Drawing Style"));    
    atomDrawingStyleButtonGroup     = new QButtonGroup;
    QGridLayout *drawingStyleLayout = new QGridLayout;
    simpleAtomDrawingButton 	    = new QRadioButton(tr("Simple"));
    simpleAtomDrawingButton->setChecked(true);
    houkMolAtomDrawingButton 	    = new QRadioButton(tr("HoukMol"));
    simpleColoredAtomDrawingButton 	= new QRadioButton(tr("Simple Colored"));
    atomDrawingStyleButtonGroup->addButton(simpleAtomDrawingButton, int(Atom::Simple));
    atomDrawingStyleButtonGroup->addButton(simpleColoredAtomDrawingButton, int(Atom::SimpleColored));
    atomDrawingStyleButtonGroup->addButton(houkMolAtomDrawingButton, int(Atom::HoukMol));
    drawingStyleLayout->addWidget(simpleAtomDrawingButton, 0, 0);
    drawingStyleLayout->addWidget(simpleColoredAtomDrawingButton, 0, 1);
    drawingStyleLayout->addWidget(houkMolAtomDrawingButton, 0, 2);
    connect(atomDrawingStyleButtonGroup, SIGNAL(buttonClicked(int)),
                 canvas, SLOT(setAtomDrawingStyle(int)));
    drawingStyleBox->setLayout(drawingStyleLayout);
    layout->addWidget(drawingStyleBox);
    
    // The atom labels
    QGroupBox *labelStyleBox 	    = new QGroupBox(tr("Atom Label"));    
    QGridLayout *labelStyleLayout   = new QGridLayout;
    
    // The atom numbers as label subscripts    
    toggleAtomNumberSubscriptsButton = new QPushButton(tr("Toggle Atom Number Subscripts"));
    toggleAtomNumberSubscriptsButton->setToolTip(tr("Add/remove atom numbers as a subscript to the selected atoms"));
    labelStyleLayout->addWidget(toggleAtomNumberSubscriptsButton, 0, 0);
    connect(toggleAtomNumberSubscriptsButton, SIGNAL(pressed()),
                canvas, SLOT(toggleAtomNumberSubscripts()));
    // The label text
    atomLabelInput = new QLineEdit;
	atomLabelInput->setText(tr("Select Atoms"));
	atomLabelInput->setToolTip(tr("Text entered here will be used as the label for the selected atom(s).  Anything appended after an underscore will be used as a subscript, anything after a carat is a superscript"));
    labelStyleLayout->addWidget(atomLabelInput, 1, 0);
    connect(atomLabelInput, SIGNAL(returnPressed()), this, SLOT(setAtomLabels()));
    // The label font
    atomLabelFontCombo = new QFontComboBox();
    atomLabelFontCombo->setEditText(tr("Select Atoms"));
    atomLabelFontCombo->setToolTip(tr("The font for the selected atoms"));
    labelStyleLayout->addWidget(atomLabelFontCombo, 2, 0);
    connect(atomLabelFontCombo, SIGNAL(currentFontChanged(const QFont &)),
            canvas, SLOT(atomLabelFontChanged(const QFont &)));
    // The label font size
    atomLabelFontSizeCombo = new QComboBox;
    atomLabelFontSizeCombo->setEditable(true);
    atomLabelFontSizeCombo->setToolTip(tr("The font size for the selected atoms"));
    for (int i = 4; i < 20; ++i){
      atomLabelFontSizeCombo->addItem(QString().setNum(i));
    }
    atomLabelFontSizeCombo->setEditText(tr("Select Atoms"));
    labelStyleLayout->addWidget(atomLabelFontSizeCombo, 3, 0);
    connect(atomLabelFontSizeCombo, SIGNAL(currentIndexChanged(const QString &)),
            canvas, SLOT(atomLabelFontSizeChanged(const QString &)));
    
    labelStyleBox->setLayout(labelStyleLayout);
    layout->addWidget(labelStyleBox);    
    
    widget->setLayout(layout);
    return widget;	
}


QWidget *MainWindow::createAnglesWidget()
{
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;

    //The labels
    QGroupBox *labelsGroupBox = new QGroupBox(tr("Labels"));
    QGridLayout *labelsGroupBoxLayout = new QGridLayout;
    toggleAngleLabelsButton = new QPushButton(tr("Toggle Angle Labels"));
    toggleAngleLabelsButton->setToolTip(tr("Select three or more atoms to toggle the angle markers and labels.  Only angles between bonds will be drawn"));
    connect(toggleAngleLabelsButton, SIGNAL(pressed()),
                    canvas, SLOT(toggleAngleLabels()));
    labelsGroupBoxLayout->addWidget(toggleAngleLabelsButton, 0, 0);
    labelsGroupBox->setLayout(labelsGroupBoxLayout);
    
    layout->addWidget(labelsGroupBox);
    
    widget->setLayout(layout);
    return widget;	
}


QSlider *MainWindow::createSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(-180, 180);
    slider->setSingleStep(1);
    slider->setPageStep(15);
    return slider;		
}




void MainWindow::setAddArrowMode()
{
  mouseModeButtonGroupClicked((int) DrawingCanvas::AddArrow);
}


void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    
    
    insertMenu = menuBar()->addMenu(tr("&Insert"));
    insertMenu->addAction(addArrowAction);
    //insertMenu->addAction(addBondAction);
    //insertMenu->addAction(addArrowAction);
    insertSymbolMenu = insertMenu->addMenu(tr("Special Symbol"));
    insertSymbolMenu->addAction(insertAngstromAction);
    insertSymbolMenu->addAction(insertDegreeAction);
    insertSymbolMenu->addAction(insertPlusMinusAction);
    
}




void MainWindow::setAtomLabels()
{
	canvas->setAtomLabels(atomLabelInput->text());
}


//QToolButton *MainWindow::makeAtomButton(const char *label)
//{
//    int dimension1 = 14;
//    int dimension2 = 12;
//	
//    QSize icon_box(dimension1, dimension2);
//	// Start by drawing the button icon
//	QPixmap icon(icon_box);
//    icon.fill(Qt::transparent);
//    QPainter painter(&icon);
//    QString text(label);
//	QFont font;
//	font.setPointSize(12);
//	painter.setFont(font);
//	painter.drawText(QRectF(0, 0, dimension1, dimension2), Qt::AlignCenter, text); 
//	
//	// Now make the button and set it up
//	QToolButton *button = new QToolButton;
//	button->setIcon(icon);
//	button->setText(text);
//    button->setIconSize(icon_box);
//    button->setCheckable(true);
//    periodicTableButtonGroup->addButton(button);
//    
//    return button;
//}
//

//QWidget *MainWindow::createPeriodicTable()
//{
//    QGridLayout *layout = new QGridLayout;
//    layout->setSpacing(0);
//    
//    layout->addWidget(makeAtomButton("H" ), 0, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("He"), 0, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Li"), 1, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Be"), 1, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("B" ), 1, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("C" ), 1, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("N" ), 1, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("O" ), 1, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("F" ), 1, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ne"), 1, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Na"), 2, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Mg"), 2, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Al"), 2, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Si"), 2, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("P" ), 2, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("S" ), 2, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cl"), 2, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ar"), 2, 9, Qt::AlignHCenter);
//    
//    layout->addWidget(makeAtomButton("K" ), 3, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ca"), 3, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ga"), 3, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ge"), 3, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("As"), 3, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Se"), 3, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Br"), 3, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Kr"), 3, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Rb"), 4, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Sr"), 4, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("In"), 4, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Sn"), 4, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Sb"), 4, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Te"), 4, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("I" ), 4, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Xe"), 4, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Cs"), 5, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ba"), 5, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Tl"), 5, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pb"), 5, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Bi"), 5, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Po"), 5, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("At"), 5, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Rn"), 5, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Fr"), 6, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ra"), 6, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ac"), 6, 1, Qt::AlignHCenter);
//
//    layout->addItem(new QSpacerItem(10,10), 7, 1);
//
//    layout->addWidget(makeAtomButton("Sc"), 8, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ti"), 8, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("V" ), 8, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cr"), 8, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Mn"), 8, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Fe"), 8, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Co"), 8, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ni"), 8, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cu"), 8, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Zn"), 8, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Y" ), 9, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Zr"), 9, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Nb"), 9, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Mo"), 9, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Tc"), 9, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ru"), 9, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Rh"), 9, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pd"), 9, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ag"), 9, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cd"), 9, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("La"), 10, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Hf"), 10, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ta"), 10, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("W" ), 10, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Re"), 10, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Os"), 10, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ir"), 10, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pt"), 10, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Au"), 10, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Hg"), 10, 9, Qt::AlignHCenter);
    
//    layout->addItem(new QSpacerItem(10,10), 11, 1);
//
//    layout->addWidget(makeAtomButton("Ce"), 12, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pr"), 12, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Nd"), 12, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pm"), 12, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Sm"), 12, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Eu"), 12, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Gd"), 12, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Tb"), 12, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Dy"), 12, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ho"), 12, 9, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Er"), 12, 10, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Tm"), 12, 11, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Yb"), 12, 12, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Lu"), 12, 13, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Th"), 13, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pa"), 13, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("U" ), 13, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Np"), 13, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pu"), 13, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Am"), 13, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cm"), 13, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Bk"), 13, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cf"), 13, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Es"), 13, 9, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Fm"), 13, 10, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Md"), 13, 11, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("No"), 13, 12, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Lr"), 13, 13, Qt::AlignHCenter);

//    QWidget *widget = new QWidget;
//    widget->setLayout(layout);
//
//    return widget;
//	
//}


void MainWindow::updateMenus()
// TODO Add text controls
{
	// The aim here is to what the properties of the selected items are.  If they are all the
	// same, the menus should be updated to reflect that setting.  If the selected items have 
	// different properties, the menus should appear blank to indicate this.  This is easy to do
	// in most cases, but comboboxes don't seem to have an easy mechanism for setting text, so 
	// a hack is used, involving the special text property, which is displayed when the minimum
	// value is reached.
	QList<double> atomScaleFactors;
	QList<double> bondScaleFactors;
	QList<QString> atomLabels;
	QList<QString> atomLabelFonts;
	QList<int> atomLabelFontSizes;
	
	foreach(QGraphicsItem *item, canvas->selectedItems()){
		if(item->type() == Bond::Type){
			Bond *bond = dynamic_cast<Bond*>(item);
			bondScaleFactors.append(bond->thickness());
		}
		if(item->type() == Atom::Type){
			Atom *atom = dynamic_cast<Atom*>(item);
			atomScaleFactors.append(atom->scaleFactor());
			atomLabels.append(atom->label());
			atomLabelFonts.append(atom->labelFont().family());
			atomLabelFontSizes.append(atom->fontSize());
		}
	}

	// Make the atom size menu appear correctly
	if(atomScaleFactors.size()){
		bool sameValues = true;
		foreach(double i, atomScaleFactors){
			if(fabs(i - atomScaleFactors[0])>TINY){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			atomSizeSpinBox->setSpecialValueText(tr(""));
			atomSizeSpinBox->setValue(atomScaleFactors[0]);
		}else{
			atomSizeSpinBox->setSpecialValueText(tr("  "));
			atomSizeSpinBox->setValue(atomSizeSpinBox->minimum());
		}
	}else{
		atomSizeSpinBox->setSpecialValueText(tr("Select Atoms"));
		atomSizeSpinBox->setValue(atomSizeSpinBox->minimum());
	}
	
	// Make the atom labels menu appears correctly
	if(atomLabels.size()){
		bool sameValues = true;
		foreach(QString i, atomLabels){
			if(i != atomLabels[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			atomLabelInput->setText(atomLabels[0]);
		}else{
			atomLabelInput->setText("");
		}
	}else{
		atomLabelInput->setText(tr("Select Atoms"));
	}

	// Make the bond thickness menu appears correctly
	if(bondScaleFactors.size()){
		bool sameValues = true;
		foreach(double i, bondScaleFactors){
			if(fabs(i - bondScaleFactors[0])>TINY){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			bondSizeSpinBox->setSpecialValueText(tr(""));
			bondSizeSpinBox->setValue(bondScaleFactors[0]);
		}else{
			bondSizeSpinBox->setSpecialValueText(tr("  "));
			bondSizeSpinBox->setValue(bondSizeSpinBox->minimum());
		}
	}else{
		bondSizeSpinBox->setSpecialValueText(tr("Select Bonds"));
		bondSizeSpinBox->setValue(bondSizeSpinBox->minimum());
	}

	// Make the atom labels font menu appears correctly
	if(atomLabelFonts.size()){
		bool sameValues = true;
		foreach(QString i, atomLabelFonts){
			if(i != atomLabelFonts[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			atomLabelFontCombo->setEditText(atomLabelFonts[0]);
		}else{
			atomLabelFontCombo->setEditText(tr(""));
		}
	}else{
		atomLabelFontCombo->setEditText(tr("Select Atoms"));
	}

	// Make the atom labels font size menu appears correctly
	if(atomLabelFontSizes.size()){
		bool sameValues = true;
		foreach(int i, atomLabelFontSizes){
			if(i != atomLabelFontSizes[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			atomLabelFontSizeCombo->setEditText(QString().setNum(atomLabelFontSizes[0]));
		}else{
			atomLabelFontSizeCombo->setEditText(tr(""));
		}
	}else{
		atomLabelFontSizeCombo->setEditText(tr("Select Atoms"));
	}
}


void MainWindow::changeAtomSize()
{
	if(atomSizeSpinBox->value() == atomSizeSpinBox->minimum()) return;

	if(atomSizeSpinBox->specialValueText().size()){
		atomSizeSpinBox->setSpecialValueText(tr(""));
		atomSizeSpinBox->setValue(DEFAULT_ATOM_SCALE_FACTOR);
	}else{
		QGraphicsItem *item;
		foreach(item, canvas->selectedItems()){
			if(item->type() == Atom::Type){
				Atom *atom = dynamic_cast<Atom*>(item);
				atom->setScaleFactor(atomSizeSpinBox->value()); 
			}
		}
		canvas->refresh();
	}	
}


void MainWindow::changeBondSize()
{
	if(bondSizeSpinBox->value() == bondSizeSpinBox->minimum()) return;

	if(bondSizeSpinBox->specialValueText().size()){
		bondSizeSpinBox->setSpecialValueText(tr(""));
		bondSizeSpinBox->setValue(DEFAULT_BOND_THICKNESS);
	}else{
		QGraphicsItem *item;
		foreach(item, canvas->selectedItems()){
			if(item->type() == Bond::Type){
				Bond *bond = dynamic_cast<Bond*>(item);
				bond->setThickness(bondSizeSpinBox->value()); 
			}
		}
		canvas->refresh();
	}	
}

void MainWindow::changeZoom(int val)
{
    drawingInfo->setZoom(val);
    canvas->refresh();
}
